#include <stdio.h>
#include <string.h>
#include "../src/obfuscator.h"

int main(int argc, char const *argv[])
{
    char* literal = "Hide me";
    printf("Before: %s\n", literal);
    printf("After: %s\n", decrypt(literal));
    return 0;
}
