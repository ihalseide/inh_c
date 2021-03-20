#define INH_STRING_IMPLEMENTATION
#include "../inh_string.h"

void test_str_cat (void) {
    INH_string * s1 = str_new("con");
    INH_string * s2 = str_new("cat");
    INH_string * s3 = str_new("concat");
    INH_string * s4 = str_new_cat(s1, s2);
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
    INH_string * s1 = str_new("%appdata%");
    assert(s1 != NULL);
    INH_string * s2 = str_dup(s1);
    assert(s2 != NULL);
    assert(str_equal(s1, s2));
    INH_string * s3 = str_new("_________ and other stuff");
    str_copy(s3, s1);
    assert(str_equal_sub(s1, s3, 0, s1->len));
    assert(!str_equal(s1, s3));

    INH_string * s4 = str_new("cheese");
    INH_string * s5 = str_dup(s4);
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
	INH_string * s = str_new(c1);
	assert(s != NULL);
	char * c2 = str_convert(s);
	assert(strcmp(c1, c2) == 0);
	free(s);
    }
}

void test_str_new (void) {
    // Test str_new_len AND str_new

    // zero-length string
    INH_string * s0 = str_new_len("cheese", 0);
    assert(s0->len == 0);

    // string as substring of c-string
    INH_string * s1 = str_new_len("cheese", 3);
    assert(s1->len == 3);
    assert(s1->buffer[0] == 'c');
    assert(s1->buffer[1] == 'h');
    assert(s1->buffer[2] == 'e');

    // string from whole c-string
    INH_string * s2 = str_new("cheese");
    assert(s2->len == 6);
    assert(s2->buffer[0] == 'c');
    assert(s2->buffer[1] == 'h');
    assert(s2->buffer[2] == 'e');
    assert(s2->buffer[3] == 'e');
    assert(s2->buffer[4] == 's');
    assert(s2->buffer[5] == 'e');

    // zero-length c-string
    INH_string * s3 = str_new("");
    assert(s3->len == 0);

    free(s0);
    free(s1);
    free(s2);
    free(s3);
}

void test_str_print (void) {
    printf("The next 3 lines should be the same:\n");

    char * c1 = "Hello, world!";
    INH_string * s1 = str_new(c1);

    puts(c1);
    str_put(s1); 
    puts(str_convert(s1));

    free(s1);
}

#define neq(_s1, _s2) assert(!str_equal(_s1, _s2)); assert( str_notequal(_s1, _s2))
#define  eq(_s1, _s2) assert( str_equal(_s1, _s2)); assert(!str_notequal(_s1, _s2))
void test_str_equal (void) {
    // Test str_equal_sub AND str_equal AND str_notequal_sub AND str_notequal
    INH_string * s1 = str_new("string 1");
    INH_string * s2 = str_new("string 2");
    INH_string * s3 = str_new("a 3rd string");
    INH_string * s4 = str_new("a 3rd string");
    INH_string * s5 = str_new("");

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
    INH_string * s1 = str_new("stinky");
	assert(s1 != NULL);

    INH_string * s2 = str_new_sub(s1, 1, 4);
	assert(s2 != NULL);

    INH_string * s3 = str_new("tin");
	assert(s3 != NULL);

    assert(str_equal(s2, s3));

    free(s1);
    free(s2);
    free(s3);
}

void test_join (void) {
    INH_string * sep = str_new(", ");
    INH_string * strings[] = {
	str_new("frog"),
	str_new("newt"),
	str_new("toad"),
	str_new("lizard"),
    };
    size_t strings_len = sizeof(strings) / sizeof(strings[0]);

    INH_string * result = str_join(sep, strings_len, strings);
    INH_string * expected = str_new("frog, newt, toad, lizard");
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
    test_str_new();
    test_str_convert();
    test_str_print();
    test_str_equal();
    test_str_dup();
    test_str_sub();
    test_str_cat();
    test_join();
}

