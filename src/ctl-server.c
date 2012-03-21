/*
 * Copyright (c) 2008 Vincent Bernat <bernat@luffy.cx>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "lldpd.h"

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

static void
ctl_callback(struct lldpd *cfg, struct lldpd_callback *callback)
{
	enum hmsg_type type;
	void          *buffer = NULL;
	int            n;

	if ((n = ctl_msg_recv(callback->fd, &type, &buffer)) == -1 ||
	    client_handle_client(cfg, callback, type, buffer, n) == -1) {
		close(callback->fd);
		lldpd_callback_del(cfg, callback->fd, ctl_callback);
	}
	free(buffer);
}

void
ctl_accept(struct lldpd *cfg, struct lldpd_callback *callback)
{
	int s;
	if ((s = accept(callback->fd, NULL, NULL)) == -1) {
		LLOG_WARN("unable to accept connection from socket");
		return;
	}
	if (lldpd_callback_add(cfg, s, ctl_callback, NULL) != 0) {
		LLOG_WARN("unable to add callback for new client");
		close(s);
	}
	return;
}
