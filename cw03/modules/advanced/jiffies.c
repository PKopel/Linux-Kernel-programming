#include "jiffies.h"

char* jiffies_buf;

struct miscdevice jiffies_dev;

const struct file_operations jiffies_fops;

int jiffies_init(void) { }

void jiffies_exit(void) { }

ssize_t jiffies_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{

        if (copy_to_user(user_buf, mybuf, to_copy)) {
                printk(KERN_WARNING "ADVANCED: could not copy data to user\n");
                return -EFAULT;
        }
        read_count++;

        *f_pos += to_copy;
        return 0;
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