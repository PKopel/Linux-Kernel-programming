#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

struct miscdevice prname_dev, jiffies_dev, mountderef_dev;

const struct file_operations prname_fops, jiffies_fops, mountderef_fops;

static int __init advanced_init(void)
{
        int result = 0;

        result = misc_register(&prname_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/prname device\n");
                goto err;
        }

        result = misc_register(&jiffies_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/jiffies device\n");
                goto err;
        }

        result = misc_register(&mountderef_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/mountderef device\n");
                goto err;
        }

        return result;

err:
        misc_deregister(&prname_dev);
        misc_deregister(&jiffies_dev);
        misc_deregister(&mountderef_dev);
        return result;
}

static void __exit advanced_exit(void)
{
        misc_deregister(&prname_dev);
        misc_deregister(&jiffies_dev);
        misc_deregister(&mountderef_dev);

        printk(KERN_INFO "The ADVANCED module has been removed\n");
}

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

ssize_t jiffies_read(
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

struct miscdevice prname_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &prname_fops,
        .mode = 0666,
        .name = "prname",
};

struct miscdevice jiffies_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &jiffies_fops,
        .mode = 0444,
        .name = "jiffies",
};

struct miscdevice mountderef_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &mountderef_fops,
        .mode = 0666,
        .name = "mountderef",
};

const struct file_operations prname_fops = {
        .owner = THIS_MODULE,
        .read = prname_read,
        .write = prname_write,
};

const struct file_operations jiffies_fops = {
        .owner = THIS_MODULE,
        .read = jiffies_read,
};

const struct file_operations mountderef_fops = {
        .owner = THIS_MODULE,
        .read = mountderef_read,
        .write = mountderef_write,
};
module_init(advanced_init);
module_exit(advanced_exit);
