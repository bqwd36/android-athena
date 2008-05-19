/* drivers/android/ashmem.c
**
** Android / Anonymous Shared Memory Subsystem, ashmem
**
** Copyright (C) 2008 Google, Inc.
**
** Robert Love <rlove@google.com>
**
** This software is licensed under the terms of the GNU General Public
** License version 2, as published by the Free Software Foundation, and
** may be copied, distributed, and modified under those terms.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/security.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/uaccess.h>
#include <linux/personality.h>
#include <linux/mutex.h>
#include <linux/shmem_fs.h>
#include <linux/ashmem.h>

/*
 * ashmem_area - android shared memory area
 * Lifecycle: From our parent file's open() until its release()
 * Locking: Protected by `ashmem_mutex'
 * Big Note: Mappings do NOT pin this structure; it dies on close()
 */
struct ashmem_area {
	char name[ASHMEM_NAME_LEN];	/* optional name for /proc/pid/maps */
	struct list_head list;		/* list of all ashmem areas */
	struct file *file;		/* the shmem-based backing file */
	size_t size;			/* size of the mapping, in bytes */
	unsigned long prot_mask;	/* allowed prot bits, as vm_flags */
	unsigned int pin_count;		/* 0 unpinned, >0 pinned */
	unsigned int purged;		/* nonzero if purged during OOM */
};

/* list of all purgable ashmem area's, protected by ashmem_mutex */
static LIST_HEAD(asma_purgable_list);

/*
 * ashmem_mutex - protects the list of and each individual ashmem_area
 *
 * Lock Ordering: ashmex_mutex -> inode->i_mutex -> inode->i_alloc_sem
 */
static DEFINE_MUTEX(ashmem_mutex);

static struct kmem_cache *ashmem_cachep __read_mostly;

#define PROT_MASK	(PROT_EXEC | PROT_READ | PROT_WRITE)

/*
 * ashmem_add_purgable - add entry to the tail of the purgable list
 * Caller must hold ashmem_mutex.
 */
static inline void ashmem_add_purgable(struct ashmem_area *asma)
{
	WARN_ON(asma->pin_count || asma->purged);
	list_add_tail(&asma->list, &asma_purgable_list);
}

/*
 * ashmem_del_purgable - remove entry from purgable list
 * Caller must hold ashmem_mutex.
 */
static inline void ashmem_del_purgable(struct ashmem_area *asma)
{
	list_del_init(&asma->list);
}

static int ashmem_open(struct inode *inode, struct file *file)
{
	struct ashmem_area *asma;
	int ret;

	ret = nonseekable_open(inode, file);
	if (unlikely(ret))
		return ret;

	asma = kmem_cache_zalloc(ashmem_cachep, GFP_KERNEL);
	if (unlikely(!asma))
		return -ENOMEM;

	INIT_LIST_HEAD(&asma->list);
	asma->prot_mask = PROT_MASK;
	asma->pin_count = 1;
	file->private_data = asma;

	return 0;
}

static int ashmem_release(struct inode *ignored, struct file *file)
{
	struct ashmem_area *asma = file->private_data;

	mutex_lock(&ashmem_mutex);
	if (!asma->pin_count && !asma->purged)
		ashmem_del_purgable(asma);
	mutex_unlock(&ashmem_mutex);

	kmem_cache_free(ashmem_cachep, asma);

	return 0;
}

static int ashmem_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct ashmem_area *asma = file->private_data;
	int ret = 0;

	mutex_lock(&ashmem_mutex);

	/* user needs to SET_SIZE before mapping */
	if (unlikely(!asma->size)) {
		ret = -EINVAL;
		goto out;
	}

	/* requested protection bits must match our allowed protection mask */
	if (unlikely((vma->vm_flags & ~asma->prot_mask) & PROT_MASK)) {
		ret = -EPERM;
		goto out;
	}

	if (!asma->file) {
		char *name = asma->name ? : ASHMEM_NAME_DEF;
		struct file *vmfile;

		/* new mapping for this fd, allocate shmem file ... */
		vmfile = shmem_file_setup(name, asma->size, vma->vm_flags);
		if (unlikely(IS_ERR(vmfile))) {
			ret = PTR_ERR(vmfile);
			goto out;
		}

		/* ... and associate it with this ashmem instance */
		asma->file = vmfile;
	} else
		get_file(asma->file);

	shmem_set_file(vma, asma->file);
	vma->vm_flags |= VM_CAN_NONLINEAR;

