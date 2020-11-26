#ifndef PRNAME_H
#define PRNAME_H

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

int prname_init(void);

void prname_exit(void);

#endif