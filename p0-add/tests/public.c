#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../p0-add.h"

/* simple addition */
START_TEST (C_simple_add)
{
    ck_assert_int_eq (add_abs(2,3), 5);
}
END_TEST

void public_tests (Suite *s)
{
    TCase *tc_public = tcase_create ("Public");
    tcase_add_test (tc_public, C_simple_add);
    suite_add_tcase (s, tc_public);
}

