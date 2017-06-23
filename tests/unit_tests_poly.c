/*
 * Unit tests for COMPOSE functionality.
 */
#include "test_utils.h"


static void test_compose_fn_helper(Poly p, unsigned count, Poly components[], Poly expected) {
  Poly result = PolyCompose(&p, count, components);
  assert_poly_equal(&result, &expected);
  PolyDestroy(&result);
  PolyDestroy(&p);
  for(unsigned i=0;i<count;++i) {
    PolyDestroy(&components[i]);
  }
  PolyDestroy(&expected);
}

static void test_compose_fn_poly_0_count_0(void **state) {
    (void)state;
    test_compose_fn_helper(
      Poly0,
      0,
      PolyL({0}),
      Poly0
    );
}

static void test_compose_fn_poly_0_count_1(void **state) {
    (void)state;
    test_compose_fn_helper(
      Poly0,
      1,
      PolyL(PolyC(42)),
      Poly0
    );
}

static void test_compose_fn_poly_const_count_0(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyC(42),
      0,
      PolyL({0}),
      PolyC(42)
    );
}

static void test_compose_fn_poly_const_count_1(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyC(42),
      1,
      PolyL(PolyC(69)),
      PolyC(42)
    );
}

static void test_compose_fn_poly_linear_count_0(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyP(PolyC(4), 1),
      0,
      PolyL({0}),
      PolyP(PolyC(4), 1)
    );
}

static void test_compose_fn_poly_linear_count_1_const(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyP(PolyC(4), 1),
      1,
      PolyL(PolyC(42)),
      PolyC(42*4)
    );
}

static void test_compose_fn_poly_linear_count_1_linear(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyP(PolyC(4), 1),
      1,
      PolyL(PolyP(PolyC(1), 0, PolyC(2), 1)),
      PolyP(PolyC(4), 0, PolyC(8), 1)
    );
}

static void test_parser_compose_no_arg(void **state) {
    (void)state;
    mock_run_calc_main(
      "COMPOSE\n",
      "",
      "ERROR 1 WRONG COUNT\n",
      0
    );
}

static void test_parser_compose_one_poly(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,5)\nCOMPOSE 0\nPRINT\n",
      "(4,5)\n",
      "",
      0
    );
}

static void test_parser_compose_max_unsigned(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)\n(256,4)+(55,8)+(11,11)\n69\nCOMPOSE 4294967295\nPRINT\n",
      "69\n",
      "ERROR 4 STACK UNDERFLOW\n",
      0
    );
}

static void test_parser_compose_minus_one(void **state) {
    (void)state;
    mock_run_calc_main(
      "(120,120)+(6,9)\nCOMPOSE -1\nPRINT\n",
      "(6,9)+(120,120)\n",
      "ERROR 2 WRONG COUNT\n",
      0
    );
}

static void test_parser_compose_max_unsigned_overflow(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE 4294967296\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

static void test_parser_compose_overflow(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE 4294967296000000000000000000000000000000000001\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

static void test_parser_compose_count_letters(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE LubieplackiLubiePLADZKIplackiBARDZOdobreSOMplackiYEEEY\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

static void test_parser_compose_count_letters_digits_combination(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE 70Lubi3epla50cki90Lub12iePL32AdZ\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

int main(void) {

    const struct CMUnitTest compose_fn_tests[] = {
      cmocka_unit_test(test_compose_fn_poly_0_count_0),
      cmocka_unit_test(test_compose_fn_poly_0_count_1),
      cmocka_unit_test(test_compose_fn_poly_const_count_0),
      cmocka_unit_test(test_compose_fn_poly_const_count_1),
      cmocka_unit_test(test_compose_fn_poly_linear_count_0),
      cmocka_unit_test(test_compose_fn_poly_linear_count_1_const),
      cmocka_unit_test(test_compose_fn_poly_linear_count_1_linear)
    };

    const struct CMUnitTest parser_compose_tests[] = {
      cmocka_unit_test(test_parser_compose_no_arg),
      cmocka_unit_test(test_parser_compose_one_poly),
      cmocka_unit_test(test_parser_compose_max_unsigned),
      cmocka_unit_test(test_parser_compose_minus_one),
      cmocka_unit_test(test_parser_compose_max_unsigned_overflow),
      cmocka_unit_test(test_parser_compose_overflow),
      cmocka_unit_test(test_parser_compose_count_letters),
      cmocka_unit_test(test_parser_compose_count_letters_digits_combination)
    };

    int status = 0;

    status |= cmocka_run_group_tests_name("compose function tests", compose_fn_tests, NULL, NULL);
    status |= cmocka_run_group_tests_name("COMPOSE parsing calculator tests", parser_compose_tests, NULL, NULL);

    return status;

}
