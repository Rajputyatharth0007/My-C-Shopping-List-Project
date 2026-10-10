#include "shopping_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int g_tests_run = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_tests_run++; \
    if (cond) { \
        g_tests_passed++; \
        printf("  [PASS] %s\n", msg); \
    } else { \
        g_tests_failed++; \
        printf("  [FAIL] %s (Line %d: %s)\n", msg, __LINE__, #cond); \
    } \
} while(0)

/* =========================================================================
 * 1. Case-Insensitive Search Tests
 * ========================================================================= */
static void test_case_insensitive_search(void)
{
    printf("\n--- Running Search Tests ---\n");

    ShoppingItem items[4] = {
        {"Milk", 2, 2550},
        {"Whole Wheat Bread", 1, 4000},
        {"Farm Eggs", 12, 750},
        {"Butter", 1, 5525}
    };
    int count = 4;

    /* Exact match */
    TEST_ASSERT(find_item_index(items, count, "Milk") == 0,
                "Exact match for 'Milk' finds index 0");

    /* Case variants */
    TEST_ASSERT(find_item_index(items, count, "milk") == 0,
                "Lowercase 'milk' matches 'Milk'");
    TEST_ASSERT(find_item_index(items, count, "MILK") == 0,
                "Uppercase 'MILK' matches 'Milk'");
    TEST_ASSERT(find_item_index(items, count, "mIlK") == 0,
                "Mixed case 'mIlK' matches 'Milk'");

    /* Multi-word case variants */
    TEST_ASSERT(find_item_index(items, count, "whole wheat bread") == 1,
                "Lowercase 'whole wheat bread' matches 'Whole Wheat Bread'");
    TEST_ASSERT(find_item_index(items, count, "FARM EGGS") == 2,
                "Uppercase 'FARM EGGS' matches 'Farm Eggs'");

    /* Search for last item */
    TEST_ASSERT(find_item_index(items, count, "butter") == 3,
                "Searching for last item 'butter' finds index 3");

    /* Missing items */
    TEST_ASSERT(find_item_index(items, count, "Cheese") == -1,
                "Missing item 'Cheese' returns -1");
    TEST_ASSERT(find_item_index(items, count, "Milky") == -1,
                "Partial prefix 'Milky' does not match 'Milk'");

    /* Empty and whitespace search inputs */
    TEST_ASSERT(find_item_index(items, count, "") == -1,
                "Empty string search returns -1");
    TEST_ASSERT(find_item_index(items, count, "   ") == -1,
                "Whitespace-only search returns -1");
    TEST_ASSERT(find_item_index(items, count, NULL) == -1,
                "NULL search string returns -1");
    TEST_ASSERT(find_item_index(NULL, count, "Milk") == -1,
                "NULL items array returns -1");
    TEST_ASSERT(find_item_index(items, 0, "Milk") == -1,
                "Count 0 returns -1");
}

/* =========================================================================
 * 2. Input Validation Tests
 * ========================================================================= */
