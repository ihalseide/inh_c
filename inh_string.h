// (For more information, see the bottom of this file).

#ifndef INH_INCLUDE_INH_STRING_H
#define INH_INCLUDE_INH_STRING_H

// --- Begin header code --- //

#ifndef ING_STRING_NO_STDLIB
#include <stdlib.h>
#endif

#ifndef INH_STRING_NO_STDBOOL
#include <stdbool.h>
#endif

#ifndef INH_STRING_NO_STDIO
#include <stdio.h>
#endif

#ifndef INH_STRING_DEF
#ifdef INH_STRING_STATIC
#define INH_STRING_DEF static
#else
#define INH_STRING_DEF extern
#endif
#endif

/*
 * The main length-encoded string structure.
 * The character data is stored in a variable-size array.
 */
typedef struct INH_string {
    size_t len;
    char buffer[];
} INH_string;

INH_STRING_DEF INH_string * str_alloc(size_t len); 

INH_STRING_DEF INH_string * str_realloc (INH_string * str, size_t new_len); 

INH_STRING_DEF INH_string * str_new_len (const char * stream, size_t len); 

INH_STRING_DEF INH_string * str_new (const char * stream); 

INH_STRING_DEF INH_string * str_new_sub (const INH_string * source, size_t start, size_t end); 

INH_STRING_DEF INH_string * str_dup (const INH_string * string); 

INH_STRING_DEF size_t str_copy_from (INH_string * dest, const INH_string * source, size_t start); 

INH_STRING_DEF size_t str_copy (INH_string * dest, const INH_string * source); 

INH_STRING_DEF INH_string * str_append (INH_string * string, char ch); 

INH_STRING_DEF INH_string * str_append_new (const INH_string * string, char ch); 

INH_STRING_DEF size_t str_write_stream (const INH_string * string, char * stream); 

INH_STRING_DEF size_t str_write_stream_term (const INH_string * string, char * stream); 

INH_STRING_DEF char * str_convert (const INH_string * str); 

INH_STRING_DEF INH_string * str_new_cat (const INH_string * first, const INH_string * next); 

INH_STRING_DEF INH_string * str_cat (INH_string ** dest, const INH_string * source); 

INH_STRING_DEF size_t str_cat_at (INH_string * dest, const INH_string * source, size_t index); 

INH_STRING_DEF INH_string * str_join (INH_string * sep, size_t len, INH_string * strings[]); 

INH_STRING_DEF bool str_equal_sub (const INH_string * str1, const INH_string * str2, size_t start, size_t end); 

INH_STRING_DEF bool str_notequal_sub (const INH_string * str1, const INH_string * str2, size_t start, size_t end); 

INH_STRING_DEF bool str_equal (const INH_string * str1, const INH_string * str2); 

INH_STRING_DEF bool str_notequal (const INH_string * str1, const INH_string * str2);

INH_STRING_DEF int str_fprint (const INH_string * string, FILE * stream); 

INH_STRING_DEF int str_print (const INH_string * string); 

INH_STRING_DEF int str_fput (const INH_string * string, FILE * stream); 

INH_STRING_DEF int str_put (const INH_string * string); 

// --- End header code --- //

#endif // INH_INCLUDE_INH_STRING_H

#ifdef INH_STRING_IMPLEMENTATION

#include <assert.h>
#include <string.h>

/*
 * String constructor
 */
INH_string * str_alloc (size_t len) {
    INH_string * new;
    new = malloc(sizeof(*new) + len);
    if (new != NULL) {
        new->len = len;
    }
    return new;
}

INH_string * str_realloc (INH_string * str, size_t new_len) {
    INH_string * result;
    result = realloc(str, sizeof(*result) + new_len);
    result->len = new_len;
    return result;
}

/*
 * Constructs a new String from a C string that is len characters long
 */
INH_string * str_new_len (const char * stream, size_t len) {
    INH_string * new = str_alloc(len);
    int i;
    for (i = 0; i < len; i++) {
        new->buffer[i] = stream[i];
    }
    return new;
}

/*
 * Constructs a new String from a C string, which must be null-terminated.
 */
INH_string * str_new (const char * stream) {
    size_t len = strlen(stream);
    return str_new_len(stream, len);
}

