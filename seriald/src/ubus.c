#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>

#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>

#include "ubus.h"
#include "seriald.h"

static struct ubus_context *ubus_ctx = NULL;
static struct blob_buf b;

/* TODO: make this work to replace the execl call */

int seriald_ubus_connect(const char *path)
{
	ubus_ctx = ubus_connect(path);
	if (!ubus_ctx) {
		DPRINTF("Failed to connect to ubus\n");
		return -EIO;
	}

	fcntl(ubus_ctx->sock.fd, F_SETFD,
			fcntl(ubus_ctx->sock.fd, F_GETFD) | FD_CLOEXEC);

	return 0;
}

void seriald_ubus_send_event(const char *data)
{
	int r;

	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "data", data);

	r = ubus_send_event(ubus_ctx, "serial", b.head);
	if (r) {
		DPRINTF("error: %s\n", ubus_strerror(r));
	}
}

void seriald_ubus_disconnect(void)
{
	ubus_free(ubus_ctx);
}
