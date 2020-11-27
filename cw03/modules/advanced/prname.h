#ifndef PRNAME_H
#define PRNAME_H

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/uaccess.h>

extern struct miscdevice prname_dev;

extern const struct file_operations prname_fops;

ssize_t prname_write(struct file* filp, const char __user* user_buf,
    size_t count, loff_t* f_pos);

ssize_t prname_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos);

#endif