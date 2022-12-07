#ifndef OBFUSCATOR
#define OBFUSCATOR

char* decrypt(char* str) {
    size_t szString = strlen(str);
    for (size_t i = 0; i < szString-1; i++)
    {
       str[i] ^= szString;
    }
    return str;
}

#endif