out:
	mutex_unlock(&ashmem_mutex);
	return ret;
}

/*
 * ashmem_shrink - our cache shrinker, called from mm/vmscan.c :: shrink_slab
 *
 * 'nr_to_scan' is the number of objects (pages) to prune, or 0 to query how
 * many objects (pages) we have in total.
 *
 * 'gfp_mask' is the mask of the allocation that got us into this mess.
 *
 * Return value is the number of objects (pages) remaining, or -1 if we cannot
 * proceed without risk of deadlock (due to gfp_mask).
 *
 * We approximate LRU via least-recently-unpinned, jettisoning entire ashmem
 * regions LRU-wise one-at-a-time until we hit 'nr_to_scan' pages freed.
 */
static int ashmem_shrink(int nr_to_scan, gfp_t gfp_mask)
{
	struct ashmem_area *asma, *next;
	int pages = 0;

	/* We may block or recurse into fs code, so bail out if necessary */
	if (!(gfp_mask & __GFP_FS) && nr_to_scan)
		return -1;

	mutex_lock(&ashmem_mutex);
	list_for_each_entry_safe(asma, next, &asma_purgable_list, list) {
		struct inode *inode = asma->file->f_dentry->d_inode;
		struct shmem_inode_info *info = SHMEM_I(inode);

		WARN_ON(asma->pin_count || asma->purged);

		pages += info->alloced;

		if (nr_to_scan > 0) {
			nr_to_scan -= info->alloced;
			asma->purged = 1;
			ashmem_del_purgable(asma);
			vmtruncate_range(inode, 0, (loff_t) -1);
		}
	}
	mutex_unlock(&ashmem_mutex);

	return pages;
}

static struct shrinker ashmem_shrinker = {
	.shrink = ashmem_shrink,
	.seeks = DEFAULT_SEEKS * 4,
};

static int set_prot_mask(struct ashmem_area *asma, unsigned long prot)
{
	int ret = 0;

	mutex_lock(&ashmem_mutex);

	/* the user can only remove, not add, protection bits */
	if (unlikely((asma->prot_mask & prot) != prot)) {
		ret = -EINVAL;
		goto out;
	}

	/* does the application expect PROT_READ to imply PROT_EXEC? */
	if ((prot & PROT_READ) && (current->personality & READ_IMPLIES_EXEC))
		prot |= PROT_EXEC;

	asma->prot_mask = prot;

out:
	mutex_unlock(&ashmem_mutex);
	return ret;
}

static int set_name(struct ashmem_area *asma, void __user *name)
{
	int ret = 0;

	mutex_lock(&ashmem_mutex);

	/* cannot change an existing mapping's name */
	if (unlikely(asma->file)) {
		ret = -EINVAL;
		goto out;
	}

	if (unlikely(copy_from_user(asma->name, name, ASHMEM_NAME_LEN)))
		ret = -EFAULT;
	asma->name[ASHMEM_NAME_LEN-1] = '\0';

out:
	mutex_unlock(&ashmem_mutex);

	return ret;
}

static int get_name(struct ashmem_area *asma, void __user *name)
{
	int ret = 0;

	mutex_lock(&ashmem_mutex);
	if (asma->name) {
		size_t len;

		/*
		 * Copying only `len', instead of ASHMEM_NAME_LEN, bytes
		 * prevents us from revealing one user's stack to another.
		 */
		len = strlen(asma->name) + 1;
		if (unlikely(copy_to_user(name, asma->name, len)))
			ret = -EFAULT;
	} else {
		if (unlikely(copy_to_user(name, ASHMEM_NAME_DEF,
					  sizeof(ASHMEM_NAME_DEF))))
			ret = -EFAULT;
	}
	mutex_unlock(&ashmem_mutex);

	return ret;
}

/*
 * ashmem_pin - pin the given ashmem region, returning whether it was
 * previously purged (ASHMEM_WAS_PURGED) or not (ASHMEM_NOT_PURGED).
 */
