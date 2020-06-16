#ifndef __VFS_H__
#define __VFS_H__

#include "type.h"

struct vnode {
  struct mount *mount;
  struct vnode_operations *v_ops;
  struct file_operations *f_ops;
  void *internal;
};

struct file {
  struct vnode *vnode;
  size_t f_pos;
  // The next read/write position of this file descriptor
  struct file_operations *f_ops;
  int flags;
};

struct mount {
  struct vnode *root;
  struct filesystem *fs;
};

struct filesystem {
  const char *name;
  int (*setup_mount)(struct filesystem *fs, struct mount *mount);
  struct filesystem *nextfs;
};

struct file_operations {
  int (*write) (struct file *file, const void *buf, size_t len);
  int (*read) (struct file *file, void *buf, size_t len);
  int (*readdir) (struct file *file, void *buf, size_t len);
};

struct vnode_operations {
  int (*lookup)(struct vnode *dir_node, struct vnode **target, const char *component_name);
  int (*create)(struct vnode *dir_node, struct vnode **target, const char *component_name);
};

extern struct mount *rootfs;
int register_filesystem(struct filesystem *fs);
struct file *vfs_open(const char *pathname, int flags);
int vfs_close(struct file *file);
int vfs_write(struct file *file, const void *buf, size_t len);
int vfs_read(struct file *file, void *buf, size_t len);
void vfs_setup();
void vfs_show();

struct mount *newMnt();

struct vnode *newVnode(
    struct mount *mount,
    struct vnode_operations *vops,
    struct file_operations *fops, 
    void *internal);

#define O_CREAT 0b1
#define EOF     4

void fs_init();
#endif