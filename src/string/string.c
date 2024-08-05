#include "string.h"

int strlen(const char* str){
    int i=0;
    char c=*(str+i);
    while(c!='\n'){
      i++;
      c=*(str+i);
    }
    return i;
}