/*
 * Print out a String to a stream
 *
 * Returns:
 * On success, returns a non-negative value
 * On failure, returns EOF and sets the error indicator (see ferror()) on stream.
 */
int str_fprint (const INH_string * string, FILE * stream) {
    size_t i;
    for (i = 0; i < string->len; i++) {
        char c = string->buffer[i];
        int result = putc(c, stream);
        if (result != c) {
            // Failure
            return result;
        }
    }
    // Success
    return 1;
}

/*
 * Like str_fprint, but only prints to the stdout stream
 * See: str_fprint
 */
int str_print (const INH_string * string) {
    return str_fprint(string, stdout);
}

/*
 * String version of puts
 */
int str_fput (const INH_string * string, FILE * stream) {
    int result = str_fprint(string, stdout);
    if (result == 1) {
        // Only add the newline if the string could be printed
        result = putc('\n', stream); 
    }
    return result;
}

/*
 * Like str_fput, but always prints to the stdout stream
 * See: str_fput
 */
int str_put (const INH_string * string) {
    return str_fput(string, stdout);
}

/*
 * Returns length copied, which is the minimum length of the source and destination
 */
size_t str_copy_from (INH_string * dest, const INH_string * source, size_t start) {
    size_t len = (dest->len < source->len) ? dest->len : source->len;
    size_t i;
    for (i = start; i < start + len; i++) {
        dest->buffer[i - start] = source->buffer[i];
    } 
    return len;
}

size_t str_copy (INH_string * dest, const INH_string * source) {
    return str_copy_from(dest, source, 0);
}

/*
 * Duplicate a string by allocating a copy of it.
 */
INH_string * str_dup (const INH_string * string) {
    INH_string * new = str_alloc(string->len);
    size_t copy_len = str_copy(new, string);
    assert(copy_len == new->len);
    return new;
}

/*
 * Create a new String that represents the substring start...end in the
 * source String
 * start is inclusive
 * end is exclusive
 */
INH_string * str_new_sub (const INH_string * source, size_t start, size_t end) {
    assert(((int)end - (int)start) >= 0);
    INH_string * new = str_alloc(end - start);
    int i;
    for (i = start; i < end; i++) {
        new->buffer[i - start] = source->buffer[i];
    }
    return new;
}

/*
 * Append a character to a String.
 * Reallocates the string.
 * Returns the string pointer.
 *
 * Note: use str_cat if you intend to add many characters to the end of the string, it's faster.
 */
INH_string * str_append (INH_string * string, char ch) {
    string = str_realloc(string, string->len + 1);
    string->buffer[string->len - 1] = ch;
    return string;
}

/*
 * Allocate a new string that is the given string with ch appended to it.
 * Returns the new string pointer.
 */
INH_string * str_append_new (const INH_string * string, char ch) {
    INH_string * new = str_alloc(string->len + 1);
    str_copy(new, string);
    new->buffer[new->len - 1] = ch;
    return new; 
}

/*
 * Write a String to a character stream, including null characters
 *
 * Returns the number of characters written
 */
size_t str_write_stream (const INH_string * string, char * stream) {
    size_t i;
    for (i = 0; i < string->len; i++) {
        stream[i] = string->buffer[i];
    }
    return i;
}

/*
 * Write a String to a char stream, but use null-termination
 */
size_t str_write_stream_term (const INH_string * string, char * stream) {
    size_t i;
    for (i = 0; (i < string->len) && string->buffer[i] != '\0'; i++) { 
        stream[i] = string->buffer[i];
    }
    stream[i] = '\0';
    return i;
}

/*
 * Convert a String to a normal C String.
 * Allocates a new char * with enough space to hold the whole String.
 * Returns a null-terminated character stream.
 */
char * str_convert (const INH_string * str) {
    char * result = malloc(str->len + 1);
    if (!result) {
        // Malloc failed
        return result;
    }

    str_write_stream_term(str, result);
    return result;
}

/*
 * Concatenate two strings into a new string
 */
INH_string * str_new_cat (const INH_string * first, const INH_string * next) {
    INH_string * new = str_alloc(first->len + next->len);
    size_t first_end = str_copy(new, first);
    size_t i;
    for (i = first_end; i < new->len; i++) {
        new->buffer[i] = next->buffer[i - first_end];
    }
    return new;
}

