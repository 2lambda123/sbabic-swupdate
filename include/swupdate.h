/*
 * (C) Copyright 2012
 * Stefano Babic, DENX Software Engineering, sbabic@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc. 
 */

#ifndef _SWUPDATE_H
#define _SWUPDATE_H

#include <sys/queue.h>
#include "flash.h"

#define BANNER "Swupdate v" SWU_VER "\n" \
	"Built on " AUTOCONF_TIMESTAMP

#define SWUPDATE_GENERAL_STRING_SIZE	256
#define MAX_IMAGE_FNAME	SWUPDATE_GENERAL_STRING_SIZE
#define MAX_VOLNAME	SWUPDATE_GENERAL_STRING_SIZE
#define MAX_HW_VERSIONS	10
#define MAX_LINE	80
#define UBOOT_VAR_LENGTH 16
#define MAX_REVISION_LENGTH	SWUPDATE_GENERAL_STRING_SIZE

/* These are fixed path to temporary files */
#define SCRIPTS_DIR	TMPDIR "scripts/"
#define DATASRC_DIR	TMPDIR "datasrc/"
#define DATADST_DIR	TMPDIR "datadst/"
#define UBOOT_SCRIPT	TMPDIR "uboot-script"

typedef enum {
	FLASH,
	UBI,
	FILEDEV,
	PARTITION,
	SCRIPT
} imagetype_t;

struct img_type {
	char type[SWUPDATE_GENERAL_STRING_SIZE];
	char fname[MAX_IMAGE_FNAME];
	char volname[MAX_VOLNAME];
	char device[MAX_VOLNAME];
	char path[MAX_IMAGE_FNAME];
	char extract_file[MAX_IMAGE_FNAME];
	char filesystem[MAX_IMAGE_FNAME];
	int required;
	int provided;
	int compressed;
	int is_script;
	int is_partitioner;
	long long partsize;
	int fdin;	/* Used for streaming file */
	off_t offset;	/* offset in cpio file */
	long long size;
	unsigned int checksum;
	LIST_ENTRY(img_type) next;
};

LIST_HEAD(imglist, img_type);

struct hw_type {
	char boardname[SWUPDATE_GENERAL_STRING_SIZE];
	char revision[SWUPDATE_GENERAL_STRING_SIZE];
	LIST_ENTRY(hw_type) next;
};

LIST_HEAD(hwlist, hw_type);

enum {
	SCRIPT_NONE,
	SCRIPT_PREINSTALL,
	SCRIPT_POSTINSTALL
};

struct uboot_var {
	char varname[UBOOT_VAR_LENGTH];
	char value[255];
	LIST_ENTRY(uboot_var) next;
};

LIST_HEAD(ubootvarlist, uboot_var);

struct swupdate_cfg {
	char name[SWUPDATE_GENERAL_STRING_SIZE];
	int valid;
	char version[SWUPDATE_GENERAL_STRING_SIZE];
	struct hwlist hardware;
	struct imglist images;
	struct imglist files;
	struct imglist partitions;
	struct imglist scripts;
	struct ubootvarlist uboot;
};

#define SEARCH_FILE(type, list, found, offs) do { \
	if (!found) { \
		type *p; \
		for (p = list.lh_first; p != NULL; \
			p = p->next.le_next) { \
			if (strcmp(p->fname, fdh.filename) == 0) { \
				found = 1; \
				p->offset = offs; \
				p->provided = 1; \
				p->size = fdh.size; \
				break; \
			} \
		} \
	} \
} while(0)

off_t extract_sw_description(int fd);
off_t extract_next_file(int fd, int fdout, off_t offset, int compressed);
off_t cpio_scan(int fd, struct swupdate_cfg *cfg, off_t start);

#endif
