// For more information such as usage and the license see the bottom of this file

// --- Begin header code --- //

#ifndef INH_INCLUDE_INH_STRING_H
#define INH_INCLUDE_INH_STRING_H

#include <stdlib.h>
#include <stdbool.h>

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
    typedef struct INH_String {
        size_t len;
        char buffer[];
    } INH_String;

    INH_STRING_DEF String * str_alloc(size_t len); 

    INH_STRING_DEF String * str_realloc (String * str, size_t new_len); 

    INH_STRING_DEF String * str_new_len (const char * stream, size_t len); 

    INH_STRING_DEF String * str_new (const char * stream); 

    INH_STRING_DEF size_t str_copy_from (String * dest, const String * source, size_t start); 

    INH_STRING_DEF size_t str_copy (String * dest, const String * source); 

    INH_STRING_DEF String * str_dup (const String * string); 

    INH_STRING_DEF String * str_sub (const String * source, size_t start, size_t end); 

    INH_STRING_DEF String * str_append (String * string, char ch); 

    INH_STRING_DEF String * str_append_new (const String * string, char ch); 

    INH_STRING_DEF size_t str_write_stream (const String * string, char * stream); 

    INH_STRING_DEF size_t str_write_stream_term (const String * string, char * stream); 

    INH_STRING_DEF char * str_convert (const String * str); 

    INH_STRING_DEF String * str_new_cat (const String * first, const String * next); 

    INH_STRING_DEF String * str_cat (String ** dest, const String * source); 

    INH_STRING_DEF size_t str_cat_at (String * dest, const String * source, size_t index); 

    INH_STRING_DEF String * str_join (String * sep, size_t len, String * strings[]); 

    INH_STRING_DEF bool str_equal_sub (const String * str1, const String * str2, size_t start, size_t end); 

    INH_STRING_DEF bool str_notequal_sub (const String * str1, const String * str2, size_t start, size_t end); 

    INH_STRING_DEF bool str_equal (const String * str1, const String * str2); 

    INH_STRING_DEF bool str_notequal (const String * str1, const String * str2);

#ifndef INH_STRING_NO_IO

    INH_STRING_DEF int str_fprint (const String * string, FILE * stream); 

    INH_STRING_DEF int str_print (const String * string); 

    INH_STRING_DEF int str_fput (const String * string, FILE * stream); 

    INH_STRING_DEF int str_put (const String * string); 

#endif

// --- End header code --- //

#endif // INH_INCLUDE_INH_STRING_H

#ifdef INH_STRING_IMPLEMENTATION

#include <assert.h>
#include <string.h>

#ifndef INH_STRING_NO_IO
#include <stdio.h>
#endif

    /*
     * String constructor
     */
    String * str_alloc (size_t len) {
        String * new;
        new = malloc(sizeof(*new) + len);
        if (new != NULL) {
            new->len = len;
        }
        return new;
    }

    String * str_realloc (String * str, size_t new_len) {
        String * result;
        result = realloc(str, sizeof(*result) + new_len);
        result->len = new_len;
        return result;
    }

    /*
     * Constructs a new String from a C string that is len characters long
     */
    String * str_new_len (const char * stream, size_t len) {
        String * new = str_alloc(len);
        int i;
        for (i = 0; i < len; i++) {
            new->buffer[i] = stream[i];
        }
        return new;
    }

    /*
     * Constructs a new String from a C string, which must be null-terminated.
     */
    String * str_new (const char * stream) {
        size_t len = strlen(stream);
        return str_new_len(stream, len);
    }

