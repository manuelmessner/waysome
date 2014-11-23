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
#include <malloc.h>
#include <pthread.h>
#include <stdbool.h>

#include "util/attributes.h"
#include "values/bool.h"

void
ws_value_bool_init(
    struct ws_value_bool* self
) {
    if (self) {
        ws_value_init(&self->value);

        self->value.type = WS_VALUE_TYPE_BOOL;
        self->value.deinit_callback = NULL;
    }
}

struct ws_value_bool*
ws_value_bool_new(void)
{
    struct ws_value_bool* v = calloc(1, sizeof(*v));
    if (!v) {
        return NULL;
    }

    ws_value_bool_init(v);
    return v;
}

bool
ws_value_bool_get(
    struct ws_value_bool* self
) {
    if (self) {
        bool b = self->b;

        return b;
    }

    return false;
}

int
ws_value_bool_set(
    struct ws_value_bool* self,
    bool b
) {
    if (self) {
        self->b = b;

        return 0;
    }

    return -EINVAL;
}
