#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 0;
    
    int maxl = 0;
    char *longest = argv[1];
    
    for (int i = 1; i < argc; i++)
    {
        int len = strlen(argv[i]);
        if (len > maxl)
        {
            maxl = len;
            longest = argv[i];
        }
    }
    
    printf("Самая длинная строка (длина: %d): %s\n", maxl, longest);
    return 0;
}