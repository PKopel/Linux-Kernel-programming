#include "mountderef.h"

char* mountderef_buf;
size_t buf_size = PATH_MAX;

int mountderef_init(void)
{
        int result = 0;

        result = misc_register(&mountderef_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/prname device\n");
                goto err;
        }

        mountderef_buf = kvmalloc(buf_size, GFP_KERNEL);
        if (!mountderef_buf) {
                result = -ENOMEM;
                goto err;
        }

        return result;
err:
        misc_deregister(&mountderef_dev);
        kvfree(mountderef_buf);
        return result;
}

void mountderef_exit(void)
{
        misc_deregister(&mountderef_dev);
        kvfree(mountderef_buf);
        printk(KERN_INFO "/dev/mountderef has been removed\n");
}

ssize_t mountderef_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{
        size_t to_copy = strlen(mountderef_buf);

        if (*f_pos >= to_copy) {
                return 0;
        }

        if (copy_to_user(user_buf, mountderef_buf, to_copy)) {
                printk(KERN_WARNING
                    "ADVANCED-mountderef: could not copy data to user\n");
                return -EFAULT;
        }

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