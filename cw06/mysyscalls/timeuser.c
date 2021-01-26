#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

typedef struct user_time {
        kuid_t user_kuid;
        u64 time;
        struct hlist_node node;
} user_time;

void update_time(kuid_t user_kuid, u64 time, struct hlist_head* hlist)
{
        user_time *current_user_time, new_user_time;
        hlist_for_each_entry(current_user_time, hlist, node)
        {
                if (uid_eq(current_user_time->user_kuid, user_kuid)) {
                        current_user_time->time += time;
                        return;
                }
        }
        new_user_time.user_kuid = user_kuid;
        new_user_time.time = time;
        hlist_add_head(&(new_user_time.node), hlist);
}

kuid_t max_time_kuid(struct hlist_head* hlist)
{
        u64 max_time = 0;
        kuid_t max_time_kuid;
        user_time* current_user_time;
        hlist_for_each_entry(current_user_time, hlist, node)
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
        struct task_struct* task;
        u64 task_time;
        kuid_t top_kuid;
        struct hlist_head hlist;
        long result = 0;
        INIT_HLIST_HEAD(&hlist);

        rcu_read_lock();
        for_each_process(task)
        {
                top_kuid = task_uid(task);
                task_time = task->utime + task->stime;
                update_time(top_kuid, task_time, &hlist);
        }
        rcu_read_unlock();

        top_kuid = max_time_kuid(&hlist);

        if (copy_to_user(uid_result, &top_kuid.val, sizeof(uid_t)))
                result = -EFAULT;

        return result;
}