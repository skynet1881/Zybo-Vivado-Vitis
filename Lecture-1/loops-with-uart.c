#include <stdio.h>

int main() 
{
   int count = 0;
   char command;

   printf("Press 'q' to exit or press any key to continue\n");

   while(1)
   {
        printf("Loop counter %d\n", count++);
        printf("Enter a command:");
        scanf(" %c", &command);

        if(command == 'q')
        {
            printf("Exit program..\n");
            break;
        }
   }

   return 0;
}
