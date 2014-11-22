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

/**
 * @addtogroup utils "(internal) utilities"
 *
 * @{
 */

/**
 * @addtogroup utils_socket "(internal) socket utility"
 *
 * This utility provides a UNIX-socket to which a external plugin can connect.
 * The plugin can send any commands and get the responses after executing.
 *
 * @{
 */

#ifndef __WS_UTIL_SOCKET_H__
#define __WS_UTIL_SOCKET_H__

#include <ev.h>

#include "connection/processor.h"

/**
 * Impementation of the socket types.
 */

struct ws_socket {
    struct ev_io io;    //!< ev_io object for libev
    int fd;             //!< file descriptor of the created socket
};

struct ws_socket_client {
    struct ev_io io;                    //!< ev_io object for libev
    struct ws_connection_manager* cm;   //!< reference to a con_man object
    int fd;                             //!< fd of the accepted client
};

/**
 * Initialize a given ws_socket object
 *
 * @return 0 in case of success, else a negative value
 */
int
ws_socket_init(
    struct ws_socket*   //!< the uninitialized ws_socket object
);

/**
 *  Create a socket with a given name this socket will be placed in the
 *  XDG_RUNTIME_DIR path
 */
int
ws_socket_create(
    char const* name //!< The name of the socket that will be created
);

#endif // __WS_UTIL_SOCKET_H__

/**
 * @}
 */

/**
 * @}
 */

