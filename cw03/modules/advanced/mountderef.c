#include "advanced.h"

struct path* mount_path;

void mountderef_exit(void)
{
        if (mount_path && mount_path->mnt)
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
        char* new_pathname;
        char* buf;

        buf = kvmalloc(count + 1, GFP_KERNEL);

        if (!buf)
                return -ENOMEM;

        if (mount_path && mount_path->mnt == NULL) {
                path_put(mount_path);
        } else {
                mount_path = kvmalloc(sizeof(struct path), GFP_KERNEL);

                if (!mount_path) {
                        result = -ENOMEM;
                        goto out;
                }

                mount_path->mnt = NULL;
        }

        if (copy_from_user(buf, user_buf, count)) {
                result = -EFAULT;
                goto out;
        }

        buf[count] = '\0';

        result = kern_path(buf, LOOKUP_FOLLOW, mount_path);
        if (result)
                goto out;

        follow_up(mount_path);

        if (IS_ERR(new_pathname)) {
                result = PTR_ERR(new_pathname);
                goto out;
        }

        result = count;
        *f_pos = count;

out:
        kvfree(buf);
        return result;
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