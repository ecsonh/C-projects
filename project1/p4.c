#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    struct Car{
        int id;
        float miles;
    } c1[10];
    for(int i =1; i<=10;i++)
    {
        c1[i].id = 0;
        c1[i].miles = 0.0;
    }
    char c[100];
    int index = 1;
    printf("> ");
    scanf("%[^\n]%*c", c);
    
    char * token = strtok(c, " ");
    while(strcmp(token,"quit") != 0)
    {
        if(strcmp(token, "AddCar") == 0)
        {
            int match = 0;
            token = strtok(NULL, " ");
            for(int i = 1; i <= 10; i++)
            {
                if(c1[i].id == atoi(token))
                {
                    match = 1;
                }
            }
            if(match == 1)
            {
                printf("Error! Car with ID %d already exists in the database.\n", atoi(token));
            }
            else
            {
                
                c1[index].id = atoi(token);
                c1[index].miles = 0.0;
                index++;
            }
            
        }
        else if(strcmp(token, "AddTrip") == 0)
        {
            token = strtok(NULL, " ");
            int match = 0;
            for(int i = 1; i <= 10; i++)
            {
                if(c1[i].id == atoi(token))
                {
                    token = strtok(NULL, " ");
                    c1[i].miles += atof(token);
                    match = 1;
                }
            }
            if(match == 0)
            {
                printf("Error! Car with ID %d doesn’t exist in the database.\n", atoi(token));
            }
            
        }
        else if(strcmp(token, "Reset") == 0)
        {
            token = strtok(NULL, " ");
            for(int i = 1; i <= 10; i++)
            {
                if(c1[i].id == atoi(token))
                {
                    c1[i].miles = 0.0;
                }
            }
        }
        else if(strcmp(token, "Display") == 0)
        {
            for(int i = 1; i <= 10; i++)
            {
                if(c1[i].id != 0)
                {
                    printf("%d\t%.1f\n", c1[i].id, c1[i].miles);
                }
            }
        }
        printf("> ");
        scanf("%[^\n]%*c", c);
        token = strtok(c, " ");
    }
    printf("$");
    return 0;
}