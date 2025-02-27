#include "advanced.h"

#define DEVICE_NAME "jiffies"

const char* const jiffies_text = "Current jiffies number: %lu\n";

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
                goto out;
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
        .name = DEVICE_NAME,
};

const struct file_operations jiffies_fops = {
        .owner = THIS_MODULE,
        .read = jiffies_read,
};