/*
 * Concatenate source onto dest
 * Returns dest
 */
INH_string * str_cat (INH_string ** dest, const INH_string * source) {
    size_t orig_len = (*dest)->len;
    *dest = str_realloc(*dest, orig_len + source->len);

    int i;
    for (i = 0; i < source->len; i++) {
        (*dest)->buffer[orig_len + i] = source->buffer[i];
    }

    return *dest;
}

/*
 * Returns (index + source->len)
 */
size_t str_cat_at (INH_string * dest, const INH_string * source, size_t index) { 
    size_t i;
    for (i = 0; i < source->len; i++) {
        dest->buffer[index + i] = source->buffer[i];
    } 
    return index + i;
}

/*
 * Join a list of strings together with a separator string.
 * Returns a newly allocated string.
 */
INH_string * str_join (INH_string * sep, size_t len, INH_string * strings[]) {
    // Find out how much string needs to be allocated
    size_t total_str_len = 0;
    size_t i;
    for (i = 0; i < len; i++) {
        total_str_len += strings[i]->len; 
    }
    total_str_len += sep->len * (len - 1);

    INH_string * new = str_alloc(total_str_len);
    if (new == NULL) {
        // alloc failed
        return new;
    }

    // No elements to join together
    if (!len) {
        return new;
    }

    size_t char_i = 0;

    // Add the first element
    INH_string * str = strings[0];
    char_i = str_cat_at(new, str, char_i);

    // Add subsequent elements
    for (i = 1; i < len; i++) {
        str = strings[i];
        char_i = str_cat_at(new, sep, char_i); 
        char_i = str_cat_at(new, str, char_i);
    }

    return new;
}

/*
 * Returns if a substring of str1 and str2 are equal
 */
bool str_equal_sub (const INH_string * str1, const INH_string * str2, size_t start, size_t end) { 
    size_t sub_len = end - start;
    assert(sub_len >= 0);
    if (sub_len > str1->len || sub_len > str2->len) {
        return false;
    }
    int i;
    for (i = 0; i < sub_len; i++) {
        if (str1->buffer[i] != str2->buffer[i]) {
            return false;
        }
    }
    return true;
}

/*
 * Returns if a substring of str1 and str2 are equal
 */
bool str_notequal_sub (const INH_string * str1, const INH_string * str2, size_t start, size_t end) { 
    assert(((int)end - (int)start) >= 0);
    size_t sub_len = end - start;
    if (sub_len > str1->len || sub_len > str2->len) {
        return true;
    }
    int i;
    for (i = 0; i < sub_len; i++) {
        if (str1->buffer[i] != str2->buffer[i]) {
            return true;
        }
    }
    return false;
}

/*
 * Returns if two strings are completely equal, including length.
 */ 
bool str_equal (const INH_string * str1, const INH_string * str2) {
    if (str1 == str2) {
        return true;
    }
    if (str1->len != str2->len) {
        return false;
    }
    return str_equal_sub(str1, str2, 0, str1->len);
}

/*
 * Returns if two strings are not equal.
 */ 
bool str_notequal (const INH_string * str1, const INH_string * str2) {
    if (str1 == str2) {
        return false;
    }
    if (str1->len != str2->len) {
        return true;
    }
    return str_notequal_sub(str1, str2, 0, str1->len);
}

// --- End of implementation --- //

#endif // INH_STRING_IMPLEMENTATION

/***

= inh_string.h 0.1.0 =

This is a single file header for the use of length-encoded Strings in the C
Programming language.

== Usage ==

Write the following to use the file as a normal header:

 #include "inh_string.h"

and then in one and only one file, write the following after including any 
files that depend on this header:

 #define INH_STRING_IMPLEMENTATION
 #include "inh_string.h"

If you want the implementation to be private to the file that defines
INH_STRING_IMPLEMENTATION, you can write the following instead:

 #define INH_STRING_STATIC
 #define INH_STRING_IMPLEMENTATION
 #include "inh_string.h"

== Changelog ==

All notable changes to this project will be documented in this section.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

This project uses semantic versioning [https://semver.org].

=== [0.1.0] - 2021-03-20 ===
==== Added ====
* This changelog

== License ==

Copyright (c) 2021 Izak Nathanael Halseide

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***/

