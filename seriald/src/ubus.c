#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>

#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>

#include "ubus.h"
#include "seriald.h"

/* TODO: add support for multiple ubus path */

struct ubus_context *ubus_ctx = NULL;
static const char *ubus_path;

static void seriald_ubus_add_fd(void);
static void seriald_ubus_connection_lost_cb(struct ubus_context *ctx);
static void seriald_ubus_reconnect_timer(struct uloop_timeout *timeout);

static void seriald_ubus_reconnect_timer(struct uloop_timeout *timeout)
{
	static struct uloop_timeout retry = {
		.cb = seriald_ubus_reconnect_timer,
	};
	int t = 2;

	if (ubus_reconnect(ubus_ctx, ubus_path)) {
		DPRINTF("failed to reconnect, trying again in %d seconds\n", t);
		uloop_timeout_set(&retry, t * 1000);
		return;
	}

	DPRINTF("reconnected to ubus, new id: %08x\n", ubus_ctx->local_id);
	seriald_ubus_add_fd();
}

static void seriald_ubus_connection_lost_cb(struct ubus_context *ctx)
{
	seriald_ubus_reconnect_timer(NULL);
}

static void seriald_ubus_add_fd(void)
{
	ubus_add_uloop(ubus_ctx);
	fcntl(ubus_ctx->sock.fd, F_SETFD,
			fcntl(ubus_ctx->sock.fd, F_GETFD) | FD_CLOEXEC);
}

int seriald_ubus_init(const char *path)
{
	uloop_init();
	ubus_path = path;

	ubus_ctx = ubus_connect(path);
	if (!ubus_ctx) {
		DPRINTF("cannot connect to ubus\n");
		return -EIO;
	}

	DPRINTF("connected as %08x\n", ubus_ctx->local_id);
	ubus_ctx->connection_lost = seriald_ubus_connection_lost_cb;
	seriald_ubus_add_fd();

	return 0;
}

void *seriald_ubus_loop(void *unused)
{
	uloop_run();
	return 0;
}

void seriald_ubus_done(void)
{
	ubus_free(ubus_ctx);
}
