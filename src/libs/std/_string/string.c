#include "string.h"

__String string = {
    .compare  = __string_compare,
    .join     = __string_join,
    .concat   = __string_concat,
    .copy     = __string_copy,
    .find     = __string_find,
    .fmt   = __string_format,
    .length   = __string_length,
    .buffer   = __string_buffer,
    .tokenize = __string_tokenize,
    .null     = (const char *)-1,
    .CHAR     = (__Char){__string_CHAR_first, __string_CHAR_last}
};



char* __int2str(int value)
{
    static char buffer[12];
    char* ptr = buffer + sizeof(buffer) - 1;
    *ptr = '\0';

    int isNegative = 0;
    if(value < 0)
    {
        isNegative = 1;
        value = -value;
    }

    while(value > 0)
    {
        *(--ptr) = '0' + (value % 10);
        value /= 10;
    }

    if (isNegative)
    {
        *(--ptr) = '-';
    }
    return ptr;
}

char* __size_t2str(unsigned long value)
{
    return __int2str(value);
}


char* __float2str(float value) {
    static char buffer[32];
    char* intPart = __int2str((int)value);
    float fractional = value - (int)value;
    if (fractional < 0) fractional = -fractional;

    int fractionalAsInt = (int)(fractional * 100);
    char* fracPart = __int2str(fractionalAsInt);

    char* ptr = buffer;

    while (*intPart) {
        *ptr++ = *intPart++;
    }
    *ptr++ = '.';

    if (fractionalAsInt < 10) {
        *ptr++ = '0';

    }
    while (*fracPart) {
        *ptr++ = *fracPart++;
    }
    *ptr = '\0';

    return buffer;
}

char* __char2str(char value)
{
    static char buf[2];
    buf[0] = value;
    buf[1] = '\0';
    return buf;
}


char* __bool2str(long value)
{
    static char result[6];
    result[5] = '\0';

    if (value)  // Works for both pointers and numbers
    {
        result[0] = 'T'; result[1] = 'r'; result[2] = 'u';
        result[3] = 'e'; result[4] = '\0';
    }
    else
    {
        result[0] = 'F'; result[1] = 'a'; result[2] = 'l';
        result[3] = 's'; result[4] = 'e';
    }

    return result;
}



//STRING
unsigned long __string_length(const char* str)
{
    unsigned long out = 0;
    while(*str != '\0')
    {
        ++out;
        ++str;
    }

    return out;
}

char* __string_concat(char* dest, const char* src)
{
    if (!dest || !src) return 0;
    char* ptr = dest;
    while (*ptr) ptr++;
    while (*src) *ptr++ = *src++;
    *ptr = '\0';
    return dest;
}

int __string_compare(const char* str1, const char* str2)
{
    while (*str1 != '\0' && *str1 == *str2) {
        str1++;
        str2++;
    }
    return (unsigned char)*str1 - (unsigned char)*str2;

}

#include "../memory.h"
__StringList __string_tokenize(const char* str, char sep)
{
    StringList list;
    list.size = 0;
    list.data = (char**)memory.allocate(sizeof(char*) * 64);

    if (str == ((const char*)0)) return list;

    unsigned long capacity = 64;
    unsigned long bufferSize = 0;
    char* token = (char*)memory.allocate(256); // Start with a 256-byte buffer

    while (*str)
    {
        if (*str == sep)
        {
            token[bufferSize] = '\0';  // Null-terminate the token

            // Store it in the list
            if (list.size >= capacity)
            {
                capacity *= 2;
                list.data = (char**)memory.reallocate(list.data, sizeof(char*) * capacity);
            }

            list.data[list.size++] = token;
            bufferSize = 0;
            token = (char*)memory.allocate(256);  // Allocate new space for next token
        }
        else
        {
            token[bufferSize++] = *str;
        }
        str++;
    }

    if (bufferSize > 0)  // Final token
    {
        token[bufferSize] = '\0';
        list.data[list.size++] = token;
    }
    else
    {
        memory.free(token);
    }

    return list;
}

#include "../arg.h"

