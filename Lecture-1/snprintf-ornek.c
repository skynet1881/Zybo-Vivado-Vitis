#include <stdio.h>

// snprintf usage example, to update the string with the new value
int main() 
{
   int year; 
   printf("Enter your birth year: ");
   scanf("%d", &year);
   
   int age = 2025 - year;
   printf("Your age %d in 2025\n", age);

   char message[100];
   snprintf(message, sizeof(message), "In 10 years, you will be %d years old", age + 10);

   printf("%s\n", message);

   return 0;
}
