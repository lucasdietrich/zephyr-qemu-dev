/*
 * Copyright (c) 2022 Lucas Dietrich
 * Copyright (c) 2022 Lukasz Majewski, DENX Software Engineering GmbH
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Sample which uses the filesystem API with littlefs */

#include <stdio.h>

#include <zephyr.h>
#include <device.h>

#include <fs/fs.h>
#include <fs/littlefs.h>
#include <ff.h>

#include <storage/flash_map.h>
#include <storage/disk_access.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(app_fs, LOG_LEVEL_DBG);

static int lsdir(const char *path)
{
	int res;
	struct fs_dir_t dirp;
	static struct fs_dirent entry;

	fs_dir_t_init(&dirp);

	/* Verify fs_opendir() */
	res = fs_opendir(&dirp, path);
	if (res) {
		LOG_ERR("Error opening dir %s [%d]", path, res);
		return res;
	}

	LOG_INF("Listing dir %s ...", path);
	for (;;) {
		/* Verify fs_readdir() */
		res = fs_readdir(&dirp, &entry);

		/* entry.name[0] == 0 means end-of-dir */
		if (res || entry.name[0] == 0) {
			if (res < 0) {
				LOG_ERR("Error reading dir [%d]", res);
			}
			break;
		}

		if (entry.type == FS_DIR_ENTRY_DIR) {
			LOG_INF("[DIR ] %s", entry.name);
		} else {
			LOG_INF("[FILE] %s (size = %zu)",
				   entry.name, entry.size);
		}
	}

	/* Verify fs_closedir() */
	fs_closedir(&dirp);

	return res;
}


static int littlefs_flash_erase(unsigned int id)
{
	const struct flash_area *pfa;
	int rc;

	rc = flash_area_open(id, &pfa);
	if (rc < 0) {
		LOG_ERR("FAIL: unable to find flash area %u: %d",
			id, rc);
		return rc;
	}

	LOG_INF("Area %u at 0x%x on %s for %u bytes",
		   id, (unsigned int)pfa->fa_off, pfa->fa_dev_name,
		   (unsigned int)pfa->fa_size);

	/* Optional wipe flash contents */
	if (IS_ENABLED(CONFIG_APP_WIPE_STORAGE)) {
		rc = flash_area_erase(pfa, 0, pfa->fa_size);
		LOG_ERR("Erasing flash area ... %d", rc);
	}

	flash_area_close(pfa);
	return rc;
}

/* LittleFS mount point */
#define LFS_PARTITION_NODE DT_NODELABEL(lfs1)
#define FATFS_PARTITION_NODE DT_NODELABEL(fatfs1)

#if DT_NODE_EXISTS(LFS_PARTITION_NODE)
FS_FSTAB_DECLARE_ENTRY(LFS_PARTITION_NODE);
#else /* LFS_PARTITION_NODE */
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
static struct fs_mount_t lfs_storage_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &storage,
	.storage_dev = (void *)FLASH_AREA_ID(storage),
	.mnt_point = "/lfs",
};
#endif /* LFS_PARTITION_NODE */

	struct fs_mount_t *mp =
#if DT_NODE_EXISTS(LFS_PARTITION_NODE)
		&FS_FSTAB_ENTRY(LFS_PARTITION_NODE)
#else
		&lfs_storage_mnt
#endif
		;

/* FATFS mount point */
static FATFS fat_fs;

static struct fs_mount_t mp2 = {
	.type = FS_FATFS,
	.fs_data = &fat_fs,
};

static const char *disk_mount_pt = "/RAM:";

int fs_stats(void)
{
	int rc;
	struct fs_statvfs sbuf;

	rc = fs_statvfs(mp->mnt_point, &sbuf);
	if (rc < 0) {
		LOG_INF("FAIL: statvfs: %d", rc);
		goto exit;
	}

	LOG_INF("%s: bsize = %lu ; frsize = %lu ;"
		   " blocks = %lu ; bfree = %lu",
		   mp->mnt_point,
		   sbuf.f_bsize, sbuf.f_frsize,
		   sbuf.f_blocks, sbuf.f_bfree);

	rc = lsdir(mp->mnt_point);
	if (rc < 0) {
		LOG_INF("FAIL: lsdir %s: %d", mp->mnt_point, rc);
		goto exit;
	}

	rc = 0;

exit:
	return rc;
}

static int fatfs_mount(void)
{
	int rc;

	/* raw disk i/o */
	do {
		static const char *disk_pdrv = "RAM";
		uint64_t memory_size_mb;
		uint32_t block_count;
		uint32_t block_size;

		if (disk_access_init(disk_pdrv) != 0) {
			LOG_ERR("Storage init ERROR!");
			break;
		}

		if (disk_access_ioctl(disk_pdrv,
				DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) {
			LOG_ERR("Unable to get sector count");
			break;
		}
		LOG_INF("Block count %u", block_count);

		if (disk_access_ioctl(disk_pdrv,
				DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
			LOG_ERR("Unable to get sector size");
			break;
		}
		printk("Sector size %u\n", block_size);

		memory_size_mb = (uint64_t)block_count * block_size;
		printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));
	} while (0);

	mp2.mnt_point = disk_mount_pt;

	rc = fs_mount(&mp2);

	if (rc == FR_OK) {
		printk("Disk mounted.\n");
		lsdir(disk_mount_pt);
	} else {
		printk("Error mounting disk.\n");
	}

	return rc;
}

static int littlefs_mount(void)
{
	int rc;

	rc = littlefs_flash_erase((uintptr_t)mp->storage_dev);
	if (rc < 0) {
		return rc;
	}

	/* Do not mount if auto-mount has been enabled */
#if !DT_NODE_EXISTS(LFS_PARTITION_NODE) ||						\
	!(FSTAB_ENTRY_DT_MOUNT_FLAGS(LFS_PARTITION_NODE) & FS_MOUNT_FLAG_AUTOMOUNT)
	rc = fs_mount(mp);
	if (rc < 0) {
		LOG_INF("FAIL: mount id %" PRIuPTR " at %s: %d",
		       (uintptr_t)mp->storage_dev, mp->mnt_point, rc);
		return rc;
	}
	LOG_INF("%s mount: %d", mp->mnt_point, rc);
#else
	LOG_INF("%s automounted", mp->mnt_point);
#endif

	return rc;
}

int fs_init(void)
{
	int rc;

	rc = fatfs_mount();
	if (rc < 0) {
		return rc;
	}

	rc = littlefs_mount();

	return rc;
}

int fs_file_add(const char *fname, const char *data, size_t size)
{
	int rc, written = 0;
	struct fs_file_t file;

	fs_file_t_init(&file);
	rc = fs_open(&file, fname, FS_O_CREATE | FS_O_WRITE);
	if (rc < 0) {
		LOG_ERR("FAIL: open %s: %d", log_strdup(fname), rc);
		return rc;
	}

	rc = fs_seek(&file, 0, FS_SEEK_SET);
	if (rc < 0) {
		LOG_ERR("FAIL: seek %s: %d", log_strdup(fname), rc);
		goto out;
	}

	rc = fs_write(&file, data, size);
	if (rc < 0) {
		LOG_ERR("FAIL: write %s: %d", log_strdup(fname), rc);
		goto out;
	}

	written = rc;

out:
	rc = fs_close(&file);
	if (rc < 0) {
		LOG_ERR("FAIL: close %s: %d", log_strdup(fname), rc);
		return rc;
	}

	return written;
}