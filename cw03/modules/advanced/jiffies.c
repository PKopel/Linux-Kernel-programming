#include "jiffies.h"

const char* const jiffies_text = "Current jiffies number: %zu\n";

char* jiffies_buf;

struct miscdevice jiffies_dev;

const struct file_operations jiffies_fops;

int jiffies_init(void)
{
        int result = 0;

        result = misc_register(&jiffies_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/circular device\n");
                goto err;
        }
        return result;

err:
        misc_deregister(&jiffies_dev);
        return result;
}

void jiffies_exit(void)
{
        misc_deregister(&jiffies_dev);
        printk(KERN_INFO "/dev/jiffies has been removed\n");
}

ssize_t jiffies_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{

        char* buf;
        size_t length;
        ssize_t result = 0;

        buf = kvmalloc(50, GFP_KERNEL);
        if (!buf) {
                result = -ENOMEM;
                goto out;
        }

        length = snprintf(buf, 50, jiffies_text, jiffies);

        if (*f_pos >= length) {
                return result;
        }

        if (copy_to_user(user_buf, buf, length)) {
                printk(KERN_WARNING
                    "ADVANCED-jiffies: could not copy data to user\n");
                result = -EFAULT;
                goto out;
        }

        result = count;
        *f_pos = length;

out:
        kvfree(buf);
        return result;
}

struct miscdevice jiffies_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &jiffies_fops,
        .mode = 0444,
        .name = "jiffies",
};

const struct file_operations jiffies_fops = {
        .owner = THIS_MODULE,
        .read = jiffies_read,
};