static void test_input_validation(void)
{
    printf("\n--- Running Input Validation Tests ---\n");

    int val = 0;

    /* Valid integers */
    TEST_ASSERT(parse_int_string("1", 1, 100, &val) == STATUS_OK && val == 1,
                "Valid minimum integer '1' parses correctly");
    TEST_ASSERT(parse_int_string("100", 1, 100, &val) == STATUS_OK && val == 100,
                "Valid maximum integer '100' parses correctly");
    TEST_ASSERT(parse_int_string("  42  ", 1, 100, &val) == STATUS_OK && val == 42,
                "Integer with surrounding whitespace parses correctly");

    /* Negative integer where prohibited */
    TEST_ASSERT(parse_int_string("-5", 1, 100, &val) == STATUS_ERR_OUT_OF_RANGE,
                "Negative integer '-5' rejected when range is [1, 100]");

    /* Zero where prohibited */
    TEST_ASSERT(parse_int_string("0", 1, 100, &val) == STATUS_ERR_OUT_OF_RANGE,
                "Zero integer '0' rejected when range is [1, 100]");

    /* Upper bound exceeded */
    TEST_ASSERT(parse_int_string("101", 1, 100, &val) == STATUS_ERR_OUT_OF_RANGE,
                "Integer '101' rejected when max is 100");

    /* Non-numeric text */
    TEST_ASSERT(parse_int_string("abc", 1, 100, &val) == STATUS_ERR_INVALID_FORMAT,
                "Non-numeric text 'abc' rejected with INVALID_FORMAT");

    /* Partial numeric text */
    TEST_ASSERT(parse_int_string("2abc", 1, 100, &val) == STATUS_ERR_TRAILING_GARBAGE,
                "Partial numeric '2abc' rejected with TRAILING_GARBAGE");

    /* Decimal values where integer required */
    TEST_ASSERT(parse_int_string("2.5", 1, 100, &val) == STATUS_ERR_TRAILING_GARBAGE,
                "Decimal '2.5' rejected where integer is required");

    /* Integer overflow and underflow */
    TEST_ASSERT(parse_int_string("99999999999999999999", 1, 100, &val) == STATUS_ERR_OUT_OF_RANGE,
                "Excessively large integer string rejected (ERANGE)");
    TEST_ASSERT(parse_int_string("-99999999999999999999", 1, 100, &val) == STATUS_ERR_OUT_OF_RANGE,
                "Excessively negative integer string rejected (ERANGE)");

    /* Empty and whitespace-only input */
    TEST_ASSERT(parse_int_string("", 1, 100, &val) == STATUS_ERR_EMPTY,
                "Empty string for integer rejected with STATUS_ERR_EMPTY");
    TEST_ASSERT(parse_int_string("   ", 1, 100, &val) == STATUS_ERR_EMPTY,
                "Whitespace-only string for integer rejected with STATUS_ERR_EMPTY");

    /* Item name validation */
    TEST_ASSERT(validate_item_name("Bread") == STATUS_OK,
                "Valid item name 'Bread' passes validation");
    TEST_ASSERT(validate_item_name("") == STATUS_ERR_EMPTY,
                "Empty item name rejected");
    TEST_ASSERT(validate_item_name("   ") == STATUS_ERR_EMPTY,
                "Whitespace-only item name rejected");
    TEST_ASSERT(validate_item_name(NULL) == STATUS_ERR_NULL_PTR,
                "NULL item name rejected");
}

/* =========================================================================
 * 3. Monetary Calculation & Precision Tests
 * ========================================================================= */
