#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(pidtoname, pid_t, pid, char __user*, namebuf)
{
        struct task_struct* process;
        struct pid* pid_struct;
        char comm[TASK_COMM_LEN];
        long result = 0;

        pid_struct = find_get_pid(pid);
        if (!pid_struct)
                return -EINVAL;

        process = pid_task(pid_struct, PIDTYPE_PID);
        if (!process) {
                result = -ESRCH;
                goto out;
        }
        get_task_comm(comm, process);

        if (copy_to_user(namebuf, comm, TASK_COMM_LEN))
                result = -EFAULT;

out:
        free_pid(pid_struct);
        return 0;
}