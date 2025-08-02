#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char result[1024] = "";
    if (argc < 2)
        return 0;
    for (int i = 1; i < argc; i++)
        strcat(result, argv[i]);
    
    printf("%s\n", result);
    return 0;
}