static void test_monetary_calculations(void)
{
    printf("\n--- Running Monetary Calculation Tests ---\n");

    int64_t paise = 0;

    /* Parse prices in rupees to integer paise */
    TEST_ASSERT(parse_price_string("25.50", MAX_PRICE_PAISE, &paise) == STATUS_OK && paise == 2550,
                "Price '25.50' parses to 2550 paise");
    TEST_ASSERT(parse_price_string("25.5", MAX_PRICE_PAISE, &paise) == STATUS_OK && paise == 2550,
                "Single decimal '25.5' parses to 2550 paise");
    TEST_ASSERT(parse_price_string("40.00", MAX_PRICE_PAISE, &paise) == STATUS_OK && paise == 4000,
                "Price '40.00' parses to 4000 paise");
    TEST_ASSERT(parse_price_string("40", MAX_PRICE_PAISE, &paise) == STATUS_OK && paise == 4000,
                "Whole number price '40' parses to 4000 paise");
    TEST_ASSERT(parse_price_string("0.05", MAX_PRICE_PAISE, &paise) == STATUS_OK && paise == 5,
                "Price '0.05' parses to 5 paise");

    /* Multiplication: quantity * unit price */
    int64_t subtotal = 0;
    TEST_ASSERT(calculate_subtotal(2, 2550, &subtotal) == STATUS_OK && subtotal == 5100,
                "2 x 2550 paise equals 5100 paise (₹25.50 x 2 = ₹51.00)");
    TEST_ASSERT(calculate_subtotal(3, 4000, &subtotal) == STATUS_OK && subtotal == 12000,
                "3 x 4000 paise equals 12000 paise (₹40.00 x 3 = ₹120.00)");
    TEST_ASSERT(calculate_subtotal(1, 5525, &subtotal) == STATUS_OK && subtotal == 5525,
                "1 x 5525 paise equals 5525 paise (₹55.25 x 1 = ₹55.25)");
    TEST_ASSERT(calculate_subtotal(12, 750, &subtotal) == STATUS_OK && subtotal == 9000,
                "12 x 750 paise equals 9000 paise (₹7.50 x 12 = ₹90.00)");

    /* Zero-price items */
    TEST_ASSERT(parse_price_string("0.00", MAX_PRICE_PAISE, &paise) == STATUS_OK && paise == 0,
                "Zero price '0.00' parses to 0 paise");
    TEST_ASSERT(calculate_subtotal(5, 0, &subtotal) == STATUS_OK && subtotal == 0,
                "Quantity 5 at ₹0.00 results in ₹0.00 subtotal");

    /* Accumulation of subtotals */
    int64_t total = 0;
    TEST_ASSERT(accumulate_total(total, 12000, &total) == STATUS_OK && total == 12000,
                "Accumulate subtotal 1: ₹120.00");
    TEST_ASSERT(accumulate_total(total, 5525, &total) == STATUS_OK && total == 17525,
                "Accumulate subtotal 2: ₹120.00 + ₹55.25 = ₹175.25");
    TEST_ASSERT(accumulate_total(total, 9000, &total) == STATUS_OK && total == 26525,
                "Accumulate subtotal 3: ₹175.25 + ₹90.00 = ₹265.25");

    /* Formatting paise to rupees */
    char buf[32];
    TEST_ASSERT(format_paise_to_rupees(26525, buf, sizeof(buf)) && strcmp(buf, "265.25") == 0,
                "Format 26525 paise outputs '265.25'");
    TEST_ASSERT(format_paise_to_rupees(5100, buf, sizeof(buf)) && strcmp(buf, "51.00") == 0,
                "Format 5100 paise outputs '51.00'");
    TEST_ASSERT(format_paise_to_rupees(5, buf, sizeof(buf)) && strcmp(buf, "0.05") == 0,
                "Format 5 paise outputs '0.05'");
    TEST_ASSERT(format_paise_to_rupees(0, buf, sizeof(buf)) && strcmp(buf, "0.00") == 0,
                "Format 0 paise outputs '0.00'");

    /* Rejection of negative price */
    TEST_ASSERT(parse_price_string("-10.00", MAX_PRICE_PAISE, &paise) == STATUS_ERR_NEGATIVE,
                "Negative price '-10.00' rejected with STATUS_ERR_NEGATIVE");

    /* Rejection of > 2 decimal places */
    TEST_ASSERT(parse_price_string("25.555", MAX_PRICE_PAISE, &paise) == STATUS_ERR_TOO_MANY_DECIMALS,
                "Price with 3 decimal places '25.555' rejected");
    TEST_ASSERT(parse_price_string("10.1234", MAX_PRICE_PAISE, &paise) == STATUS_ERR_TOO_MANY_DECIMALS,
                "Price with 4 decimal places '10.1234' rejected");

    /* Rejection of invalid non-finite strings */
    TEST_ASSERT(parse_price_string("nan", MAX_PRICE_PAISE, &paise) == STATUS_ERR_INVALID_FORMAT,
                "Literal 'nan' price rejected");
    TEST_ASSERT(parse_price_string("inf", MAX_PRICE_PAISE, &paise) == STATUS_ERR_INVALID_FORMAT,
                "Literal 'inf' price rejected");
    TEST_ASSERT(parse_price_string("infinity", MAX_PRICE_PAISE, &paise) == STATUS_ERR_INVALID_FORMAT,
                "Literal 'infinity' price rejected");

    /* Multiplication overflow protection */
    int64_t overflow_sub = 0;
    TEST_ASSERT(calculate_subtotal(1000000, INT64_MAX / 2, &overflow_sub) == STATUS_ERR_OVERFLOW,
                "Subtotal calculation overflow correctly detected and rejected");

    /* Total accumulation overflow protection */
    int64_t overflow_tot = 0;
    TEST_ASSERT(accumulate_total(INT64_MAX - 100, 200, &overflow_tot) == STATUS_ERR_OVERFLOW,
                "Total accumulation overflow correctly detected and rejected");
}

