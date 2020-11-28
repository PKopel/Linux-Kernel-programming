#ifndef ADVANCED_H
#define ADVANCED_H

#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/namei.h>
#include <linux/pid.h>
#include <linux/uaccess.h>

/* mountderef device */

extern struct miscdevice mountderef_dev;

extern const struct file_operations mountderef_fops;

void mountderef_exit(void);

ssize_t mountderef_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos);

ssize_t mountderef_write(struct file* filp, const char __user* user_buf,
    size_t count, loff_t* f_pos);

/* jiffies device */

extern struct miscdevice jiffies_dev;

extern const struct file_operations jiffies_fops;

ssize_t jiffies_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos);

/* prname device */

extern struct miscdevice prname_dev;

extern const struct file_operations prname_fops;

ssize_t prname_write(struct file* filp, const char __user* user_buf,
    size_t count, loff_t* f_pos);

ssize_t prname_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos);

#endif