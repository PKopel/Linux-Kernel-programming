#include "prname.h"

char* prname_buf;

struct miscdevice prname_dev;

const struct file_operations prname_fops;

int prname_init(void)
{
        int result = 0;

        result = misc_register(&prname_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/prname device\n");
                goto err;
        }
err:
        misc_deregister(&prname_dev);
        return result;
}

void prname_exit(void) { misc_deregister(&prname_dev); }

ssize_t prname_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{
        size_t to_copy = strlen(mybuf);

        printk(KERN_WARNING "ADVANCED: read f_pos is %lld\n", *f_pos);

        if (*f_pos >= to_copy) {
                return 0;
        }

        if (copy_to_user(user_buf, mybuf, to_copy)) {
                printk(KERN_WARNING "ADVANCED: could not copy data to user\n");
                return -EFAULT;
        }
        read_count++;

        *f_pos += to_copy;
        return to_copy;
}

ssize_t prname_write(
    struct file* filp, const char __user* user_buf, size_t count, loff_t* f_pos)
{
        printk(KERN_WARNING "ADVANCED: write f_pos is %lld\n", *f_pos);

        // Cannot write more than buffer size (+ '\0')
        if (*f_pos >= MYBUF_SIZE - 1) {
                return -ENOSPC;
        }
        if (*f_pos + count > MYBUF_SIZE - 1) {
                count = MYBUF_SIZE - 1 - *f_pos;
        }
        if (copy_from_user(mybuf + *f_pos, user_buf, count)) {
                printk(
                    KERN_WARNING "ADVANCED: could not copy data from user\n");
                return -EFAULT;
        }
        mybuf[count] = '\0';
        write_count++;
        *f_pos += count;
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