/*
 * This file is a part of plotnetcfg, a tool to visualize network config.
 * Copyright (C) 2014 Red Hat, Inc. -- Jiri Benc <jbenc@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _HANDLER_H
#define _HANDLER_H
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/rtnetlink.h>
#include "if.h"
#include "netns.h"

/* Only one handler for each driver allowed.
 * Generic handlers called for every interface are supported and are created
 * by setting driver to NULL. Generic handlers are not allowed to use
 * handler_private field in struct if_entry.
 *
 * If you want to use handler_private, private_size bytes will be allocated
 * before any callback is called.
 *
 * Callbacks are called in this order:
 *   1. netlink - while reading interface data from netlink
 *   2. scan - while scanning interfaces, sysfs is mounted
 *   3. post - all interfaces are scanned, use this for inter-interface
 *      scanning
 *   4. cleanup - destroy private structure. handler_private itself will be
 *      freed automatically.
 */
struct handler {
	struct handler *next;
	const char *driver;
	size_t private_size;
	int (*netlink)(struct if_entry *entry, struct rtattr **linkinfo);
	int (*scan)(struct if_entry *entry);
	int (*post)(struct if_entry *entry, struct netns_entry *root);
	void (*cleanup)(struct if_entry *entry);
};

void handler_register(struct handler *h);
int handler_init(struct if_entry *entry);
int handler_netlink(struct if_entry *entry, struct rtattr **linkinfo);
int handler_scan(struct if_entry *entry);
int handler_post(struct netns_entry *root);
void handler_cleanup(struct if_entry *entry);

struct global_handler {
	struct global_handler *next;
	void (*init)(void);
	int (*post)(struct netns_entry *root);
	void (*cleanup)(struct netns_entry *root);
};

void global_handler_register(struct global_handler *h);
void global_handler_init(void);
int global_handler_post(struct netns_entry *root);
void global_handler_cleanup(struct netns_entry *root);

#endif
