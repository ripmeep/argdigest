/*
    Author:     ripmeep
    Instagram:  @rip.meep
    GitHub:     https://github.com/ripmeep
*/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "argdigest.h"


bool ARGDIGEST_ERROR_VERBOSE = false;
bool ARGDIGEST_EXIT_ON_FAILURE = false;

char *ARGDIGEST_HELP_SHORT_SWITCH = "-h";
char *ARGDIGEST_HELP_LONG_SWITCH = "--help";

int ArgDigestInit(ArgDigest *digest, int argc, char **argv, char *help, char *description)
{
    if (argc < 1) /* who's using this with no args???? */
    {
        if (ARGDIGEST_EXIT_ON_FAILURE)
            exit(-1);

        return -1;
    }

    digest->argc = argc;
    digest->argv = argv; /* easy enough? I hope? */
    digest->custom_help = false;

    if (help != NULL)
    {
        digest->help = help;
        digest->custom_help = true;
    } else
    {
        size_t base_help_len = strlen(ARGDIGEST_DEFAULT_HELP_PREFIX) + strlen(description) + strlen(ARGDIGEST_HELP_SHORT_SWITCH) + strlen(ARGDIGEST_HELP_LONG_SWITCH) +  strlen(argv[0]);

        digest->help = (char*)malloc(base_help_len + 1);
        
        if (digest->help == NULL)
        {
            if (ARGDIGEST_ERROR_VERBOSE)
                ARGDIGEST_ERRLOG("Failed initializing Argument Digester -> help [malloc()]: %s\n", strerror(errno));

            return -2;
        }
    
        sprintf(digest->help, "%s\n\n"ARGDIGEST_DEFAULT_HELP_PREFIX, description, argv[0], ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
    }



    digest->description = description;
    digest->help_len = strlen(digest->help);
    digest->n_params = 0;
    digest->n_params_list = 0;
    digest->param_list = (char**)malloc(digest->n_params_list + 1); /* not including -h/--help */

    if (digest->param_list == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Failed initializing Argument Digester -> parameter list [malloc()]: %s\n", strerror(errno));

        return -3;
    }

    digest->param_names = (char**)malloc(digest->n_params + 1);

    if (digest->param_names == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Failed initializing Argument Digester -> parameter names [malloc()]: %s\n", strerror(errno));

        return -4;
    }

    digest->arglist = (struct argresult_t**)malloc(digest->n_params + 1);

    if (digest->arglist == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Failed initializing Argument Digester -> argument list [malloc()]: %s\n", strerror(errno));

        return -4;
    }


    return 0;
}

int ArgDigestAddParam(ArgDigest *digest, char param_name[], char *param, char *full_param, char help[], ARGDIGEST_TYPE arg_type, ARGDIGEST_MANDATORY required)
{
    if (param == NULL && full_param == NULL) 
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("New parameter must include a short or long parameter switch, niether was passed\n");

        return -1;
    }

    if (strlen(param_name) < 1)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Parameter name cannot be empty\n");

        return -2;
    }

    for (int i = 0; i < digest->n_params_list; i++)
    {
        if (param){
            if (!strcmp(digest->param_list[i], param))
            {
                if (ARGDIGEST_ERROR_VERBOSE)
                    ARGDIGEST_ERRLOG("Duplicate parameter detected -> %s\n", param);

                return -3;
            }
        }

        if (full_param)
        {
            if (!strcmp(digest->param_list[i], full_param))
            {
                if (ARGDIGEST_ERROR_VERBOSE)
                    ARGDIGEST_ERRLOG("Duplicate parameter detected -> %s\n", param);

                return -3;
            }
        }
    }

    int new_entries = 1;
    bool both_params = 0;
    bool short_param = 0;
    bool long_param = 0;

    if (param != NULL && full_param != NULL)
    {
        new_entries = 2;
        both_params = true;
        short_param = true;
        long_param = true;
    }

    digest->param_list = (char**)realloc(digest->param_list, (digest->n_params_list + new_entries + 1) * sizeof(*digest->param_list));

    if (digest->param_list == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Failed to extend Argument Digester parameter list [realloc()]: %s\n", strerror(errno));

        return -4;
    }

    if (both_params)
    {
        digest->param_list[digest->n_params_list] = strdup(param);
        digest->param_list[digest->n_params_list + 1] = strdup(full_param);
    } else
    {
        char *new;

        if (param)
        {
            new = param;
            short_param = true;
        } else
        {
            new = full_param;
            long_param = true;
        }

        digest->param_list[digest->n_params_list] = strdup(new);
    }

    digest->n_params_list += new_entries;

    if (!digest->custom_help)
    {
        size_t help_len = strlen(help);
        size_t new_help_len = digest->help_len + strlen(param_name) + help_len;
        
        if (short_param)
        {
            new_help_len += strlen(param);
        }

        if (long_param)
        {
            new_help_len += strlen(full_param);
        }

        digest->help = (char*)realloc(digest->help, new_help_len + 128);

        if (digest->help == NULL)
        {
            if (ARGDIGEST_ERROR_VERBOSE)
                ARGDIGEST_ERRLOG("Failed to extend the Argument Digester help [realloc()]: %s\n", strerror(errno));

            return -5;
        }

        char new_entry[new_help_len + 128];

        int new_bytes = sprintf(new_entry, "    %10s [%s%s%s] | %s %s %s: %s %s\n", param_name,  ((arg_type==ARG_STR)?"STRING":""), ((arg_type==ARG_INT)?"NUMBER":""), ((arg_type==ARG_SWITCH)?" BOOL ":""), ((param==NULL)?"":param), ((param==NULL||full_param==NULL)?"":"/"), ((full_param==NULL)?"":full_param), help, ((required==true)?"(Required)":""));

        memcpy(digest->help + digest->help_len, new_entry, new_bytes);

        digest->help_len += new_bytes;
        digest->help[digest->help_len] = '\0';
    }

    digest->param_names[digest->n_params] = strdup(param_name);
    
    digest->arglist[digest->n_params] = (struct argresult_t*)malloc(sizeof(struct argresult_t));
    struct argresult_t *arg = digest->arglist[digest->n_params];
    memset(arg, 0, sizeof(struct argresult_t));

    arg->param = param;
    arg->full_param = full_param;
    arg->param_name = param_name;
    arg->type = arg_type;
    arg->required = required;

    digest->n_params += 1;
    digest->param_names = (char**)realloc(digest->param_names, (digest->n_params + 1) * sizeof(*digest->param_names));

    if (digest->param_names == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Failed to extend the Argument Digester parameter name list [realloc()]: %s\n", strerror(errno));

        return -6;
    }

    digest->arglist = (struct argresult_t**)realloc(digest->arglist, (digest->n_params + 1) * sizeof(*digest->arglist));

    if (digest->param_names == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Failed to extend the Argument Digester argument list [realloc()]: %s\n", strerror(errno));

        return -7;
    }

   return 0;
}

