#ifndef MOUNTDEREF_H
#define MOUNTDEREF_H

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

int mountderef_init(void);

void mountderef_exit(void);

#endif