#include "jiffies.h"
//#include "mountderef.h"
#include "prname.h"
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

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
        /*

        result = mountderef_init();
        if (result) {
                printk(KERN_WARNING "Cannot initiaize mountderef\n");
                goto err;
        }
        */
        return result;

err:
        misc_deregister(&prname_dev);
        misc_deregister(&jiffies_dev);
        return result;
}

static void __exit advanced_exit(void)
{
        misc_deregister(&prname_dev);
        misc_deregister(&jiffies_dev);
        //        mountderef_exit();

        printk(KERN_INFO "The ADVANCED module has been removed\n");
}

module_init(advanced_init);
module_exit(advanced_exit);
