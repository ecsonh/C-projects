#include <stdio.h>

int main()
{
    char c[256];
    int index, length;
    printf("> ");
    for(int i = 0; i< 3 ; i++)
    {
        if(i == 0)
        {
            scanf("%[^,]s", c);
        }
        else if(i == 1)
        {
            scanf(", %d", &index);
        }
        else
        {
            scanf(", %d", &length);
        }
    }

    for(int i = 0; i< length; i++)
    {
        if(c[index+ i] != '\0')
        {
            printf("%c",c[index+ i]);
        }
        else
        {
            break;
        }
    }
    printf("\n");
    return 0;
}
