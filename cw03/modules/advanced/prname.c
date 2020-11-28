#include "advanced.h"

const char* const prname_text = "Process name for pid %d: %s\n";
static char comm[TASK_COMM_LEN];

pid_t current_pid;

struct task_struct* current_process;

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

        get_task_comm(comm, current_process);

        length = snprintf(buf, 50, prname_text, current_pid, comm);

        if (*f_pos >= length) {
                goto out;
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