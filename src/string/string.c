#include "string.h"

int str_len(char* str){
    int i=0;
    char c=*(str+i);
    while(c!='\n'){
      i++;
      c=*(str+i);
    }
    return i;
}

void str_n_cpy(char* dest,char* src, size_t n ){
  for(int i=0;i<n;i++){
    *(dest+i)=*(src+i);
  }
  dest[n]='\n';

}

int str_n_cmp(char *str1,char *str2, size_t n){
  for(int i=0;i<n;i++){
    if(*(str1+i)!=*(str2+i)){
      return -1;
    }
  }
  return 0;
}