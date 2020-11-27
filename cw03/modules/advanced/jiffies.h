#ifndef JIFFIES_H
#define JIFFIES_H

#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/uaccess.h>

int jiffies_init(void);

void jiffies_exit(void);

#endif