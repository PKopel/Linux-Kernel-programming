#include "mountderef.h"

char* mountderef_buf;

struct miscdevice mountderef_dev;

const struct file_operations mountderef_fops;

int mountderef_init(void) { }

void mountderef_exit(void) { }

ssize_t mountderef_read(
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

ssize_t mountderef_write(
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

struct miscdevice mountderef_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &mountderef_fops,
        .mode = 0666,
        .name = "mountderef",
};

const struct file_operations mountderef_fops = {
        .owner = THIS_MODULE,
        .read = mountderef_read,
        .write = mountderef_write,
};