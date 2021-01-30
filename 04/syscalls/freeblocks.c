#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/statfs.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE2(
    freeblocks, const char __user*, u_path, u64 __user*, u_free_blocks)
{
        struct kstatfs stat;
        u64 free_blocks;
        long result = 0;

        result = user_statfs(u_path, &stat);
        if (result)
                return result;

        free_blocks = stat.f_bfree;

        if (copy_to_user(u_free_blocks, &free_blocks, sizeof(u64)))
                result = -EFAULT;

        return result;
}