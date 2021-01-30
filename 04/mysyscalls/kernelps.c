#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>

long compute_length(__user size_t* u_length)
{
        struct task_struct* task;
        size_t length = 0;
        rcu_read_lock();
        for_each_process(task) length++;
        rcu_read_unlock();
        if (copy_to_user(u_length, &length, sizeof(size_t)))
                return -EFAULT;
        return 0;
}

SYSCALL_DEFINE2(kernelps, __user size_t*, u_length, __user char**, u_comm_table)
{

        struct task_struct* process;
        size_t length = 0, curr = 0;
        char(*comm_table)[TASK_COMM_LEN];
        long result = 0;

        if (u_comm_table == NULL)
                return compute_length(u_length);

        if (copy_from_user(&length, u_length, sizeof(size_t)))
                return -EFAULT;

        comm_table = kmalloc(TASK_COMM_LEN * length, GFP_KERNEL);
        if (!comm_table)
                return -ENOMEM;

        rcu_read_lock();
        for_each_process(process)
        {
                if (curr >= length) {
                        result = -EAGAIN;
                        goto out;
                }
                get_task_comm(comm_table[curr++], process);
        }
        rcu_read_unlock();

        for (curr = 0; curr < length; curr++) {
                if (copy_to_user(
                        u_comm_table[curr], comm_table[curr], TASK_COMM_LEN)) {
                        result = -EFAULT;
                        printk(KERN_ERR "copy to user err\n");
                        goto out;
                }
        }

out:
        kfree(comm_table);
        return result;
}