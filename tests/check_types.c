#include <check.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"

Suite *write_suite(void);

START_TEST(test_dns_type_to_int)
{
	struct test_data
	{
		char *type;
		uint16_t result;
	};

	struct test_data data[] = {
		{"example", DNS_TYPE_ERROR},
		{"A", DNS_TYPE_A},
		{"a", DNS_TYPE_A},
		{"NS", DNS_TYPE_NS},
		{"ns", DNS_TYPE_NS},
	};

	for (int i = 0; i < 5; i++)
	{
		ck_assert_int_eq(dns_type_to_int(data[i].type), data[i].result);
	}
}
END_TEST

START_TEST(test_int_to_dns_type)
{
	struct test_data
	{
		uint16_t type;
		char *result;
	};

	struct test_data data[] = {
		{9999, "ERROR"},
		{DNS_TYPE_ERROR, "ERROR"},
		{DNS_TYPE_A, "A"},
		{DNS_TYPE_NS, "NS"},
		{DNS_TYPE_AAAA, "AAAA"},
	};

	for (int i = 0; i < 5; i++)
	{
		ck_assert_int_eq(strcmp(int_to_dns_type(data[i].type), data[i].result), 0);
	}
}
END_TEST

Suite *dns_resolv_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("types");
	tc_core = tcase_create("types");

	tcase_add_test(tc_core, test_dns_type_to_int);
	tcase_add_test(tc_core, test_int_to_dns_type);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	SRunner *sr;

	sr = srunner_create(dns_resolv_suite());
	srunner_add_suite(sr, write_suite());
	srunner_run_all(sr, CK_NORMAL);

	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}