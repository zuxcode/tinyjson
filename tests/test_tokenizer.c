#include "tinyjson/tokenizer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ---------------------------------------------------------
 * PUNCTUATION
 * --------------------------------------------------------- */

static void test_punctuation(void)
{
    const char *json = "{}[]:,";
    const char *current = json;

    Token token;

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_LEFT_BRACE);
    assert(token.length == 1);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_RIGHT_BRACE);
    assert(token.length == 1);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_LEFT_BRACKET);
    assert(token.length == 1);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_RIGHT_BRACKET);
    assert(token.length == 1);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_COLON);
    assert(token.length == 1);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_COMMA);
    assert(token.length == 1);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_EOF);
    assert(token.length == 0);
}

/* ---------------------------------------------------------
 * STRINGS
 * --------------------------------------------------------- */

static void test_string(void)
{
    const char *json = "\"hello\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
    assert(token.length == 5);
    assert(strncmp(token.start, "hello", token.length) == 0);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_EOF);
}

static void test_empty_string(void)
{
    const char *json = "\"\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
    assert(token.length == 0);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_EOF);
}

static void test_string_with_escape(void)
{
    const char *json = "\"hello \\\"world\\\"\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);

    /*
     * The tokenizer keeps the escaped quotes
     * as part of the token.
     */
    assert(strncmp(token.start,
                   "hello \\\"world\\\"",
                   token.length) == 0);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_EOF);
}

static void test_valid_escape_sequences(void)
{
    const char *json =
        "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"";

    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
}