char* __string_format(const char* fmt, ...) {
    if (!fmt) return (char*)0;

    va_list args;
    va_start(args, fmt);

    unsigned long total_length = 0;
    const char* current = fmt;

    while (*current) {
        if (*current == '%' && *(current + 1)) {
            current++;
            if (*current == 'd' || *current == 'i') {
                total_length += string.length(__int2str(va_arg(args, int)));
            } else if (*current == 'f') {
                total_length += string.length(__float2str(va_arg(args, double)));
            } else if (*current == 's') {
                total_length += string.length(va_arg(args, const char*));
            } else if (*current == 'c') {
                total_length += 1;
            } else if (*current == 'b') {
                total_length += string.length(__bool2str(va_arg(args, int)));
            } else if (*current == '%') {
                total_length += 1;
            }
        } else {
            total_length++;
        }
        current++;
    }

    va_end(args);

    char* buffer = string.buffer(total_length + 1);
    if (!buffer) return 0;

    char* buf_ptr = buffer;
    va_start(args, fmt);
    current = fmt;

    while (*current) {
        if (*current == '%' && *(current + 1)) {
            current++;
            const char* value = 0;
            if (*current == 'd' || *current == 'i') {
                value = __int2str(va_arg(args, int));
            } else if (*current == 'f') {
                value = __float2str(va_arg(args, double));
            } else if (*current == 's') {
                value = va_arg(args, const char*);
            } else if (*current == 'c') {
                *buf_ptr++ = (char)va_arg(args, int);
                current++;
                continue;
            } else if (*current == 'b') {
                value = __bool2str(va_arg(args, int));
            } else if (*current == '%') {
                *buf_ptr++ = '%';
                current++;
                continue;
            }

            while (value && *value) {
                *buf_ptr++ = *value++;
            }
        } else {
            *buf_ptr++ = *current;
        }
        current++;
    }

    *buf_ptr = '\0';
    va_end(args);
    return buffer;
}


char *__string_join(const char *str, ...)
{
    if (!str) return 0;

    va_list args;
    va_start(args, str);

    unsigned long total_length = 0;
    const char *current = str;

    while (current != (const char *)-1)
    {
        if (*current) // **Skip empty strings**
        {
            for (const char *s = current; *s; s++) {
                total_length++;
            }
        }
        current = va_arg(args, const char *);
    }
    va_end(args);

    char *buffer = __string_buffer(total_length + 1);
    if (!buffer) return 0;

    char *buf_ptr = buffer;
    va_start(args, str);
    current = str;

    while (current != (const char *)-1)
    {
        if (*current) // **Skip empty strings**
        {
            while (*current) {
                *buf_ptr++ = *current++;
            }
        }
        current = va_arg(args, const char *);
    }

    *buf_ptr = 0;
    va_end(args);
    return buffer;
}




char* __string_copy(char* dest, const char* src, unsigned long n)
{
    if (!dest || !src) return 0;
    char* ptr = dest;
    while (n-- && *src) *ptr++ = *src++;
    *ptr = '\0';
    return dest;
}

char* __string_find(const char* haystack, const char* needle)
{
    if (!haystack || !needle) return (char*)0;
    const char *h = haystack, *n = needle;
    while (*haystack) {
        h = haystack;
        n = needle;
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
        haystack++;
    }
    return (char*)0;
}

char* __string_CHAR_first(const char* str, int c)
{
    if (!str) return 0;
    while (*str) {
        if (*str == c) return (char*)str;
        str++;
    }
    return 0;
}

char* __string_CHAR_last(const char* str, int c)
{
    char* last = 0;
    if (!str) return 0;
    while (*str) {
        if (*str == c) last = (char*)str;
        str++;
    }
    return last;
}

char* __string_CHAR_getPosition(const char* str, int c)
{
    static char buffer[32];
    unsigned long pos = 0;
    if (!str) return 0;
    while (*str) {
        if (*str == c) {
            int i = 31;
            buffer[i--] = '\0';
            do {
                buffer[i--] = (pos % 10) + '0';
                pos /= 10;
            } while (pos > 0);
            return &buffer[i + 1];
        }
        pos++;
        str++;
    }
    return 0;
}


#define INITIAL_STRING_BUFFER_SIZE (8192)
#define MAX_STRING_BUFFER_SIZE (128 * 1024)

static char static_string_memory[MAX_STRING_BUFFER_SIZE];
static unsigned long static_string_offset = 0;

static char* string_buffer_pool = 0;
static unsigned long string_buffer_size = 0;
static unsigned long string_buffer_offset = 0;

char* __string_buffer_allocate_static(unsigned long size)
{
    if (static_string_offset + size > MAX_STRING_BUFFER_SIZE) return 0;
    char* mem = &static_string_memory[static_string_offset];
    static_string_offset += size;
    return mem;
}

char* __string_buffer(unsigned long size)
{
    if (!string_buffer_pool) {
        string_buffer_size = INITIAL_STRING_BUFFER_SIZE;
        string_buffer_pool = __string_buffer_allocate_static(string_buffer_size);
        if (!string_buffer_pool) return 0;
    }

    if (string_buffer_offset + size > string_buffer_size)
    {
        unsigned long new_size = string_buffer_size * 2;
        while (string_buffer_offset + size > new_size) {
            new_size *= 2;
        }

        if (new_size > MAX_STRING_BUFFER_SIZE) return 0;

        char* new_pool = __string_buffer_allocate_static(new_size);
        if (!new_pool) return 0;

        for (unsigned long i = 0; i < string_buffer_offset; i++) {
            new_pool[i] = string_buffer_pool[i];
        }

        string_buffer_pool = new_pool;
        string_buffer_size = new_size;
    }

    char* buf = &string_buffer_pool[string_buffer_offset];
    string_buffer_offset += size;
    return buf;
}

