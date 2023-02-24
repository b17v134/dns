#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "write.h"

START_TEST(test_write_qname)
{
    void* buf;
    buf = malloc(sizeof(uint8_t) * 11);
    ck_assert_ptr_ne(buf, NULL);
    char result[11] = { 4, 't', 'e', 's', 't', 5, 'l', 'o', 'c', 'a', 'l' };
    int len = write_qname(buf, "test.local");
    ck_assert_int_eq(memcmp((void*)result, buf, 11), 0);
    ck_assert_int_eq(len, 11);
    free(buf);
}
END_TEST

Suite* write_suite(void)
{
    Suite* s;
    TCase* tc_core;

    s = suite_create("write");
    tc_core = tcase_create("write");

    tcase_add_test(tc_core, test_write_qname);

    suite_add_tcase(s, tc_core);

    return s;
}
