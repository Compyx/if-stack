/** \file   ifstack.c
 * \brief   IF stack implementation
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "ifstack.h"


/** \brief  Doubly linked list node making up the IF stack
 */
typedef struct ifstack_s {
    bool              state;    /**< branch IF state */
    bool              in_else;  /**< currently in local ELSE branch */
    struct ifstack_s *next;     /**< next node (up in stack) */
    struct ifstack_s *prev;     /**< previous node (down in stack) */
} ifstack_t;


/** \brief  IF stack reference
 *
 * This references the top of the stack.
 */
static ifstack_t *stack;

/** \brief  IF stack bottom reference
 *
 * Used for printing the stack from bottom to top in ifstack_print().
 */
static ifstack_t *stack_bottom;

/** \brief  Global "truth" state
 */
static bool       current_state;


static void *lib_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr,
                "%s(): failed to allocate %zu bytes, exiting.\n",
                __func__, size);
        exit(1);
    }
    return ptr;
}

static void lib_free(void *ptr)
{
    free(ptr);
}

static void ifstack_push(bool state)
{
    ifstack_t *node = lib_malloc(sizeof *node);

    node->state   = state;
    node->in_else = false;
    node->next    = NULL;
    node->prev    = stack;
    if (stack != NULL) {
        stack->next = node;
    } else {
        stack_bottom = node;
    }
    stack = node;
}

static void ifstack_pull(void)
{
    if (stack == NULL) {
        fprintf(stderr, "%s(): error: stack empty!\n", __func__);
        exit(1);
    } else {
        ifstack_t *prev = stack->prev;

        lib_free(stack);
        stack = prev;
        if (stack == NULL) {
            /* no previous condition of stack, set current state to true */
            current_state = true;
            /* clear stack bottom pointer */
            stack_bottom = NULL;
        } else {
            stack->next = NULL;
            if (current_state) {
                if (stack->in_else) {
                    current_state = !(stack->state);
                } else {
                    current_state = stack->state;
                }
            }
        }
    }
}



static void ifstack_free(void)
{
    ifstack_t *node = stack;

    while (node != NULL) {
        ifstack_t *prev = node->prev;
        lib_free(node);
        node = prev;
    }
}



void ifstack_init(void)
{
    stack = NULL;
    ifstack_reset();
}


void ifstack_reset(void)
{
    ifstack_free();
    stack         = NULL;
    stack_bottom  = NULL;
    current_state = true;
}


void ifstack_shutdown(void)
{
    ifstack_free();
}


void ifstack_print(void)
{
    ifstack_t *node = stack_bottom;

    putchar('[');
    while (node != NULL) {
        putchar(node->state ? '1' : '0');
        if (node->next != NULL) {
            printf(", ");
        }
        node = node->next;
    }
    putchar(']');
}


bool ifstack_true(void)
{
    return current_state;
}


void ifstack_if(bool state)
{
    ifstack_push(state);
    if (stack->prev == NULL || (stack->prev != NULL && stack->prev->state)) {
        current_state = state;
    }
}


bool ifstack_else(void)
{
    if (stack == NULL) {
        fprintf(stderr, "error: ELSE without IF.\n");
        return false;
    }
    if (stack->in_else) {
        printf("error: already in ELSE branch.\n");
        return false;
    }

    stack->in_else = true;
    stack->state   = !stack->state;
    /* only invert global state if it's true */
    if (current_state) {
        current_state = !current_state;
    } else if (stack->prev == NULL) {
        current_state = !current_state;
    } else if (stack->prev != NULL && stack->prev->state) {
        current_state = !current_state;
    }

    return true;
}


bool ifstack_endif(void)
{
    if (stack == NULL) {
        printf("error: ENDIF without preceeding IF [ELSE].\n");
        return false;
    }

    /* pull condition of the stack */
    ifstack_pull();
    return true;
}
