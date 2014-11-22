/*
 * waysome - wayland based window manager
 *
 * Copyright in alphabetical order:
 *
 * Copyright (C) 2014-2015 Julian Ganz
 * Copyright (C) 2014-2015 Manuel Messner
 * Copyright (C) 2014-2015 Marcel Müller
 * Copyright (C) 2014-2015 Matthias Beyer
 * Copyright (C) 2014-2015 Nadja Sommerfeld
 *
 * This file is part of waysome.
 *
 * waysome is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * waysome is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with waysome. If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "logger/module.h"
#include "util/socket.h"

#define XDG_RUNTIME_DIR "XDG_RUNTIME_DIR"
#define UNIX_PATH "waysome.sock"
#define UNIX_PATH_MAX 108

static struct ws_logger_context log_ctx = { .prefix = "[Sockets Utils] " };

static void
socket_read_cb(
    struct ev_loop* loop,
    struct ev_io* io,
    int revents
) {
    //!< @todo I'll add this when ws_(de)serializer_new is implemented
    if (revents & EV_ERROR) {
        ws_log(&log_ctx, LOG_WARNING, "Libev error in read callback!");
        return;
    }
}

static void
socket_accept_cb(
    struct ev_loop* loop,
    struct ev_io* io,
    int revents
) {
    struct ws_socket* s = (struct ws_socket*) io;
    struct ws_socket_client* c = NULL;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);

    if (revents & EV_ERROR) {
        ws_log(&log_ctx, LOG_WARNING, "Libev error in accept callback!");
        return;
    }

    c = calloc(1, sizeof(c));
    if (!c) {
        ws_log(&log_ctx, LOG_ERR, "Could not allocate ws_socket_client");
        return;
    }

    memset(&addr, 0, len);
    c->fd = accept(s->fd, (struct sockaddr*) &addr, &len);
    if (c->fd < 0) {
        ws_log(&log_ctx, LOG_ERR, "Could not accept client: %d", errno);
        free(c);
        return;
    }

    /**
     * @todo: I'll comment the lines in when the given functions are implemented
     *
     *  struct ws_serializer* ws = ws_serializer_new();
     *  struct ws_deserializer* wd = ws_deserializer_new();
     *  if (!ws || !wd) {
     *      ws_log(&log_ctx, LOG_ERR, "Could not create serialization objects");
     *      return;
     *  }
     *  c->cm = ws_connection_manager_new(c->fd, ws, wd);
     */

    ws_log(&log_ctx, LOG_DEBUG, "Connection established!");

    ev_io_init(&c->io, socket_read_cb, c->fd, EV_READ);
    ev_io_start(loop, &c->io);
}

int
ws_socket_init(
    struct ws_socket* s
) {
    struct ev_loop* loop = ev_default_loop(EVFLAG_AUTO);
    if (!loop) {
        ws_log(&log_ctx, LOG_WARNING, "Could not initialize ev loop!");
        free(s);
        return -1;
    }

    s->fd = ws_socket_create(UNIX_PATH);
    if (s->fd < 0) {
        return -1;
    }

    ev_io_init(&s->io, socket_accept_cb, s->fd, EV_READ);
    ev_io_start(loop, &s->io);

    return 0;
}

struct ws_socket*
ws_socket_new(
    void
) {
    struct ws_socket* s = calloc(1, sizeof(*s));
    if (!s) {
        ws_log(&log_ctx, LOG_WARNING, "Could not allocate ws_socket object!");
        return NULL;
    }

    if (ws_socket_init(s) < 0) {
        ws_log(&log_ctx, LOG_WARNING, "Could not initialize ws_socket object!");
        return NULL;
    }

    return s;
}

int
ws_socket_create(
    char const* name
) {
    char* xdg_env = getenv(XDG_RUNTIME_DIR);

    if (!xdg_env) {
        ws_log(&log_ctx, LOG_WARNING, "XDG_RUNTIME_DIR is not set!");
        xdg_env = "/tmp";
    }

    /* Sock(et)s!
     *  mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm   mmm
     *  | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |
     *  | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |   | |
     *  \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \   \ \
     *   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)   \_)
     */

    int sock = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sock < 0) {
        ws_log(&log_ctx, LOG_ERR, "Could not create socket");
        return sock;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    int length = snprintf(addr.sun_path, UNIX_PATH_MAX, "%s/%s", xdg_env, name);

    if (!length || length >= UNIX_PATH_MAX) {
        ws_log(&log_ctx, LOG_ERR, "Could not create socket at %s/%s",
                xdg_env, name);
        return -1;
    }

    int res = bind(sock, (struct sockaddr*) &addr, sizeof(addr));

    if (res < 0) {
        ws_log(&log_ctx, LOG_ERR, "Could not bind.");
        return -1;
    }

    return sock;
}

