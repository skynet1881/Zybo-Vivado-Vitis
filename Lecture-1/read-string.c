#include <stdio.h>

int main() 
{
    char name[50];

    // Get input from user
    printf("Enter your name: ");
    scanf("%s", name);

    printf("Hello, %s!\n Zybo sees you! \n", name);

    return 0;
}
