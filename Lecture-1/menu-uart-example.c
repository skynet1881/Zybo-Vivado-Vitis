#include <stdio.h>
#include <stdbool.h>

void print_menu_options()
{
    printf("\n--------- MENU ------------ \n");
    printf("1. Print Message\n");
    printf("2. Enter name\n");
    printf("3. Exit\n");
}

bool process_command(char command)
{
	char name[20];

    switch(command)
    {
        case '1':
            printf("Hello Zybo\n");
            return true;

        case '2':
            printf("Enter your name: ");
            scanf("%s", name);
            printf("Hello %s\n", name);
            return true;

        case '3':
            
            return false;

        default:

            printf("Invalid command\n");

            return true;
    }
}

int main() 
{
    char command;
    bool run_program = true;

    while(run_program)
    {
        print_menu_options();

        printf("Enter your choice: ");
        scanf(" %c", &command);

        run_program = process_command(command);

        if(run_program == false)
        {
            printf("Exiting program\n");

            break;
        }
    }

    return 0;
}

