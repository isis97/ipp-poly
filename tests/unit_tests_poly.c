/*
* Unit tests for COMPOSE functionality.
*/
#include "test_utils.h"

/*
* Compose helper function witch do the PolyCompose composition
* And checks if result is matching expected polynomial
* Then cleanups everything
*/
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

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                0
*      count:            0
*      components:      [ ]
*   expected output:     0
*/
static void test_compose_fn_poly_0_count_0(void **state) {
    (void)state;
    test_compose_fn_helper(
      Poly0,
      0,
      PolyL({0}),
      Poly0
    );
}

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                0
*      count:            1
*      components:      [ 42 ]
*   expected output:     0
*/
static void test_compose_fn_poly_0_count_1(void **state) {
    (void)state;
    test_compose_fn_helper(
      Poly0,
      1,
      PolyL(PolyC(42)),
      Poly0
    );
}

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                42
*      count:            0
*      components:      [ ]
*   expected output:     42
*/
static void test_compose_fn_poly_const_count_0(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyC(42),
      0,
      PolyL({0}),
      PolyC(42)
    );
}

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                42
*      count:            1
*      components:      [ 69 ]
*   expected output:     42
*/
static void test_compose_fn_poly_const_count_1(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyC(42),
      1,
      PolyL(PolyC(69)),
      PolyC(42)
    );
}

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                4x
*      count:            0
*      components:      [ ]
*   expected output:     4x
*/
static void test_compose_fn_poly_linear_count_0(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyP(PolyC(4), 1),
      0,
      PolyL({0}),
      PolyP(PolyC(4), 1)
    );
}

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                4x
*      count:            1
*      components:      [ 42 ]
*   expected output:     168
*/
static void test_compose_fn_poly_linear_count_1_const(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyP(PolyC(4), 1),
      1,
      PolyL(PolyC(42)),
      PolyC(168)
    );
}

/*
* Single test of PolyCompose
*   description:        single composition test
*   input:
*      p:                4x
*      count:            1
*      components:      [ 1+2x ]
*   expected output:     4+8x
*/
static void test_compose_fn_poly_linear_count_1_linear(void **state) {
    (void)state;
    test_compose_fn_helper(
      PolyP(PolyC(4), 1),
      1,
      PolyL(PolyP(PolyC(1), 0, PolyC(2), 1)),
      PolyP(PolyC(4), 0, PolyC(8), 1)
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      COMPOSE
*   expected std output:    NONE
*   expected err output:    WRONG COUNT ERROR
*/
static void test_parser_compose_no_arg(void **state) {
    (void)state;
    mock_run_calc_main(
      "COMPOSE\n",
      "",
      "ERROR 1 WRONG COUNT\n",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (4,5)
*      COMPOSE 0
*      PRINT
*   expected std output:    
*      (4,5)
*   expected err output:    NONE
*/
static void test_parser_compose_one_poly(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,5)\nCOMPOSE 0\nPRINT\n",
      "(4,5)\n",
      "",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (4,2)
*      (256,4)+(55,8)+(11,11)
*      69
*      COMPOSE 4294967295
*      PRINT
*   expected std output:    
*      69
*   expected err output:    STACK UNDERFLOW
*/
static void test_parser_compose_max_unsigned(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)\n(256,4)+(55,8)+(11,11)\n69\nCOMPOSE 4294967295\nPRINT\n",
      "69\n",
      "ERROR 4 STACK UNDERFLOW\n",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (120,120)+(6,9)
*      COMPOSE -1
*      PRINT
*   expected std output:    
*      (6,9)+(120,120)
*   expected err output:    WRONG COUNT
*/
static void test_parser_compose_minus_one(void **state) {
    (void)state;
    mock_run_calc_main(
      "(120,120)+(6,9)\nCOMPOSE -1\nPRINT\n",
      "(6,9)+(120,120)\n",
      "ERROR 2 WRONG COUNT\n",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (4,2)+(2,4)
*      (55,8)+(256,4)+(11,11)
*      COMPOSE 4294967296
*      PRINT
*   expected std output:    
*      (256,4)+(55,8)+(11,11)
*   expected err output:    WRONG COUNT
*/
static void test_parser_compose_max_unsigned_overflow(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE 4294967296\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (4,2)+(2,4)
*      (55,8)+(256,4)+(11,11)
*      COMPOSE 4294967296000000000000000000000000000000000001
*      PRINT
*   expected std output:    
*      (256,4)+(55,8)+(11,11)
*   expected err output:    WRONG COUNT
*/
static void test_parser_compose_overflow(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE 4294967296000000000000000000000000000000000001\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (4,2)+(2,4)
*      (55,8)+(256,4)+(11,11)
*      COMPOSE LubieplackiLubiePLADZKIplackiBARDZOdobreSOMplackiYEEEY
*      PRINT
*   expected std output:    
*      (256,4)+(55,8)+(11,11)
*   expected err output:    WRONG COUNT
*/
static void test_parser_compose_count_letters(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE LubieplackiLubiePLADZKIplackiBARDZOdobreSOMplackiYEEEY\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

/*
* Single test of calculator op COMPOSE
*   description:        single calculator composition test
*   input:
*      (4,2)+(2,4)
*      (55,8)+(256,4)+(11,11)
*      COMPOSE 70Lubi3epla50cki90Lub12iePL32AdZ
*      PRINT
*   expected std output:    
*      (256,4)+(55,8)+(11,11)
*   expected err output:    WRONG COUNT
*/
static void test_parser_compose_count_letters_digits_combination(void **state) {
    (void)state;
    mock_run_calc_main(
      "(4,2)+(2,4)\n(55,8)+(256,4)+(11,11)\nCOMPOSE 70Lubi3epla50cki90Lub12iePL32AdZ\nPRINT\n",
      "(256,4)+(55,8)+(11,11)\n",
      "ERROR 3 WRONG COUNT\n",
      0
    );
}

/*
* Tests entry point
*/
int main(void) {

    /*
    * Group test
    *   description:
    *        Testing composition via PolyCompose
    *
    */
    const struct CMUnitTest compose_fn_tests[] = {
      cmocka_unit_test(test_compose_fn_poly_0_count_0),
      cmocka_unit_test(test_compose_fn_poly_0_count_1),
      cmocka_unit_test(test_compose_fn_poly_const_count_0),
      cmocka_unit_test(test_compose_fn_poly_const_count_1),
      cmocka_unit_test(test_compose_fn_poly_linear_count_0),
      cmocka_unit_test(test_compose_fn_poly_linear_count_1_const),
      cmocka_unit_test(test_compose_fn_poly_linear_count_1_linear)
    };

    /*
    * Group test
    *   description:
    *        Testing composition via calculator calling COMPOSE
    *        operation
    *
    */
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

    
    // Run tests
    int status = 0;
    status |= cmocka_run_group_tests_name("compose function tests", compose_fn_tests, NULL, NULL);
    status |= cmocka_run_group_tests_name("COMPOSE parsing calculator tests", parser_compose_tests, NULL, NULL);
    return status;

}
