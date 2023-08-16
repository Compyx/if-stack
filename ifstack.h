/** \file   ifstack.h
 * \brief   IF stack implementation - header
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

#ifndef IFSTACK_H
#define IFSTACK_H

#include <stdbool.h>

enum {
    IFSTACK_ERR_OK,
    IFSTACK_ERR_ELSE_WITHOUT_IF,
    IFSTACK_ERR_ENDIF_WITHOUT_IF
};

extern int ifstack_errno;

void ifstack_init(void);
void ifstack_reset(void);
void ifstack_free(void);
void ifstack_print(void);

void ifstack_if(bool state);
bool ifstack_else(void);
bool ifstack_endif(void);
bool ifstack_true(void);

const char *ifstack_strerror(int errnum);

#endif
