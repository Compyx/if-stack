/** \file   ifstack.h
 * \brief   IF stack implementation - header
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

#ifndef IFSTACK_H
#define IFSTACK_H

#include <stdbool.h>

void ifstack_init(void);
void ifstack_reset(void);
void ifstack_shutdown(void);
void ifstack_print(void);

void ifstack_if(bool state);
bool ifstack_else(void);
bool ifstack_endif(void);
bool ifstack_true(void);

#endif