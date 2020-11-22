#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define CIRCULAR_MAJOR 199

size_t buf_size = 40;

const char* const text = "CIRCULAR. Read calls: %zu, Write calls: %zu\n";

size_t circular_buf_index;
char* circular_buf;
bool copied;

struct miscdevice circular_dev;

struct proc_dir_entry* proc_entry;

/* Operations for /dev/circular */
const struct file_operations circular_fops;

/* Operations for /proc/circular */
const struct file_operations proc_fops;

static int __init circular_init(void)
{
        int result = 0;

        proc_entry = proc_create("circular", 0000, NULL, &proc_fops);
        if (!proc_entry) {
                printk(KERN_WARNING "Cannot create /proc/circular\n");
                goto err;
        }

        result = misc_register(&circular_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register the "
                                    "/dev/circular device with major "
                                    "number: %d\n",
                    CIRCULAR_MAJOR);
                goto err;
        }

        circular_buf = kvmalloc(buf_size, GFP_KERNEL);
        if (!circular_buf) {
                result = -ENOMEM;
                goto err;
        } else {
                circular_buf[0] = '\0';
                result = 0;
                printk(KERN_INFO "The CIRCULAR module has been inserted.\n");
        }
        return result;

err:
        if (proc_entry) {
                proc_remove(proc_entry);
        }
        misc_deregister(&circular_dev);
        kvfree(circular_buf);
        return result;
}

static void __exit circular_exit(void)
{
        misc_deregister(&circular_dev);
        if (proc_entry) {
                proc_remove(proc_entry);
        }

        kvfree(circular_buf);

        printk(KERN_INFO "The CIRCULAR module has been removed\n");
}

ssize_t circular_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{
        size_t to_copy = min(count, buf_size - *f_pos);

        if (copy_to_user(user_buf, circular_buf + *f_pos, to_copy)) {
                printk(KERN_WARNING "CIRCULAR: could not copy data to user\n");
                return -EFAULT;
        }

        if (to_copy < count) {
                if (copy_to_user(
                        user_buf + to_copy, circular_buf, count - to_copy)) {
                        printk(KERN_WARNING
                            "CIRCULAR: could not copy data to user\n");
                        return -EFAULT;
                }
        }

        *f_pos += count % buf_size;
        return to_copy;
}

ssize_t circular_write(
    struct file* filp, const char __user* user_buf, size_t count, loff_t* f_pos)
{
        size_t head, tail;
        if (*f_pos + count > buf_size) {
                tail = *f_pos + count - buf_size;
                head = count - tail;
                if (copy_from_user(circular_buf + *f_pos, user_buf, head)) {
                        printk(KERN_WARNING "CIRCULAR: could not copy data "
                                            "from user\n");
                        return -EFAULT;
                }
                if (copy_from_user(circular_buf, user_buf + head, tail)) {
                        printk(KERN_WARNING "CIRCULAR: could not copy data "
                                            "from user\n");
                        return -EFAULT;
                }
        } else {
                if (copy_from_user(circular_buf + *f_pos, user_buf, count)) {
                        printk(KERN_WARNING "CIRCULAR: could not copy data "
                                            "from user\n");
                        return -EFAULT;
                }
        }

        *f_pos += count % buf_size;
        return count;
}

ssize_t circular_write_proc(
    struct file* filp, const char __user* user_buf, size_t count, loff_t* f_pos)
{
        int result;
        size_t new_size;
        char* new_buf;

        result = kstrtoul_from_user(user_buf, count, 10, &new_size);
        if (result) {
                return result;
        }

        if (new_size < 1) {
                return -EINVAL;
        }

        new_buf = kvmalloc(new_size, GFP_KERNEL);
        if (!new_buf) {
                return -ENOMEM;
        }

        if (new_size > buf_size) {
                memcpy(new_buf, circular_buf, buf_size);
                new_buf[buf_size] = '\0';
        } else {
                memcpy(new_buf, circular_buf, new_size);
        }

        kvfree(circular_buf);

        circular_buf = new_buf;
        buf_size = new_size;

        return count;
}

struct miscdevice circular_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &circular_fops,
        .mode = 0666,
        .name = "circular",
};

const struct file_operations circular_fops = {
        .owner = THIS_MODULE,
        .read = circular_read,
        .write = circular_write,
};

const struct file_operations proc_fops = {
        .owner = THIS_MODULE,
        .write = circular_write_proc,
};

module_init(circular_init);
module_exit(circular_exit);