static int ashmem_pin(struct ashmem_area *asma)
{
	int ret = ASHMEM_NOT_PURGED;

	if (unlikely(!asma->file))
		return -EINVAL;

	mutex_lock(&ashmem_mutex);
	if (++asma->pin_count == 1) {
		/* if we were purged, we were already removed from the list */
		if (!asma->purged)
			ashmem_del_purgable(asma);
		else {
			ret = ASHMEM_WAS_PURGED;
			asma->purged = 0;
		}
	}
	mutex_unlock(&ashmem_mutex);

	return ret;
}

static int ashmem_unpin(struct ashmem_area *asma)
{
	int ret = ASHMEM_IS_UNPINNED;

	if (unlikely(!asma->file))
		return -EINVAL;

	mutex_lock(&ashmem_mutex);
	/* only if our count is now zero do we go on the list */
	if (--asma->pin_count)
		ret = ASHMEM_IS_PINNED;
	else
		ashmem_add_purgable(asma);
	mutex_unlock(&ashmem_mutex);
	return ret;
}

static long ashmem_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct ashmem_area *asma = file->private_data;
	long ret = -ENOTTY;

	switch (cmd) {
	case ASHMEM_SET_NAME:
		ret = set_name(asma, (void __user *) arg);
		break;
	case ASHMEM_GET_NAME:
		ret = get_name(asma, (void __user *) arg);
		break;
	case ASHMEM_SET_SIZE:
		ret = -EINVAL;
		if (likely(!asma->file)) {
			ret = 0;
			asma->size = (size_t) arg;
		}
		break;
	case ASHMEM_GET_SIZE:
		ret = asma->size;
		break;
	case ASHMEM_SET_PROT_MASK:
		ret = set_prot_mask(asma, arg);
		break;
	case ASHMEM_GET_PROT_MASK:
		ret = asma->prot_mask;
		break;
	case ASHMEM_PIN:
		ret = ashmem_pin(asma);
		break;
	case ASHMEM_UNPIN:
		ret = ashmem_unpin(asma);
		break;
	case ASHMEM_GET_PIN_STATUS:
		if (asma->pin_count)
			ret = ASHMEM_IS_PINNED;
		else
			ret = ASHMEM_IS_UNPINNED;
		break;
	case ASHMEM_PURGE_ALL_CACHES:
		ret = -EPERM;
		if (capable(CAP_SYS_ADMIN)) {
			int pages;

			ret = 0;
			pages = ashmem_shrink(0, GFP_KERNEL);
			ashmem_shrink(pages, GFP_KERNEL);
		}
		break;
	}

	return ret;
}

static struct file_operations ashmem_fops = {
	.owner = THIS_MODULE,
	.open = ashmem_open,
	.release = ashmem_release,
	.mmap = ashmem_mmap,
	.unlocked_ioctl = ashmem_ioctl,
	.compat_ioctl = ashmem_ioctl,
};

static struct miscdevice ashmem_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ashmem",
	.fops = &ashmem_fops,
};

static int __init ashmem_init(void)
{
	int ret;

	ashmem_cachep = kmem_cache_create("ashmem_cache",
					  sizeof(struct ashmem_area),
					  0, 0, NULL);
	if (unlikely(!ashmem_cachep)) {
		printk(KERN_ERR "ashmem: failed to create slab cache\n");
		return -ENOMEM;
	}

	ret = misc_register(&ashmem_misc);
	if (unlikely(ret)) {
		printk(KERN_ERR "ashmem: failed to register misc device!\n");
		return ret;
	}

	register_shrinker(&ashmem_shrinker);

	printk(KERN_INFO "ashmem: initialized\n");

	return 0;
}

static void __exit ashmem_exit(void)
{
	int ret;

	unregister_shrinker(&ashmem_shrinker);

	ret = misc_deregister(&ashmem_misc);
	if (unlikely(ret))
		printk(KERN_ERR "ashmem: failed to unregister misc device!\n");

	printk(KERN_INFO "ashmem: unloaded\n");
}

module_init(ashmem_init);
module_exit(ashmem_exit);

MODULE_LICENSE("GPL");
