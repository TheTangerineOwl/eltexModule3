#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    double sum = 0;
    if (argc < 2)
        return 0;

    for (int i = 1; i < argc; i++)
        sum += atof(argv[i]);
    
    printf("Сумма: %.2f\n", sum);
    return 0;
}