int ArgDigestGlobalSetOpt(ARGDIGEST_OPTION mode, void *value)
{
    if (value == NULL)
    {
        if (ARGDIGEST_ERROR_VERBOSE)
            ARGDIGEST_ERRLOG("Parameter 'value' must be a valid pointer, cannot be NULL [ArgDigestSetOpt()]\n");


        return -1;
    }

    switch (mode)
    {
        case (ARGDIGEST_SET_EXIT_ON_FAILURE):
            ARGDIGEST_EXIT_ON_FAILURE = *(bool*)value;
            break;
        case (ARGDIGEST_SET_ERROR_VERBOSITY):
            ARGDIGEST_ERROR_VERBOSE = *(bool*)value;
            break;
        case (ARGDIGEST_SET_HELP_SHORT_SWITCH):
            ARGDIGEST_HELP_SHORT_SWITCH = (char*)value;
            break;
        case (ARGDIGEST_SET_HELP_LONG_SWITCH):
            ARGDIGEST_HELP_LONG_SWITCH = (char*)value;
            break;
    }
}

int ArgDigestInvokeDigestion(ArgDigest *digest)
{
    int parsed = 0;

    for (int i = 1; i < digest->argc; ++i)
    {
        if (!strcmp(digest->argv[i], ARGDIGEST_HELP_SHORT_SWITCH) || !strcmp(digest->argv[i], ARGDIGEST_HELP_LONG_SWITCH))
        {
            puts(digest->help);
        
            if (ARGDIGEST_EXIT_ON_FAILURE)
                exit(-1);
            
            return -1;
        }

        for (int j = 0; j < digest->n_params; j++)
        {
            struct argresult_t *arg = digest->arglist[j];
            bool get_value = false;
            bool valid_param = false;

            char *cparam = NULL;

            if (arg->param)
            {
                if (!strcmp(arg->param, digest->argv[i]))
                {
                    if (arg->chosen)
                    {
                        fprintf(stderr, ARGDIGEST_ERROR_DUPLICATE_PARAM, arg->param, ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
                        fflush(stderr);

                        if (ARGDIGEST_EXIT_ON_FAILURE)
                            exit(-1);

                        return -1;
                    }
        
                    arg->chosen = true;
                    get_value = (arg->type != ARG_SWITCH);
                    cparam = arg->param;
                    valid_param = true;
                    parsed++;
                }
            }

            if (arg->full_param && valid_param == false)
            {
                if (!strcmp(arg->full_param, digest->argv[i]))
                {
                    if (arg->chosen)
                    {
                        fprintf(stderr, ARGDIGEST_ERROR_DUPLICATE_PARAM, arg->full_param, ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
                        fflush(stderr);

                        if (ARGDIGEST_EXIT_ON_FAILURE)
                            exit(-1);

                        return -1;
                    }

                    arg->chosen = true;
                    get_value = (arg->type != ARG_SWITCH);
                    cparam = arg->full_param;
                    valid_param = true;
                    parsed++;
                }
            }
            
            if (get_value)
            {
                if ((digest->argc - 1) <= i)
                {
                    fprintf(stderr, ARGDIGEST_ERROR_REQUIRES_VALUE, cparam, ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
                    fflush(stderr);
                    return -2;
                }

                for (int x = 0; x < digest->n_params; x++)
                {
                    if (digest->arglist[x]->param)
                    {
                        if (!strcmp(digest->arglist[x]->param, digest->argv[i + 1]))
                        {
                            fprintf(stderr, ARGDIGEST_ERROR_REQUIRES_VALUE, cparam, ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
                            fflush(stderr);
                            return -2;
                        }
                    }

                    if (digest->arglist[x]->full_param)
                    {
                        if (!strcmp(digest->arglist[x]->full_param, digest->argv[i + 1]))
                        {
                            fprintf(stderr, ARGDIGEST_ERROR_REQUIRES_VALUE, cparam, ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
                            fflush(stderr);
                            return -2;
                        }
                    }
                }

                switch (arg->type)
                {
                    case ARG_STR:
                        arg->value = digest->argv[i + 1];
                        break;
                    case ARG_INT:
                        arg->value = (int*)strtoul(digest->argv[i + 1], NULL, 10);
                        break;
                }

                i++;
            }
        }
    }

    for (int i = 0; i < digest->n_params; i++)
    {
        struct argresult_t *arg = digest->arglist[i];

        if (arg->required && !arg->chosen)
        {
            fprintf(stderr, ARGDIGEST_ERROR_REQUIRED_PARAM, arg->param_name, ARGDIGEST_HELP_SHORT_SWITCH, ARGDIGEST_HELP_LONG_SWITCH);
            fprintf(stderr, "Parameter Informaion: %s [%s%s%s] %s %s %s\n", \
                arg->param_name, 
                ((arg->type==ARG_STR)?"STRING":""), 
                ((arg->type==ARG_INT)?"NUMBER":""), 
                ((arg->type==ARG_SWITCH)?"BOOL":""), 
                ((arg->param!=NULL)?arg->param:""),
                ((arg->param==NULL||arg->full_param==NULL)?"":"/"),
                ((arg->full_param!=NULL)?arg->full_param:"")
            );
            fflush(stderr);

            if (ARGDIGEST_EXIT_ON_FAILURE)
                exit(-1);

            return -3;
        }
    }

    return parsed;
}

ArgResult *ArgDigestGetValue(ArgDigest *digest, char param_name[])
{
    for (int i = 0; i < digest->n_params; i++)
    {
        struct argresult_t *arg = digest->arglist[i];

        if (!strcmp(arg->param_name, param_name))
        {
            return (arg->chosen==false)?NULL:arg;
        }
    }

    return NULL;
}

void ArgDigestFree(ArgDigest *digest)
{
    for (int i = 0; i < digest->n_params; i++)
    {
        free(digest->arglist[i]);
        free(digest->param_names[i]);
    }

    if (digest->help != NULL)
    {
        free(digest->help);
    }

    for (int i = 0; i < digest->n_params_list; i++)
    {
        free(digest->param_list[i]);
    }
}

char *adlcltm() {
	time_t t;
	time(&t);

	struct tm *ti = localtime(&t);

	char lt[9];

	snprintf(lt, 9, "%02d:%02d:%02d", ti->tm_hour, ti->tm_min, ti->tm_sec);

	return strdup(lt);
}
