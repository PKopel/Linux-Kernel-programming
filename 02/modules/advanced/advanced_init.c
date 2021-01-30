#include "advanced.h"

MODULE_LICENSE("GPL");

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
                printk(KERN_WARNING "Cannot register /dev/circular device\n");
                goto err;
        }

        result = misc_register(&mountderef_dev);
        if (result) {
                printk(KERN_WARNING "Cannot register /dev/prname device\n");
                goto err;
        }

        printk(KERN_INFO "The ADVANCED module has been inserted\n");
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
        mountderef_exit();

        printk(KERN_INFO "The ADVANCED module has been removed\n");
}

module_init(advanced_init);
module_exit(advanced_exit);
