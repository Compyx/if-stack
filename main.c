/** \file   main.c
 * \brief   IF stack implementation test
 *
 * \note    Uses some POSIX functions like \c strcasecmp()
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


typedef struct bvalue_s {
    const char *text;
    bool        value;
} bvalue_t;


/** \brief  Line read from file for processing */
static char line[256];

/** \brief  Token buffer */
static char token[sizeof line];

static const bvalue_t booleans[] = {
    { "0",      false },
    { "1",      true  },
    { "false",  false },
    { "true",   true  }
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

    printf("line  source                                    output                                    stack\n");
    printf("----  ----------------------------------------  ----------------------------------------  -------------------\n");

    lineno = 1;
    do {
        int i;

        memset(line, 0, sizeof line);
        fgets(line, (int)(sizeof line) - 1, fp);
        i = (int)strlen(line) - 1;
        while (i >= 0 && isspace((unsigned char)line[i])) {
            line[i--] = '\0';
        }

        printf("%4d  %-40s  ", lineno, line);
        if (!handle_line()) {
            fprintf(stderr, "%s(): error: something went wrong.\n", __func__);
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

    ifstack_shutdown();
    return status;
}