/* =========================================================================
 * 4. Shopping List Integration Tests
 * ========================================================================= */
static void test_shopping_list_integration(void)
{
    printf("\n--- Running Shopping List Integration Tests ---\n");

    /* Case A: One item */
    ShoppingItem single_item[1] = {
        {"Milk", 2, 2550}
    };
    int64_t sub1 = 0;
    int64_t tot1 = 0;
    TEST_ASSERT(calculate_subtotal(single_item[0].quantity, single_item[0].price_paise, &sub1) == STATUS_OK && sub1 == 5100,
                "Single item subtotal is 5100 paise");
    TEST_ASSERT(accumulate_total(tot1, sub1, &tot1) == STATUS_OK && tot1 == 5100,
                "Single item grand total is 5100 paise (₹51.00)");
    TEST_ASSERT(find_item_index(single_item, 1, "MILK") == 0,
                "Single item list finds 'MILK'");

    /* Case B: Multiple items (Bread, Butter, Eggs) */
    ShoppingItem basket[3] = {
        {"Bread", 3, 4000},
        {"Butter", 1, 5525},
        {"Eggs", 12, 750}
    };
    int64_t grand_total = 0;
    for (int i = 0; i < 3; i++)
    {
        int64_t sub = 0;
        calculate_subtotal(basket[i].quantity, basket[i].price_paise, &sub);
        accumulate_total(grand_total, sub, &grand_total);
    }
    TEST_ASSERT(grand_total == 26525,
                "Basket of 3 items sums to exactly 26525 paise (₹265.25)");

    /* Case C: Maximum supported item count */
    ShoppingItem max_items[MAX_ITEMS];
    int64_t max_total = 0;
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        snprintf(max_items[i].item, sizeof(max_items[i].item), "Item_%d", i + 1);
        max_items[i].quantity = 1;
        max_items[i].price_paise = 1000; /* ₹10.00 */
        int64_t sub = 0;
        calculate_subtotal(max_items[i].quantity, max_items[i].price_paise, &sub);
        accumulate_total(max_total, sub, &max_total);
    }
    TEST_ASSERT(max_total == 100000,
                "100 items at ₹10.00 each sum to exactly ₹1000.00 (100000 paise)");
    TEST_ASSERT(find_item_index(max_items, MAX_ITEMS, "item_100") == 99,
                "Search finds last item 'item_100' in max-capacity list");

    /* Case D: Invalid item counts */
    int parsed_count = 0;
    TEST_ASSERT(parse_int_string("0", 1, MAX_ITEMS, &parsed_count) == STATUS_ERR_OUT_OF_RANGE,
                "Item count 0 rejected");
    TEST_ASSERT(parse_int_string("-1", 1, MAX_ITEMS, &parsed_count) == STATUS_ERR_OUT_OF_RANGE,
                "Item count -1 rejected");
    TEST_ASSERT(parse_int_string("101", 1, MAX_ITEMS, &parsed_count) == STATUS_ERR_OUT_OF_RANGE,
                "Item count 101 rejected (exceeds MAX_ITEMS=100)");
}

int main(void)
{
    printf("=========================================\n");
    printf("  RUNNING SHOPPING LIST AUTOMATED TESTS  \n");
    printf("=========================================\n");

    test_case_insensitive_search();
    test_input_validation();
    test_monetary_calculations();
    test_shopping_list_integration();

    printf("\n=========================================\n");
    printf("TEST SUMMARY: %d Run, %d Passed, %d Failed\n",
           g_tests_run, g_tests_passed, g_tests_failed);
    printf("=========================================\n");

    return (g_tests_failed == 0) ? 0 : 1;
}
