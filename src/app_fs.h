#ifndef _LUA_FS_H_
#define _LUA_FS_H_

#include <fs/fs.h>

extern struct fs_mount_t *mp;

int fs_init(void);

int fs_file_add(const char *name, const char *data, size_t size);

int fs_stats(void);

#endif /* _LUA_FS_H_ */