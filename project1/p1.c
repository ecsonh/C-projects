#include <stdio.h>
#include <string.h>
#include <ctype.h>
void descending(char*c, int length)
{
    
    char t;
    for(int i = 0; i<length ;i++)
    {
        for(int j = 0; j < length; j++)
        {
            if (c[i] < c[j])
            {
                t = c[i];
                c[i] = c[j];
                c[j] = t;
            }
        }
    }
}
void removespace(char* str, int length, char*str1)
{
    
    int j =0;
    for(int i = 0; i< length;i++)
    {
        if(isalpha(str[i]) != 0)
        {
            str1[j] = str[i];
            j++;
        }
        
    }
    str1[j] = 0;

}
void findmin(int *map, char *ctr)
{
    while(1)
    {
        int min = 1000000;
        int index;
        for(int i = 0; i<26; i++)
        {
            if(map[i]< min && map[i]>0)
            {
                min = map[i];
                index = i;
            }
        }
        if(min == 1000000)
        {
            break;
        }
        map[index] = 0;
        printf("%c: %d \n", ctr[index], min);
    }
}
int main()
{
    char str1[256];
    char c[256];
    char ctr[26] = "abcdefghijklmnopqrstuvwxyz";
    printf("> ");
    scanf("%[^\n]s", c);
    
    int count, map[26];
    int length = strlen(c);
    for(int j = 0; j< 26; j++) //set all to zero
    {
        map[j]=0;
    }
    for(int i = 0 ; i<length ;i++)
    {
        for(int j = 0; j< 26; j++)
        {
            if (c[i] == ctr[j])
            {
                map[j] +=1;
            }
        }
    }
    findmin(map, ctr);

    removespace(c, length, str1);
    length = strlen(str1);
    descending(str1, length);
    
    printf("%s",str1);
    printf("\n");
    return 0;
}