#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

typedef struct user_time {
        kuid_t user_kuid;
        u64 time;
        struct list_head node;
} user_time;

void update_time(kuid_t user_kuid, u64 time, struct list_head* list)
{
        user_time *current_user_time, *new_user_time;
        list_for_each_entry(current_user_time, list, node)
        {
                if (uid_eq(current_user_time->user_kuid, user_kuid)) {
                        current_user_time->time += time;
                        return;
                }
        }
        new_user_time = kmalloc(sizeof(user_time), GFP_KERNEL);
        new_user_time->user_kuid = user_kuid;
        new_user_time->time = time;
        list_add_tail(&(new_user_time->node), list);
}

kuid_t max_time_kuid(struct list_head* list)
{
        u64 max_time = 0;
        kuid_t max_time_kuid;
        user_time* current_user_time;
        list_for_each_entry(current_user_time, list, node)
        {
                if (current_user_time->time > max_time) {
                        max_time = current_user_time->time;
                        max_time_kuid = current_user_time->user_kuid;
                }
        }
        return max_time_kuid;
}

SYSCALL_DEFINE1(timeuser, uid_t __user*, uid_result)
{
        struct task_struct* process;
        u64 task_time;
        kuid_t top_kuid;
        user_time* data;
        struct list_head list, *pos, *n;
        long result = 0;
        INIT_LIST_HEAD(&list);

        rcu_read_lock();
        for_each_process(process)
        {
                top_kuid = task_uid(process);
                task_time = process->utime + process->stime;
                update_time(top_kuid, task_time, &list);
        }
        rcu_read_unlock();

        top_kuid = max_time_kuid(&list);

        if (copy_to_user(uid_result, &top_kuid.val, sizeof(uid_t)))
                result = -EFAULT;

        list_for_each_safe(pos, n, &list)
        {
                data = list_entry(pos, user_time, node);
                list_del(&(data->node));
                kfree(data);
        }

        return result;
}