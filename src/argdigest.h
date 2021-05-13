/*
    Author:     ripmeep
    Instagram:  @rip.meep
    GitHub:     https://github.com/ripmeep
*/

#ifndef ARGDIGEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define ARGDIGEST_DEFAULT_HELP_PREFIX   "Usage and parameter help for %s:\n    -h/--help: Shows this usage information\n\n"
#define ARGDIGEST_ERROR_DUPLICATE_PARAM "Duplicate parameter detected '%s'\nUse -h or --help for more information\n"
#define ARGDIGEST_ERROR_UNKNOWN_PARAM   "Unknown parameter detected '%s'\nUse -h or --help for a full list of usable parameters and more information\n"
#define ARGDIGEST_ERROR_REQUIRES_VALUE  "The parameter '%s' requires a value\nUse -h or --help for more information\n"
#define ARGDIGEST_ERROR_REQUIRED_PARAM  "The %s parameter is required\nUse -h or --help for more information\n"
#define ARGDIGEST_ERROR_VERBOSE         1 /* 1 = on    0 = off, simple right? */


char *adlcltm()
{
	time_t t;
	time(&t);

	struct tm *ti = localtime(&t);

	char lt[9];

	snprintf(lt, 9, "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);

	return strdup(lt);
}

#define ARGDIGEST_ERRLOG(fmt, args...) fprintf(stderr, "\r[%s] (#%d->%s) ", adlcltm(), __LINE__, __FILE__); fprintf(stderr, ""fmt, ##args); fflush(stderr);

typedef enum { ARG_STR, ARG_INT, ARG_SWITCH } ARGDIGEST_TYPE;

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
int ArgDigestAddParam(ArgDigest *digest, char param_name[], char *param, char *full_param, char help[], ARGDIGEST_TYPE arg_type, bool required);
int ArgDigestInvokeDigestion(ArgDigest *digest);

#define ARGDIGEST_H
#endif