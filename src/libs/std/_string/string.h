#ifndef STRING_H
#define STRING_H

#define String char*
#define StringList __StringList

#define MAX_LIST_SIZE 2048

typedef struct __ToString
{
    char* (*INT)    (int);
    char* (*FLOAT)  (float);
    char* (*CHAR)   (char);
    char* (*BOOL)   (long);
    char* (*SIZE_T) (unsigned long);
} __ToString;

typedef struct __StringList
{
    unsigned long size;
    char** data;
} __StringList;


char* __int2str     (int   value);
char* __float2str   (float value);
char* __char2str    (char  value);
char* __bool2str    (long value);
char* __size_t2str  (unsigned long value);

#define toString   ((__ToString){__int2str, __float2str, __char2str, __bool2str, __size_t2str})

typedef struct __Char
{
    char* (*first) (const char*,int);
    char* (*last)  (const char*, int);
} __Char;

typedef struct __String
{
    unsigned long (*length)   (const char*);
    char* (*concat)   (char*, const char*);
    char* (*join)     (const char*, ...);
    int   (*compare)  (const char *, const char*);
    char* (*copy)     (char*, const char *, unsigned long);
    char* (*find)     (const char*, const char*);
    char* (*buffer)   (unsigned long size);
    __StringList (*tokenize) (const char*, char);
    char* (*fmt)   (const char*, ...);

    const char* null;
    struct __Char CHAR;
} __String;

unsigned long __string_length      (const char* str);
char * __string_concat      (char* dest, const char* src);
int    __string_compare     (const char* str1, const char* str2);
char * __string_copy        (char* dest, const char* src, unsigned long n);
char * __string_find        (const char* haystack, const char* needle);
char * __string_join        (const char* str, ...);
char * __string_CHAR_first  (const char* str, int c);
char * __string_CHAR_last   (const char* str, int c);
char * __string_format      (const char* fmt, ...);
__StringList __string_tokenize    (const char* str, char sep);
extern __String string;

char* __string_buffer_allocate_static(unsigned long size);
char* __string_buffer(unsigned long size);

#endif
