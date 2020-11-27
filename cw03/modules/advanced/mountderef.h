#ifndef MOUNTDEREF_H
#define MOUNTDEREF_H

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

extern struct miscdevice mountderef_dev;

extern const struct file_operations mountderef_fops;

int mountderef_init(void);

void mountderef_exit(void);

ssize_t mountderef_read(
    struct file* filp, char __user* user_buf, size_t count, loff_t* f_pos);

ssize_t mountderef_write(struct file* filp, const char __user* user_buf,
    size_t count, loff_t* f_pos);

#endif