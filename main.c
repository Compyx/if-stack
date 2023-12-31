/** \file   main.c
 * \brief   IF stack implementation test
 *
 * \note    Uses some POSIX functions like \c strcasecmp()
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <libgen.h>

#include "ifstack.h"

/** \brief  Boolean value translation
 */
typedef struct bvalue_s {
    const char *text;   /**< text */
    bool        value;  /**< boolean value */
} bvalue_t;


/** \brief  Line read from file for processing */
static char line[256];

/** \brief  Token buffer */
static char token[sizeof line];

/** \brief  Table of words to translate to boolean values
 */
static const bvalue_t booleans[] = {
    { "0",      false },
    { "1",      true  },
    { "false",  false },
    { "true",   true  },
    { "no",     false },
    { "yes",    true  }
};


/** \brief  Print usage message on stdout
 *
 * \param[in]   argv0   content of argv[0]
 */
static void usage(char *argv0)
{
    printf("usage: %s <filename>\n", basename(argv0));
}

/** \brief  Get token from current line
 *
 * \param[in]   pos position in \c line[]
 *
 * \return  position in \a line of first whitespace character or -1 when no
 *          token was encountered
 */
static int get_token(int pos)
{
    int t;

    /* skip whitespace */
    while (pos < (int)sizeof line - 1 && line[pos] != '\0' && isspace(line[pos])) {
        pos++;
    }
    if (line[pos] == '\0') {
        /* no token */
        token[0] = '\0';
        return -1;
    }

    t = 0;
    while (pos < (int)sizeof line - 1 && line[pos] != '\0' && !isspace(line[pos])) {
        token[t++] = line[pos++];
    }
    token[t] = '\0';
    return pos;
}

/** \brief  Handle IF statement
 *
 * \param[in]   pos position in \c line[] after 'if'
 *
 * \return  \c false if argument to IF missing
 */
static bool handle_if(int pos)
{
    bool state = true;  /* anything not explicitly false will be considered true */

    if (get_token(pos) < 0) {
        printf("%s(): error: expected token after 'IF'\n", __func__);
        return false;
    }
    for (size_t i = 0; i < sizeof booleans / sizeof booleans[0]; i++) {
        if (strcasecmp(booleans[i].text, token) == 0) {
            state = booleans[i].value;
            break;
        }
    }

    printf("%-40s  ", "");
    ifstack_if(state);
    return true;
}

/** \brief  Handle ELSE statement
 *
 * \return  \c false if not in an IF branch or already in ELSE branch
 */
static bool handle_else(void)
{
    printf("%-40s  ", "");
    return ifstack_else();
}

/** \brief  Handle ENDIF statement
 *
 * \return  \c false if not in an IF or ELSE branch
 */
static bool handle_endif(void)
{
    printf("%-40s  ", "");
    return ifstack_endif();
}

/** \brief  Handle normal text
 *
 * Print text from input file if the current if-stack condition is \c true.
 */
static bool handle_text(void)
{
    printf("%-40s  ", ifstack_true() ? line : "");
    return true;
}

/** \brief  Handle line of input from file
 *
 * \return  \c false on error
 */
static bool handle_line(void)
{
    bool result;
    int  pos;

    pos = get_token(0);
    if (pos < 0) {
        /* empty line */
        result = handle_text();
    } else {
        if (strcasecmp(token, "if") == 0) {
            result = handle_if(pos);
        } else if (strcasecmp(token, "else") == 0) {
            result = handle_else();
        } else if (strcasecmp(token, "endif") == 0) {
            result = handle_endif();
        } else {
            result = handle_text();
        }
    }
    ifstack_print();
    putchar('\n');
    return result;
}

/** \brief  Parse file and process IF/THEN/ELSE statements
 *
 * Parse \a path and handle IF/THEN/ELSE using the if-stack, printing normal
 * lines when the if-stack's global condition is true.
 *
 * \return  \a true on success
 */
static bool parse(const char *path)
{
    FILE *fp;
    int   lineno;


    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "error: failed to open \"%s\": (%d) %s\n",
                path, errno, strerror(errno));
        return false;
    }

    printf("line  source                                  "
           "  output                                    stack\n");
    printf("----  ----------------------------------------"
           "  ----------------------------------------  -----\n");

    lineno = 1;
    do {
        memset(line, 0, sizeof line);
        if (fgets(line, (int)(sizeof line) - 1, fp) == NULL) {
            break;
        }
        int i = (int)strlen(line) - 1;
        while (i >= 0 && isspace((unsigned char)line[i])) {
            line[i--] = '\0';
        }

        printf("%4d  %-40s  ", lineno, line);
        if (!handle_line()) {
            fprintf(stderr,
                    "%s(): error %d: %s\n",
                    __func__, ifstack_errno, ifstack_strerror(ifstack_errno));
            goto cleanup;
        }

        lineno++;
    } while (!feof(fp));

cleanup:
    fclose(fp);
    return true;
}


/** \brief  Program driver
 *
 * Parse file in argv[1] to test the if-stack implementation.
 *
 * \param[in]   argc    argument count
 * \param[in]   argv    argument vector
 *
 * \return  \c EXIT_SUCCESS on success, \c EXIT_FAILURE on failure
 */
int main(int argc, char *argv[])
{
    int status = EXIT_SUCCESS;

    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    } else if (strcmp(argv[1], "--help") == 0) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    ifstack_init();

    printf("Parsing \"%s\"\n", argv[1]);
    if (!parse(argv[1])) {
        status = EXIT_FAILURE;
    }

    ifstack_free();
    return status;
}
