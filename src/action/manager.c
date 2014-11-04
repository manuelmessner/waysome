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
#include <stddef.h>

#include "action/manager.h"
#include "action/processor.h"
#include "action/processor_stack.h"
#include "objects/message/error_reply.h"
#include "objects/message/transaction.h"
#include "objects/message/value_reply.h"


/**
 * Internal context of the transaction manager
 */
struct {
    struct ws_set transactions; //!< @public transactions registered
    struct ws_set registrations; //!< @public registrations of transactions
} actman_ctx;


/*
 *
 * Forward declarations
 *
 */

/**
 * Run a transaction
 *
 * This function will run a transaction and generate a reply by returning either
 * an error reply message or a value reply message transporting the topmost
 * element from the processor stack, if the transaction left anything, tat is.
 * If the transaction didn't leave a value, a nil value will be embedded in the
 * reply.
 *
 * @return reply generated by running the transaction
 */
static struct ws_reply*
run_transaction(
    struct ws_transaction* transaction, // transaction to run
    struct ws_value* context //!< context to push on the stack
);


/*
 *
 * Interface implementation
 *
 */

struct ws_reply*
ws_action_manager_process(
    struct ws_message* message
) {
    // check whether the message is a transaction
    if (message->obj.id == &WS_OBJECT_TYPE_ID_TRANSACTION) {
        struct ws_transaction* transaction;
        transaction = (struct ws_transaction*) message;

        // get the flags
        enum ws_transaction_flags flags = ws_transaction_flags(transaction);

        //!< @todo store the transaction if requested

        if (flags & WS_TRANSACTION_FLAGS_EXEC) {
            // execute the transaction
            return run_transaction(transaction, NULL);
        }

        return NULL;
    }

    //!< @todo handle event messages

    return NULL;
}


/*
 *
 * Internal implementation
 *
 */

static struct ws_reply*
run_transaction(
    struct ws_transaction* transaction,
    struct ws_value* context
) {
    struct ws_reply* retval = NULL;
    int res;

    // prepare the stack
    struct ws_processor_stack stack;
    res = ws_processor_stack_init(&stack);
    if (res < 0) {
        retval = (struct ws_reply*)
                 ws_error_reply_new(transaction, -res, "Could not init stack",
                                    NULL);
        goto cleanup_stack;
    }

    // push environment on the stack
    res = ws_processor_stack_push(&stack, 2);
    if (res < 0) {
        retval = (struct ws_reply*)
                 ws_error_reply_new(transaction, -res, "Could not init stack",
                                    NULL);
        goto cleanup_stack;
    }

    {
        union ws_value_union* bottom = ws_processor_stack_bottom(&stack);

        // initialize the global context
        ws_value_union_reinit(bottom, WS_VALUE_TYPE_NIL); //!< @todo real thing

        // initialize the event context
        ++bottom;
        if (context) {
            ws_value_union_init_from_val(bottom, context);
        } else {
            ws_value_union_reinit(bottom, WS_VALUE_TYPE_NIL);
        }
    }

    // we start a new frame, but we will never restore the default frame
    (void) ws_processor_stack_start_frame(&stack);

    // get the commands from the transaction
    struct ws_transaction_command_list* commands;
    commands = ws_transaction_commands(transaction);
    if (!commands) {
        retval = (struct ws_reply*)
                 ws_error_reply_new(transaction, EINVAL,
                                    "Command list malformed", NULL);
        goto cleanup_stack;
    }

    // prepare the processor
    struct ws_processor proc;
    res = ws_processor_init(&proc, &stack, commands);
    if (res < 0) {
        retval = (struct ws_reply*)
                 ws_error_reply_new(transaction, -res,
                                    "Could not init processor", NULL);
        goto cleanup_stack;
    }

    // run processor
    res = ws_processor_exec(&proc);
    if (res < 0) {
        //!< @todo more elaborate error description
        retval = (struct ws_reply*)
                 ws_error_reply_new(transaction, -res,
                                    "Could not exec transaction", NULL);
        goto cleanup_processor;
    }

    // create the return message from the transaction and the value
    struct ws_value* value = ws_processor_stack_value_at(&stack, -1, NULL);
    retval = (struct ws_reply*) ws_value_reply_new(transaction, value);

    // cleanup

cleanup_processor:
    ws_processor_deinit(&proc);

cleanup_stack:
    ws_processor_stack_deinit(&stack);
    return retval;
}

