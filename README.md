# argdigest
A simple argument parser and digester for C

# Compile the library


# Compile the library

```
$ git clone https://github.com/ripmeep/argdigest/
$ cd argdigest/
$ make
```

# Run the test

```
$ gcc test.c -o test -largdigest
$ ./test # (or ./test --help)
```

_______________________

# Using the library

(Refer to `test.c` for a full example)

If you are familiar with the basics of C (unlike me) you know you can initialize main with `argc` and `argv`.
But normally, just using `argv` to parse your arguments looks weak compared to the superior `--`s and `-`s!
Now, you can (hopefully) parse your `argv` in a neater, more friendly cooler way.

Start of with a simple C file:

```c
#include <stdio.h>

int main(int argc, char **argv)
{
  printf("Hello, world!\n"); /* simple, right? */
  
  return 0;
}
```

Now, using this library has 3 simple steps:
  1. Initialize with argc and argv
  2. Add your argument parameters
  3. Digest them
  4. Invest in DOGE (not required)

# How to initialize?
`argdigest.c`
```c
int ArgDigestInit(ArgDigest *digest, int argc, char **argv, char *help, char *description);
```

(Leave `help` as NULL if you want argdigest to automatically make the help message for you)

So in your C file, you can do this:
```c
#include <stdio.h>
#include <argdigest.h>

int main(int argc, char **argv)
{
  ArgDigest args;
  
  ArgDigestInit(&args, argc, argv, NULL, "The description of your program");
  
  ...
```

Now that you are initialized, you can start adding your arguments.
Arguments in argdigest come in 3 types:
  1. String
  2. Integer
  3. Switch (boolean)

You specifiy this for each argument you make.
You need to give each argument a name, at least 1 parameter switch, and it's type.
You can also specify if the argument is required or not.

`argdigest.h`
```c
int ArgDigestAddParam(ArgDigest *digest, char param_name[], char *param, char *full_param, char help[], ARGDIGEST_TYPE arg_type, bool required);
```

Let's say that I want to make 2 parameters.
One for my name (which I will make required) and one for my age.

```c

#include <stdio.h>
#include <argdigest.h>

int main(int argc, char **argv)
{
  ArgDigest args;
  
  ArgDigestInit(&args, argc, argv, NULL, "The description of your program");
  
  ArgDigestAddParam(&args, "Name", "-N", "--name", "What is your name?", ARG_STR, true); // required = true
  ArgDigestAddParam(&args, "Age", "-A", "--age", "How old are you?", ARG_INT, false);
  
  ...
```

Now we can parse them!

Note: -h/--help is automatically reserved and will show the help message if called

```c
if (ArgDigestInvokeDigestion(&args) < 0)
  return -1; // will return negative if failed or required value was not supplied

ArgResult *name = ArgDigestGetValue(&args, "Name"); // will return NULL if user didn't call it or its not a valid parameter
ArgResult *age = ArgDigestGetValue(&args, "Age");

printf("Your name is %s\n", name->value);

if (age)
  printf("%s is %d years old\n", name->value, (int*)age->value);
```

If you are nice, you will also `free` the `ArgDigest` structure when you're done using it.

```c
ArgDigestFree(&args);
```

Any issues, please let me know! Thank you :)

Instagram: @rip.meep
