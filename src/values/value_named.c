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
#include <pthread.h>
#include <string.h>

#include "objects/object.h"
#include "objects/string.h"

#include "values/value.h"
#include "values/value_named.h"

static void
deinit_namedval(
    struct ws_value* self
) {
    if (!self || self->type != WS_VALUE_TYPE_NAMED) {
        return;
    }

    struct ws_value_named_value* nv;
    nv = (struct ws_value_named_value*) self;

    ws_object_unref((struct ws_object*) nv->name);
}

void
ws_value_named_value_init(
    struct ws_value_named_value* self
) {
    if (!self || self->value.type != WS_VALUE_TYPE_NAMED) {
        return;
    }

    self->name = NULL;
    self->v = NULL;

    self->value.deinit_callback = deinit_namedval;
}

struct ws_value_named_value*
ws_value_named_value_new(void)
{

    struct ws_value_named_value* v = calloc(1, sizeof(*v));
    if (!v) {
        return NULL;
    }

    ws_value_named_value_init(v);
    return v;
}

int
ws_value_named_set_name(
    struct ws_value_named_value* self,
    struct ws_string* str
) {
    if (self) {
        ws_object_getref((struct ws_object*) str);
        struct ws_string* old = self->name;
        self->name = str;
        ws_object_unref((struct ws_object*) old);
    }
    return -EINVAL;
}

int
ws_value_named_set_value(
    struct ws_value_named_value* self,
    struct ws_value* val
) {
    if (self) {
        self->v = val;
    }
    return -EINVAL;
}

struct ws_string const*
ws_value_named_get_name(
    struct ws_value_named_value* self
) {
    if (self) {
        ws_object_getref((struct ws_object*) self->name);
        return self->name;
    }

    return NULL;
}

struct ws_value*
ws_value_named_get_value(
    struct ws_value_named_value* self
) {
    if (self) {
        return self->v;
    }

    return NULL;
}

signed int
ws_value_named_cmp(
    struct ws_value_named_value* self,
    struct ws_value_named_value* other
) {

    /*
     * IF
     *  self does not exist, but other
     * OR
     *  self is not a WS_VALUE_TYPE_NAMED, but other
     *
     * return 1
     */
    if ((!self && other) || ((self->value.type != WS_VALUE_TYPE_NAMED) &&
            (other->value.type == WS_VALUE_TYPE_NAMED))) {
        return 1;
    }
    /*
     * else, IF
     *  self does exist but not other
     * OR
     * self is a WS_VALUE_TYPE_NAMED, but not other
     *
     * return -1
     */
    else if ((self && !other) || ((self->value.type == WS_VALUE_TYPE_NAMED) &&
            (other->value.type != WS_VALUE_TYPE_NAMED))) {
        return -1;
    }

    /*
     * If they both exist and are both WS_VALUE_TYPE_NAMED types
     */

    int cmp = ws_string_cmp(self->name, other->name);

    return cmp;
}
