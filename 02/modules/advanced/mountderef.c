#include "advanced.h"

#define DEVICE_NAME "mountderef"

struct path* mount_path;

void mountderef_exit(void)
{
        if (mount_path)
                path_put(mount_path);
        kvfree(mount_path);
        misc_deregister(&mountderef_dev);
}

ssize_t mountderef_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos)
{
        char *buf, *path_name;
        size_t length;
        ssize_t result = 0;

        if (!mount_path) {
                return -ENODATA;
        }

        buf = kvmalloc(count, GFP_KERNEL);
        if (!buf) {
                result = -ENOMEM;
                goto out;
        }

        path_name = d_path(mount_path, buf, count);

        if (IS_ERR(path_name)) {
                result = PTR_ERR(path_name);
                goto out;
        }

        length = strlen(path_name);

        if (*f_pos >= length || count <= length) {
                goto out;
        }

        if (copy_to_user(user_buf, path_name, length)) {
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

ssize_t mountderef_write(
    struct file* filp, const char __user* user_buf, size_t count, loff_t* f_pos)
{
        ssize_t result = 0;
        struct path* new_mount_path;
        char* buf;

        buf = kvmalloc(count + 1, GFP_KERNEL);

        if (!buf)
                return -ENOMEM;

        new_mount_path = kvmalloc(sizeof(struct path), GFP_KERNEL);

        if (!new_mount_path) {
                result = -ENOMEM;
                goto out_buf;
        }

        if (copy_from_user(buf, user_buf, count)) {
                result = -EFAULT;
                goto out_path;
        }

        buf[count] = '\0';

        result = kern_path(buf, LOOKUP_FOLLOW, new_mount_path);
        if (result)
                goto out_path;

        follow_up(new_mount_path);

        if (mount_path)
                path_put(mount_path);
        kvfree(mount_path);

        mount_path = new_mount_path;
        result = count;
        *f_pos = count;

out_buf:
        kvfree(buf);
        return result;

out_path:
        kvfree(new_mount_path);
        kvfree(buf);
        return result;
}

struct miscdevice mountderef_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &mountderef_fops,
        .mode = 0666,
        .name = DEVICE_NAME,
};

const struct file_operations mountderef_fops = {
        .owner = THIS_MODULE,
        .read = mountderef_read,
        .write = mountderef_write,
};