static void test_invalid_escape_sequence(void)
{
    const char *json = "\"hello \\q world\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_escape_q(void)
{
    const char *json = "\"hello \\q\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_escape_x(void)
{
    const char *json = "\"hello \\x\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_escape_v(void)
{
    const char *json = "\"hello \\v\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_unterminated_escape(void)
{
    const char *json = "\"hello\\";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_unterminated_string(void)
{
    const char *json = "\"hello";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_control_character(void)
{
    const char json[] = "\"hello\x01world\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_raw_newline_in_string(void)
{
    const char json[] = "\"hello\nworld\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

/* ---------------------------------------------------------
 * UNICODE ESCAPES
 * --------------------------------------------------------- */

static void test_valid_unicode_escape(void)
{
    const char *json = "\"\\u0041\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
    assert(token.length == 6);
}

static void test_valid_unicode_escape_uppercase(void)
{
    const char *json = "\"\\uABCD\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
}

static void test_valid_unicode_escape_lowercase(void)
{
    const char *json = "\"\\uabcd\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
}

static void test_valid_unicode_escape_boundaries(void)
{
    const char *json = "\"\\u0000\\uFFFF\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
}

static void test_unicode_escape_followed_by_character(void)
{
    /*
     * This is valid:
     *
     *     \u1234
     *     5
     *
     * The '5' is a normal character after the
     * four hexadecimal digits.
     */
    const char *json = "\"\\u12345\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_STRING);
}

static void test_invalid_unicode_escape_too_short(void)
{
    const char *json = "\"\\u123\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_unicode_escape_no_digits(void)
{
    const char *json = "\"\\u\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_unicode_escape_non_hex(void)
{
    const char *json = "\"\\u12G4\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_unicode_escape_negative(void)
{
    const char *json = "\"\\u-234\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_unicode_escape_space(void)
{
    const char *json = "\"\\u 234\"";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

/* ---------------------------------------------------------
 * NUMBERS
 * --------------------------------------------------------- */

static void test_zero(void)
{
    const char *json = "0";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 1);
    assert(strncmp(token.start, "0", token.length) == 0);
}

static void test_negative_zero(void)
{
    const char *json = "-0";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 2);
    assert(strncmp(token.start, "-0", token.length) == 0);
}

static void test_integer(void)
{
    const char *json = "42";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 2);
    assert(strncmp(token.start, "42", token.length) == 0);
}

static void test_negative_number(void)
{
    const char *json = "-42";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 3);
    assert(strncmp(token.start, "-42", token.length) == 0);
}

static void test_large_integer(void)
{
    const char *json = "123456789";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 9);
}

static void test_decimal(void)
{
    const char *json = "28.5";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 4);
    assert(strncmp(token.start, "28.5", token.length) == 0);
}

static void test_negative_decimal(void)
{
    const char *json = "-0.5";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 4);
}

static void test_integer_with_decimal_zero(void)
{
    const char *json = "1.0";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 3);
}

static void test_exponent(void)
{
    const char *json = "-2.5e-3";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 7);
    assert(strncmp(token.start, "-2.5e-3", token.length) == 0);
}

static void test_positive_exponent(void)
{
    const char *json = "1e+10";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 5);
}

static void test_uppercase_exponent(void)
{
    const char *json = "1E10";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 4);
}

static void test_negative_exponent(void)
{
    const char *json = "1E-10";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 5);
}

/* ---------------------------------------------------------
 * INVALID NUMBERS
 * --------------------------------------------------------- */

static void test_invalid_exponent(void)
{
    const char *json = "1e";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_decimal(void)
{
    const char *json = "1.";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_negative_decimal(void)
{
    const char *json = "-1.";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_exponent_sign(void)
{
    const char *json = "1e-";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_exponent_plus(void)
{
    const char *json = "1e+";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_uppercase_exponent(void)
{
    const char *json = "1E";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_leading_zero(void)
{
    const char *json = "01";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_negative_leading_zero(void)
{
    const char *json = "-01";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_minus(void)
{
    const char *json = "-";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_plus(void)
{
    const char *json = "+1";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_decimal_start(void)
{
    const char *json = ".5";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_negative_decimal_start(void)
{
    const char *json = "-.5";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_number_followed_by_letter(void)
{
    const char *json = "1abc";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_number_followed_by_dot(void)
{
    const char *json = "1.2.3";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

/* ---------------------------------------------------------
 * NUMBER BOUNDARIES
 * --------------------------------------------------------- */

static void test_number_before_comma(void)
{
    const char *json = "42,";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 2);

    token = tokenizer_next(&current);

    assert(token.type == TOKEN_COMMA);
}

static void test_number_before_right_bracket(void)
{
    const char *json = "42]";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);

    token = tokenizer_next(&current);

    assert(token.type == TOKEN_RIGHT_BRACKET);
}

static void test_number_before_right_brace(void)
{
    const char *json = "42}";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);

    token = tokenizer_next(&current);

    assert(token.type == TOKEN_RIGHT_BRACE);
}

/* ---------------------------------------------------------
 * LITERALS
 * --------------------------------------------------------- */

static void test_true(void)
{
    const char *json = "true";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_TRUE);
    assert(token.length == 4);
    assert(strncmp(token.start, "true", token.length) == 0);

    token = tokenizer_next(&current);

    assert(token.type == TOKEN_EOF);
}

static void test_false(void)
{
    const char *json = "false";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_FALSE);
    assert(token.length == 5);
    assert(strncmp(token.start, "false", token.length) == 0);
}

static void test_null(void)
{
    const char *json = "null";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NULL);
    assert(token.length == 4);
    assert(strncmp(token.start, "null", token.length) == 0);
}

/* ---------------------------------------------------------
 * INVALID LITERALS
 * --------------------------------------------------------- */

static void test_invalid_true(void)
{
    const char *json = "tru";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_false(void)
{
    const char *json = "fals";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_null(void)
{
    const char *json = "nul";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_true_boundary(void)
{
    const char *json = "trueXYZ";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_null_boundary(void)
{
    const char *json = "nullabc";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_false_boundary(void)
{
    const char *json = "falseXYZ";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_true_case(void)
{
    const char *json = "tRue";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_false_case(void)
{
    const char *json = "False";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

static void test_invalid_null_case(void)
{
    const char *json = "Null";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
}

/* ---------------------------------------------------------
 * WHITESPACE
 * --------------------------------------------------------- */

static void test_whitespace(void)
{
    const char *json = "   \n\t  42  \r\n";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_NUMBER);
    assert(token.length == 2);
    assert(strncmp(token.start, "42", token.length) == 0);

    token = tokenizer_next(&current);

    assert(token.type == TOKEN_EOF);
}

static void test_whitespace_between_tokens(void)
{
    const char *json = "true false null";
    const char *current = json;

    Token token = tokenizer_next(&current);
    assert(token.type == TOKEN_TRUE);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_FALSE);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_NULL);

    token = tokenizer_next(&current);
    assert(token.type == TOKEN_EOF);
}

/* ---------------------------------------------------------
 * EOF / EMPTY INPUT
 * --------------------------------------------------------- */

static void test_empty_input(void)
{
    const char *json = "";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_EOF);
    assert(token.length == 0);
}

/* ---------------------------------------------------------
 * INVALID CHARACTERS
 * --------------------------------------------------------- */

static void test_invalid_character(void)
{
    const char *json = "@";
    const char *current = json;

    Token token = tokenizer_next(&current);

    assert(token.type == TOKEN_ERROR);
    assert(token.length == 1);
}

/* ---------------------------------------------------------
 * MAIN
 * --------------------------------------------------------- */

int main(void)
{
    /* Punctuation */
    test_punctuation();

    /* Strings */
    test_string();
    test_empty_string();
    test_string_with_escape();
    test_valid_escape_sequences();
    test_unterminated_string();
    test_unterminated_escape();
    test_invalid_escape_sequence();
    test_invalid_escape_q();
    test_invalid_escape_x();
    test_invalid_escape_v();
    test_invalid_control_character();
    test_invalid_raw_newline_in_string();

    /* Unicode */
    test_valid_unicode_escape();
    test_valid_unicode_escape_uppercase();
    test_valid_unicode_escape_lowercase();
    test_valid_unicode_escape_boundaries();
    test_unicode_escape_followed_by_character();
    test_invalid_unicode_escape_too_short();
    test_invalid_unicode_escape_no_digits();
    test_invalid_unicode_escape_non_hex();
    test_invalid_unicode_escape_negative();
    test_invalid_unicode_escape_space();

    /* Valid numbers */
    test_zero();
    test_negative_zero();
    test_integer();
    test_negative_number();
    test_large_integer();
    test_decimal();
    test_negative_decimal();
    test_integer_with_decimal_zero();
    test_exponent();
    test_positive_exponent();
    test_uppercase_exponent();
    test_negative_exponent();

    /* Invalid numbers */
    test_invalid_exponent();
    test_invalid_decimal();
    test_invalid_negative_decimal();
    test_invalid_exponent_sign();
    test_invalid_exponent_plus();
    test_invalid_uppercase_exponent();
    test_invalid_leading_zero();
    test_invalid_negative_leading_zero();
    test_invalid_minus();
    test_invalid_plus();
    test_invalid_decimal_start();
    test_invalid_negative_decimal_start();
    test_invalid_number_followed_by_letter();
    test_invalid_number_followed_by_dot();

    /* Number boundaries */
    test_number_before_comma();
    test_number_before_right_bracket();
    test_number_before_right_brace();

    /* Valid literals */
    test_true();
    test_false();
    test_null();

    /* Invalid literals */
    test_invalid_true();
    test_invalid_false();
    test_invalid_null();
    test_invalid_true_boundary();
    test_invalid_false_boundary();
    test_invalid_null_boundary();
    test_invalid_true_case();
    test_invalid_false_case();
    test_invalid_null_case();

    /* Whitespace */
    test_whitespace();
    test_whitespace_between_tokens();

    /* EOF */
    test_empty_input();

    /* Invalid characters */
    test_invalid_character();

    printf("All tokenizer tests passed!\n");

    return 0;
}
