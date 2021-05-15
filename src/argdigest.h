/*
    Author:     ripmeep
    Instagram:  @rip.meep
    GitHub:     https://github.com/ripmeep
*/

#ifndef ARGDIGEST_H
#define ARGDIGEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define ARGDIGEST_DEFAULT_HELP_PREFIX   "Usage and parameter help for %s:\n    %s OR %s: Shows this usage information\n\n"

#define ARGDIGEST_ERROR_DUPLICATE_PARAM "Duplicate parameter detected '%s'\nUse -h or --help for more information\n"
#define ARGDIGEST_ERROR_UNKNOWN_PARAM   "Unknown parameter detected '%s'\nUse %s or %s for a full list of usable parameters and more information\n"
#define ARGDIGEST_ERROR_REQUIRES_VALUE  "The parameter '%s' requires a value\nUse %s or %s for more information\n"
#define ARGDIGEST_ERROR_REQUIRED_PARAM  "The %s parameter is required\nUse %s or %s for more information\n"

extern bool ARGDIGEST_ERROR_VERBOSE;
extern bool ARGDIGEST_EXIT_ON_FAILURE;      /* true = yes   false = no */
extern char *ARGDIGEST_HELP_SHORT_SWITCH;
extern char *ARGDIGEST_HELP_LONG_SWITCH;

char *adlcltm();

#define ARGDIGEST_ERRLOG(fmt, args...) fprintf(stderr, ""); fprintf(stderr, "[%s] (#%d->%s) "fmt, adlcltm(), __LINE__, __FILE__, ##args); fflush(stderr);

typedef enum { ARG_STR, ARG_INT, ARG_SWITCH } ARGDIGEST_TYPE;
typedef enum { ARG_NOT_REQUIRED, ARG_REQUIRED } ARGDIGEST_MANDATORY;
typedef enum { ARGDIGEST_SET_EXIT_ON_FAILURE, ARGDIGEST_SET_ERROR_VERBOSITY, ARGDIGEST_SET_HELP_SHORT_SWITCH, ARGDIGEST_SET_HELP_LONG_SWITCH } ARGDIGEST_OPTION;

typedef struct argresult_t
{
    char *param;
    char *full_param;
    char *param_name; /* for finding and extraction */

    bool chosen;
    bool required;

    ARGDIGEST_TYPE type;

    void *value;
} ArgResult;

typedef struct argdigest_t
{
    int argc;
    char **argv; /* hey this looks familiar.... */

    char **param_list; /* 2D list of parameter argument switches */
    char **param_names; /* 2D list of parameter names */

    int n_params_list;
    int n_params; /* how many parameters even are there? */

    char *help; /* help message (-h and --help are reserved btw)*/
    size_t help_len;

    bool custom_help;

    char *description;

    struct argresult_t **arglist;
} ArgDigest;

int ArgDigestInit(ArgDigest *digest, int argc, char **argv, char *help, char *description);
int ArgDigestAddParam(ArgDigest *digest, char param_name[], char *param, char *full_param, char help[], ARGDIGEST_TYPE arg_type, ARGDIGEST_MANDATORY required);
int ArgDigestGlobalSetOpt(ARGDIGEST_OPTION mode, void *value);
int ArgDigestInvokeDigestion(ArgDigest *digest);
ArgResult *ArgDigestGetValue(ArgDigest *digest, char param_name[]);
void ArgDigestFree(ArgDigest *digest);

#endif
