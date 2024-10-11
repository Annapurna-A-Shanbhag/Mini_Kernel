#include "string.h"

int str_len(char *str)
{ // We can simplify this
  int i = 0;
  char c = *(str + i);
  while (c != '\0')
  {
    i++;
    c = *(str + i);
  }
  return i;
}

void str_n_cpy(char *dest, char *src, size_t n)
{
  for (int i = 0; i < n; i++)
  {
    *(dest + i) = *(src + i);
    if(src[i]=='\0')
       return;
  }
  dest[n] = '\0';
}

int str_n_cmp(char *str1, char *str2, size_t n)
{
  for (int i = 0; i < n; i++)
  {
    if (*(str1 + i) != *(str2 + i))
    {
      return -1;
    }
    if(str1[i]=='\0'){
      return 0;
    }
  }
  return 0;
}

int isdigit(char c)
{
  if (c >= 48 && c <= 57)
  {
    return 0;
  }
  return -1;
}

char to_lower(char c){
  if(c>=65 && c<=90){
    return c+32;
  }
  return c;

}
int str_n_cmp_i(char * str1,char* str2,int n)
{
  char c1,c2;
  while(n-- > 0){
  c1=*str1++;
  c2=*str2++;
    if(c1!=c2 && to_lower(c1)!=to_lower(c2)){
      return -1;
    }
    if(c1=='\0')
      return 0;
  }
  return 0;

}

