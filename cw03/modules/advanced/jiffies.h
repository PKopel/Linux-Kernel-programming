#ifndef JIFFIES_H
#define JIFFIES_H

#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

extern struct miscdevice jiffies_dev;

extern const struct file_operations jiffies_fops;

ssize_t jiffies_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos);

#endif