#include "prname.h"

const char* const prname_text = "Process name for pid %d: %s\n";

struct miscdevice prname_dev;

const struct file_operations prname_fops;

pid_t current_pid;

struct task_struct* current_process;

int prname_init(void)
{
        int result = 0;

        result = misc_register(&prname_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/prname device\n");
                goto err;
        }
        return result;
err:
        misc_deregister(&prname_dev);
        return result;
}

void prname_exit(void)
{
        misc_deregister(&prname_dev);
        printk(KERN_INFO "/dev/prname has been removed\n");
}

ssize_t prname_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{
        char* buf;
        size_t length;
        ssize_t result = 0;

        if (!current_process) {
                return -ENODATA;
        }

        buf = kvmalloc(50, GFP_KERNEL);
        if (!buf) {
                result = -ENOMEM;
                goto out;
        }

        length = snprintf(
            buf, 50, prname_text, current_pid, current_process->comm);

        if (*f_pos >= length) {
                return result;
        }

        if (copy_to_user(user_buf, buf, length)) {
                printk(KERN_WARNING
                    "ADVANCED-prname: could not copy data to user\n");
                result = -EFAULT;
                goto out;
        }

        result = count;
        *f_pos = length;

out:
        kvfree(buf);
        return result;
}

ssize_t prname_write(
    struct file* filp, const char __user* user_buf, size_t count, loff_t* f_pos)
{
        ssize_t result;
        pid_t new_pid;
        struct pid* pid_struct;

        result = kstrtoint_from_user(user_buf, count, 10, &new_pid);
        if (result)
                return result;

        if (new_pid < 1)
                return -EINVAL;

        pid_struct = find_get_pid(new_pid);
        if (!pid_struct)
                return -EINVAL;

        current_process = pid_task(pid_struct, PIDTYPE_PID);
        if (!current_process)
                return -EINVAL;

        current_pid = new_pid;
        return count;
}

struct miscdevice prname_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &prname_fops,
        .mode = 0666,
        .name = "prname",
};

const struct file_operations prname_fops = {
        .owner = THIS_MODULE,
        .read = prname_read,
        .write = prname_write,
};