#ifndef __STRING_H__
#define __STRING_H__
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, int n);
char *strchr(const char *s, int c);
unsigned int strlen(const char *s);
void strncpy(void *dest, const void *src, unsigned long num);
void *memset(void *s, int c, unsigned long n);
int strbeg(const char *str, char *beg);
void strcpy(char *dest, const char *src);
char *strdup(const char *str);

int isupper(char c);
int islower(char c);
char tolower(char c);
char toupper(char c);

#define EQS(xs, ys) (!strcmp(xs, ys))
#endif
