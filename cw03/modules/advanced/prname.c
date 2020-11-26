#include "prname.h"

char* prname_buf;

struct miscdevice prname_dev;

const struct file_operations prname_fops