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
 * @addtogroup values "Value types"
 *
 * @{
 */

#ifndef __WS_VALUES_OBJECT_ID_H__
#define __WS_VALUES_OBJECT_ID_H__


#include "values/value.h"

/**
 * ws_value_object_id type definition
 */
struct ws_value_object_id {
    struct ws_value val; //!< @protected Base class.
    struct ws_object* obj; //!< @protected Reference to a ws_object
};

/**
 * Initialize a ws_value_object_id object
 *
 * @memberof ws_value_object_id
 */
void
ws_value_object_id_init(
    struct ws_value_object_id* self
)
__ws_nonnull__(1)
;

/**
 * Create a new ws_value_object_id object
 *
 * @memberof ws_value_object_id
 */
struct ws_value_object_id*
ws_value_object_id_new(void);

/**
 * Get the saved reference to a ws_object object from a given ws_value_object_id
 * object
 *
 * @memberof ws_value_object_id
 *
 * @return the reference to the ws_object object or NULL on failure
 */
struct ws_object*
ws_value_object_id_get(
    struct ws_value_object_id* self
);

/**
 * Set the `obj` member of a given ws_value_object_id object to a reference to
 * a given ws_object object
 *
 * @memberof ws_value_object_id
 */
void
ws_value_object_id_set(
    struct ws_value_object_id* self,
    struct ws_object* obj
);

#endif // __WS_VALUES_OBJECT_ID_H__

/**
 * @}
 */
