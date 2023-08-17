/** \file   ifstack.h
 * \brief   IF stack implementation - header
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/* Copyright (C) 2023  Bas Wassink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
