#include "fs.h"
#include "io.h"
#include "tmpfs.h"
#include "fat32.h"
#include "string.h"
#include "allocator.h"

#define xp_func(_path, opset, op, ...) \
  *_path == '/' ? rootfs->root->opset->op( \
      rootfs->root, ## __VA_ARGS__, _path) : \
      current_task->pwd->opset->op( \
          current_task->pwd, ## __VA_ARGS__, _path)

struct mount *rootfs = NULL;

struct filesystem *regedfs = NULL;

struct vnode *move_mount_root(struct vnode *node){
  if(node && node->mount && node->mount->root != node)
    return node->mount->root;
  return node;
}

struct mount *newMnt(struct vnode *mp, struct vnode *root){
  struct mount *newmnt =
    (struct mount*)kmalloc(sizeof(struct mount));
  newmnt->mp = mp;
  newmnt->root = root;
  newmnt->fs = NULL;
  return newmnt;
}

struct file *newFd(struct vnode *vnode, int flags){
  if(!vnode) return NULL;
  struct file *fd = (struct file*)kmalloc(sizeof(struct file));
  fd->vnode = vnode, fd->flags = flags;
  fd->f_pos = 0, fd->f_ops = fd->vnode->f_ops;
  return fd;
}

struct vnode *newVnode(
    struct mount *mount,
    struct vnode_operations *vops,
    struct file_operations *fops, 
    struct directory_operations *dops, 
    void *internal){
  struct vnode *node = (struct vnode *)kmalloc(sizeof(struct vnode));
  node->mount = mount;
  node->v_ops = vops;
  node->f_ops = fops;
  node->d_ops = dops;
  node->internal = internal;
  return node;
}

int register_filesystem(struct filesystem *fs) {
  // register the file system to the kernel.
  fs->nextfs = regedfs;
  regedfs = fs;
  // you can also initialize memory pool 
  // of the file system here.
  return 0;
}

struct filesystem *find_fs(const char *name){
  struct filesystem *fs = regedfs;
  while(fs){
    if(EQS(fs->name, name)) break;
    fs = fs->nextfs;
  }
  return fs;
}

struct file *vfs_open(const char *pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  struct vnode *target = 0;
  xp_func(pathname, v_ops, lookup, &target);

  target = move_mount_root(target);

  if(target && target->v_ops->typeof(target) != dirent_file){
    return NULL;
  }
  // 2. Create a new file descriptor for this vnode if found.
  if(!target && flags & O_CREAT)
    xp_func(pathname, v_ops, create, &target);
  // 3. Create a new file if O_CREAT is specified in flags.
  return newFd(target, flags);
}

int vfs_close(struct file *file) {
  // 1. release the file descriptor
  if(file) kfree(file);
  return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  return file->vnode->f_ops->write(file, buf, len);
  // 2. return written size or error code if an error occurs.
}

int vfs_read(struct file *file, void *buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  return file ? file->vnode->f_ops->read(file, buf, len) : 0;
  // 2. return read size or error code if an error occurs.
  return 0;
}

void vfs_show(){
  tmpfs_show_tree(rootfs->root, 0);
}

DIR *vfs_opendir(const char *pathname){
  DIR *dir = (DIR*)kmalloc(sizeof(DIR));
  if(xp_func(pathname, d_ops, opendir, dir)){
    return dir;
  }
  if(dir) kfree(dir);
  return NULL;
}

DIR *vfs_opendir_by_node(
    struct vnode *node,
    const char *pathname){
  DIR *dir = (DIR*)kmalloc(sizeof(DIR));
  if(node->d_ops->opendir(node, dir, pathname)){
    return dir;
  }
  if(dir) kfree(dir);
  return NULL;
}



dirent *vfs_readdir(DIR *dir){
  return dir->dops->readdir(dir);
}

void vfs_closedir(DIR *dir){
  if(dir && dir->path) kfree(dir->path);
  if(dir) kfree(dir);
}

int vfs_mkdir(const char *pathname){
  int len = strlen(pathname), name = 0;
  char path[len], *p = path + len - 1, *newp = NULL;
  strcpy(path, pathname);
  while(p > path){
    if(*p == ' ') p--;
    else if(*p == '/'){
      if(name) break;
      else p--;
    }
    else name = 1, p--;
  }
  if(p != path) *p = 0, newp = p + 1, p = path;
  else newp = path, p = path + len;
  while(*newp == '/') newp++;

  struct vnode *target;
  if(xp_func(pathname, v_ops, lookup, &target)){
    // existed directory
    return 0;
  }
  if(xp_func(p, v_ops, lookup, &target)){
    target = move_mount_root(target);
    return target->d_ops->mkdir(target, newp);
  }
  return 0;
}

int vfs_chdir(const char *path){
  return xp_func(path, d_ops, chdir);
}

int vfs_mount(
    const char *dev, const char *mpt, const char *fs){

  struct vnode *dev_vnode = 0, *mpt_vnode = 0;
  xp_func(dev, v_ops, lookup, &dev_vnode);

  dev_vnode = move_mount_root(dev_vnode);

  struct filesystem *devfs = find_fs(dev);
  if(devfs && devfs->mnt) dev_vnode = devfs->mnt->root;

  xp_func(mpt, v_ops, lookup, &mpt_vnode);

  mpt_vnode = move_mount_root(mpt_vnode);

  if(mpt_vnode){
    mpt_vnode->mount = newMnt(mpt_vnode, dev_vnode);
#if 0
    printfmt("new mount %x", mpt_vnode->mount);
#endif
    struct filesystem *newfs = find_fs(fs);
    if(newfs) newfs->setup_mount(newfs, mpt_vnode->mount);
    else { puts("cannot find fs"); return 0; }
  } else { puts("cannot find mpt vnode"); return 0; }
  return 1;
}

int vfs_umount(const char *mpt){
  struct vnode *mpt_vnode = 0;
  xp_func(mpt, v_ops, lookup, &mpt_vnode);
  if(mpt_vnode && mpt_vnode->mount){
    printfmt("root mount %x", (rootfs));
    printfmt("release mount %x", (mpt_vnode->mount));
    kfree(mpt_vnode->mount);
    mpt_vnode->mount = NULL;
    return 1;
  }
  return 0;
  //return xp_func(mpt, v_ops, umount);
}

void indent(int n){
  for(int i = 0; i < n * 2; i ++){
    printf(" ");
  }
}

void list_dir(DIR *dir, int lv){
  if(lv > 5){
    indent(lv);
    puts("...");
    return;
  }
  dirent *entry;
  indent(lv);
  printfmt("{%s}", dir->path);
  while((entry = vfs_readdir(dir))){
    if(entry->type == dirent_dir){
      DIR *subd = vfs_opendir_by_node(
          dir->root, entry->name);
      list_dir(subd, lv + 1);
      vfs_closedir(subd);
    }
    else{
      indent(lv + 1);
      printf("<%s>" NEWLINE, entry->name);
    }
  }
}

void fs_init(){
  register_filesystem(tmpfs);
  tmpfs->setup_mount(tmpfs, rootfs = newMnt(NULL, NULL));
  register_filesystem(fat32);
}

int subpath_of(const char *sub, const char *full, int (*cmp)(char a, char b)){
  while(*sub){
    if(cmp && cmp(*full, *sub)) full++, sub++;
    else if(*full == *sub) full++, sub++;
    else break;
  }
  return (*full == '/' || *full == 0) && *sub == 0;
}

int exist_slash(const char *path){
  while(*path)
    if(*path == '/') return 1;
    else path++;
  return 0;
}
