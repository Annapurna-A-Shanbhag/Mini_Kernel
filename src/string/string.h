#ifndef STRING_H
#define STRING_H

#include <stddef.h>
int str_len(char *str);
void str_n_cpy(char *dest, char *src, size_t n);
int str_n_cmp(char *str1, char *str2, size_t n);
int isdigit(char c);
int str_n_cmp_i(char * str1,char* str2,int n);

#endif