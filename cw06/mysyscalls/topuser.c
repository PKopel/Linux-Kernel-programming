#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/user.h>

SYSCALL_DEFINE1(topuser, uid_t __user*, uid_result)
{
        struct task_struct* process;
        kuid_t top_kuid, current_kuid;
        uint top_count = 0, current_count;
        struct user_struct* current_user;
        long result = 0;

        rcu_read_lock();
        for_each_process(process)
        {
                current_kuid = task_uid(process);
                current_user = find_user(current_kuid);
                if (current_user) {
                        current_count = atomic_read(&(current_user->processes));
                        if (top_count < current_count) {
                                top_kuid = current_user->uid;
                                top_count = current_count;
                        }
                        free_uid(current_user);
                }
        }
        rcu_read_unlock();

        if (copy_to_user(uid_result, &top_kuid.val, sizeof(uid_t)))
                result = -EFAULT;

        return result;
}