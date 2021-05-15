#include <unistd.h>

int main(int argc, char **argv)
{
        int count = 5;
        bool eof = true;

        ArgDigest args;

        ArgDigestInit(&args, argc, argv, NULL, "\nA test binary for my Argument Digster!");
        ArgDigestGlobalSetOpt(ARGDIGEST_SET_EXIT_ON_FAILURE, &eof); // EXIT ON USER FAILURE

        /* Count, integer type, NOT required (defaul = 5) */
        ArgDigestAddParam(&args, "Count", "-C", "--count", "How many times do you want to print your name? (Default = 5)", ARG_INT, false);

        /* Red, switch type, NOT required */
        ArgDigestAddParam(&args, "Red", "-R", NULL, "Print it in red?", ARG_SWITCH, false);

        /* Name, string type, required */
        ArgDigestAddParam(&args, "Name", "-N", "--name", "What is your name?", ARG_STR, true);

        ArgDigestInvokeDigestion(&args);

        ArgResult *arg_name = ArgDigestGetValue(&args, "Name");
        ArgResult *arg_red = ArgDigestGetValue(&args, "Red");
        ArgResult *arg_count = ArgDigestGetValue(&args, "Count");

        if (arg_count)
                count = *(int*)&arg_count->value;

        if (arg_red)
                printf("\033[01;31m");

        printf("Args:\n\tName - %s\n\tCount - %d\n\tRed - %s\n", arg_name->value, count, (arg_red)?"true":"false");

        for (int i = 0; i < count; i++)
        {
                puts(arg_name->value);
        }

        ArgDigestFree(&args);

        return 0;
}