#ifndef INH_STRING_NO_IO

    /*
     * Print out a String to a stream
     *
     * Returns:
     * On success, returns a non-negative value
     * On failure, returns EOF and sets the error indicator (see ferror()) on stream.
     */
    int str_fprint (const String * string, FILE * stream) {
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
    int str_print (const String * string) {
        return str_fprint(string, stdout);
    }

    /*
     * String version of puts
     */
    int str_fput (const String * string, FILE * stream) {
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
    int str_put (const String * string) {
        return str_fput(string, stdout);
    }

#endif // INH_STRING_NO_IO

    /*
     * Returns length copied, which is the minimum length of the source and destination
     */
    size_t str_copy_from (String * dest, const String * source, size_t start) {
        size_t len = (dest->len < source->len) ? dest->len : source->len;
        size_t i;
        for (i = start; i < start + len; i++) {
            dest->buffer[i - start] = source->buffer[i];
        } 
        return len;
    }

    size_t str_copy (String * dest, const String * source) {
        return str_copy_from(dest, source, 0);
    }

    /*
     * Duplicate a string by allocating a copy of it.
     */
    String * str_dup (const String * string) {
       String * new = str_alloc(string->len);
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
    String * str_sub (const String * source, size_t start, size_t end) {
        assert(((int)end - (int)start) >= 0);
        String * new = str_alloc(end - start);
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
    String * str_append (String * string, char ch) {
        string = str_realloc(string, string->len + 1);
        string->buffer[string->len - 1] = ch;
        return string;
    }

    /*
     * Allocate a new string that is the given string with ch appended to it.
     * Returns the new string pointer.
     */
    String * str_append_new (const String * string, char ch) {
        String * new = str_alloc(string->len + 1);
        str_copy(new, string);
        new->buffer[new->len - 1] = ch;
        return new; 
    }

    /*
     * Write a String to a character stream, including null characters
     *
     * Returns the number of characters written
     */
    size_t str_write_stream (const String * string, char * stream) {
        size_t i;
        for (i = 0; i < string->len; i++) {
            stream[i] = string->buffer[i];
        }
        return i;
    }

    /*
     * Write a String to a char stream, but use null-termination
     */
    size_t str_write_stream_term (const String * string, char * stream) {
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
    char * str_convert (const String * str) {
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
    String * str_new_cat (const String * first, const String * next) {
        String * new = str_alloc(first->len + next->len);
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
    String * str_cat (String ** dest, const String * source) {
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
    size_t str_cat_at (String * dest, const String * source, size_t index) { 
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
    String * str_join (String * sep, size_t len, String * strings[]) {
        // Find out how much string needs to be allocated
        size_t total_str_len = 0;
        size_t i;
        for (i = 0; i < len; i++) {
            total_str_len += strings[i]->len; 
        }
        total_str_len += sep->len * (len - 1);

        String * new = str_alloc(total_str_len);
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
        String * str = strings[0];
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
    bool str_equal_sub (const String * str1, const String * str2, size_t start, size_t end) { 
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
    bool str_notequal_sub (const String * str1, const String * str2, size_t start, size_t end) { 
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
    bool str_equal (const String * str1, const String * str2) {
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
    bool str_notequal (const String * str1, const String * str2) {
        if (str1 == str2) {
            return false;
        }
        if (str1->len != str2->len) {
            return true;
        }
        return str_notequal_sub(str1, str2, 0, str1->len);
    }

// --- Begin test code --- //

#ifdef INH_STRING_TEST

        void test_str_cat (void) {
            String * s1 = str_new("con");
            String * s2 = str_new("cat");
            String * s3 = str_new("concat");
            String * s4 = str_new_cat(s1, s2);
            assert(s4 != NULL);
            assert(str_notequal(s1, s3));
            assert(str_equal(s3, s4));

            str_cat(&s1, s2);
            assert(str_equal(s1, s3));
            assert(str_equal(s1, s4));

            free(s1);
            free(s2);
            free(s3);
            free(s4);
        }

        void test_str_dup (void) {
            // test str_dup and str_copy
            String * s1 = str_new("%appdata%");
            assert(s1 != NULL);
            String * s2 = str_dup(s1);
            assert(s2 != NULL);
            assert(str_equal(s1, s2));
            String * s3 = str_new("_________ and other stuff");
            str_copy(s3, s1);
            assert(str_equal_sub(s1, s3, 0, s1->len));
            assert(!str_equal(s1, s3));

            String * s4 = str_new("cheese");
            String * s5 = str_dup(s4);
            assert(str_equal(s4, s5));
            str_put(s4);
            str_put(s5);

            free(s1);
            free(s2);
            free(s3);
            free(s4);
            free(s5);
        }

        void test_str_convert (void) {
            // note: this test uses strcmp from <string.h>
            char * cstrings[] = {
                "",
                " ",
                "1",
                "22",
                "333",
                "boy",
                "hello",
                "9283i2i3j",
                "dfioawfjhiw32ejfhksldjfskdfjas;df",
                "Hello, world!\n",
            };
            size_t length = sizeof(cstrings) / sizeof(*cstrings);
            assert(length == 10); // note: manually update this length to match the above array

            size_t i;
            for (i = 0; i < length; i++) {
                char * c1 = cstrings[i];
                String * s = str_new(c1);
                assert(s != NULL);
                char * c2 = str_convert(s);
                assert(strcmp(c1, c2) == 0);
                free(s);
            }
        }

        void test_str_new (void) {
            // Test str_new_len AND str_new

            // zero-length string
            String * s0 = str_new_len("cheese", 0);
            assert(s0->len == 0);

            // string as substring of c-string
            String * s1 = str_new_len("cheese", 3);
            assert(s1->len == 3);
            assert(s1->buffer[0] == 'c');
            assert(s1->buffer[1] == 'h');
            assert(s1->buffer[2] == 'e');

            // string from whole c-string
            String * s2 = str_new("cheese");
            assert(s2->len == 6);
            assert(s2->buffer[0] == 'c');
            assert(s2->buffer[1] == 'h');
            assert(s2->buffer[2] == 'e');
            assert(s2->buffer[3] == 'e');
            assert(s2->buffer[4] == 's');
            assert(s2->buffer[5] == 'e');

            // zero-length c-string
            String * s3 = str_new("");
            assert(s3->len == 0);

            free(s0);
            free(s1);
            free(s2);
            free(s3);
        }

        void test_str_print (void) {
            printf("The next 3 lines should be the same:\n");

            char * c1 = "Hello, world!";
            String * s1 = str_new(c1);

            puts(c1);
            str_put(s1); 
            puts(str_convert(s1));

            free(s1);
        }

#define neq(_s1, _s2) assert(!str_equal(_s1, _s2)); assert( str_notequal(_s1, _s2))
#define  eq(_s1, _s2) assert( str_equal(_s1, _s2)); assert(!str_notequal(_s1, _s2))
        void test_str_equal (void) {
            // Test str_equal_sub AND str_equal AND str_notequal_sub AND str_notequal
            String * s1 = str_new("string 1");
            String * s2 = str_new("string 2");
            String * s3 = str_new("a 3rd string");
            String * s4 = str_new("a 3rd string");
            String * s5 = str_new("");

            // Test self-equality
            eq(s1, s1);
            eq(s2, s2);
            eq(s3, s3); 
            eq(s4, s4); 
            eq(s5, s5); 

            // substring equality with s1 and s2
            int i;
            for (i = 0; i < 6; i++) {
                assert(str_equal_sub(s1, s2, 0, i));
                assert(!str_notequal_sub(s1, s2, 0, i));
            } 

            // different strings with same content
            eq(s3, s4);

            neq(s1, s2);
            neq(s2, s1);

            neq(s1, s3);
            neq(s3, s1);

            neq(s1, s4);
            neq(s4, s1);

            neq(s1, s5);
            neq(s5, s1);

            neq(s2, s3);
            neq(s3, s2);

            free(s1);
            free(s2);
            free(s3);
            free(s4);
            free(s5);
        }
#undef eq
#undef neq

        void test_str_sub (void) {
            String * s1 = str_new("stinky");
                assert(s1 != NULL);

            String * s2 = str_sub(s1, 1, 4);
                assert(s2 != NULL);

            String * s3 = str_new("tin");
                assert(s3 != NULL);

            assert(str_equal(s2, s3));

            free(s1);
            free(s2);
            free(s3);
        }

        void test_join (void) {
            String * sep = str_new(", ");
            String * strings[] = {
                str_new("frog"),
                str_new("newt"),
                str_new("toad"),
                str_new("lizard"),
            };
            size_t strings_len = sizeof(strings) / sizeof(strings[0]);

            String * result = str_join(sep, strings_len, strings);
            String * expected = str_new("frog, newt, toad, lizard");
            str_put(result);
            str_put(expected);
            assert(str_equal(result, expected));

            int i;
            for (i = 0; i < strings_len; i++) {
                free(strings[i]);
            }
            free(sep);
            free(result);
            free(expected);

        }

        int main () {
            // Run all tests    
            test_str_new();
            test_str_convert();
            test_str_print();
            test_str_equal();
            test_str_dup();
            test_str_sub();
            test_str_cat();
            test_join();
        }

// --- End of test code --- //

#endif // INH_STRING_TEST

// --- End of implementation --- //

#endif // INH_STRING_IMPLEMENTATION

/* 

== About ==

This is a single file header for the use of length-encoded Strings in the C
Programming language.

== Usage ==

Write the following to use the file as a normal header:

    #include "inh_string.h"

and then in one and only one file, write the following after including any 
files that depend on this header:

    #define INH_STRING_IMPLEMENTATION
    #include "inh_string.h"

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

*/

