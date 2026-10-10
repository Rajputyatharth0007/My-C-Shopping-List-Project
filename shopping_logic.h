#ifndef SHOPPING_LOGIC_H
#define SHOPPING_LOGIC_H

#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_NAME_LEN 50
#define MAX_ITEMS 100
#define MAX_QUANTITY 1000000
#define MAX_PRICE_PAISE (10000000LL * 100LL) /* Up to ₹10,000,000.00 (10^9 paise) */
#define BUFFER_SIZE 128

typedef struct
{
    char item[MAX_NAME_LEN];
    int quantity;
    int64_t price_paise; /* unit price in paise (1 rupee = 100 paise) */
} ShoppingItem;

/* Status codes for validation and arithmetic operations */
typedef enum
{
    STATUS_OK = 0,
    STATUS_ERR_EMPTY,
    STATUS_ERR_INVALID_FORMAT,
    STATUS_ERR_OUT_OF_RANGE,
    STATUS_ERR_TRAILING_GARBAGE,
    STATUS_ERR_TOO_MANY_DECIMALS,
    STATUS_ERR_NEGATIVE,
    STATUS_ERR_OVERFLOW,
    STATUS_ERR_NULL_PTR
} StatusCode;

/* Case-insensitive string comparison using standard tolower((unsigned char)c) */
int case_insensitive_compare(const char *s1, const char *s2);

/* Search for an item by name in a case-insensitive manner. Returns index or -1 if not found. */
int find_item_index(const ShoppingItem *items, int count, const char *search_name);

/* Parse an integer from a string with strict boundary and format checks */
StatusCode parse_int_string(const char *str, int min_val, int max_val, int *out_val);

/* Parse a monetary price string (e.g. "25", "25.5", "25.50") into integer paise */
StatusCode parse_price_string(const char *str, int64_t max_paise, int64_t *out_paise);

/* Multiply quantity and unit price in paise with overflow protection */
StatusCode calculate_subtotal(int quantity, int64_t unit_price_paise, int64_t *out_subtotal_paise);

/* Accumulate subtotal into grand total with overflow protection */
StatusCode accumulate_total(int64_t current_total_paise, int64_t subtotal_paise, int64_t *out_new_total_paise);

/* Format paise value into standard rupees string with 2 decimal places (e.g. 2550 -> "25.50") */
bool format_paise_to_rupees(int64_t paise, char *buffer, size_t size);

/* Validate non-empty item name */
StatusCode validate_item_name(const char *name);

#endif /* SHOPPING_LOGIC_H */
