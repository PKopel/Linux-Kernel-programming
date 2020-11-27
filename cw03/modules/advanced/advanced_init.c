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

        result = prname_init();
        if (result) {
                printk(KERN_WARNING "Cannot init /dev/prname device\n");
                goto err;
        }

        result = jiffies_init();
        if (result) {
                printk(KERN_WARNING "Cannot init /dev/jiffies device\n");
                goto err;
        }
        /*
                result = mountderef_init();
                if (result) {
                        printk(KERN_WARNING "Cannot init /dev/mountderef
           device\n"); goto err;
                }
        */
        return result;

err:
        prname_exit();
        jiffies_exit();
        //        mountderef_exit();
        return result;
}

static void __exit advanced_exit(void)
{
        prname_exit();
        jiffies_exit();
        //        mountderef_exit();

        printk(KERN_INFO "The ADVANCED module has been removed\n");
}

module_init(advanced_init);
module_exit(advanced_exit);
