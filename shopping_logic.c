#include "shopping_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

int case_insensitive_compare(const char *s1, const char *s2)
{
    if (s1 == NULL || s2 == NULL)
    {
        if (s1 == s2)
        {
            return 0;
        }
        return (s1 != NULL) ? 1 : -1;
    }

    while (*s1 != '\0' && *s2 != '\0')
    {
        unsigned char c1 = (unsigned char)*s1;
        unsigned char c2 = (unsigned char)*s2;
        int diff = tolower(c1) - tolower(c2);
        if (diff != 0)
        {
            return diff;
        }
        s1++;
        s2++;
    }

    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int find_item_index(const ShoppingItem *items, int count, const char *search_name)
{
    if (items == NULL || search_name == NULL || count <= 0)
    {
        return -1;
    }

    if (validate_item_name(search_name) != STATUS_OK)
    {
        return -1;
    }

    for (int i = 0; i < count; i++)
    {
        if (case_insensitive_compare(search_name, items[i].item) == 0)
        {
            return i;
        }
    }

    return -1;
}

StatusCode validate_item_name(const char *name)
{
    if (name == NULL)
    {
        return STATUS_ERR_NULL_PTR;
    }

    while (*name != '\0' && isspace((unsigned char)*name))
    {
        name++;
    }

    if (*name == '\0')
    {
        return STATUS_ERR_EMPTY;
    }

    return STATUS_OK;
}

StatusCode parse_int_string(const char *str, int min_val, int max_val, int *out_val)
{
    if (str == NULL || out_val == NULL)
    {
        return STATUS_ERR_NULL_PTR;
    }

    while (*str != '\0' && isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str == '\0')
    {
        return STATUS_ERR_EMPTY;
    }

    char *endptr = NULL;
    errno = 0;
    long val = strtol(str, &endptr, 10);

    if (endptr == str)
    {
        return STATUS_ERR_INVALID_FORMAT;
    }

    /* Reject if a decimal point follows (e.g. "2.5") */
    if (*endptr == '.')
    {
        return STATUS_ERR_TRAILING_GARBAGE;
    }

    /* Check for trailing non-whitespace characters (e.g. "2abc") */
    while (*endptr != '\0' && isspace((unsigned char)*endptr))
    {
        endptr++;
    }

    if (*endptr != '\0')
    {
        return STATUS_ERR_TRAILING_GARBAGE;
    }

    if (errno == ERANGE || val < min_val || val > max_val)
    {
        return STATUS_ERR_OUT_OF_RANGE;
    }

    *out_val = (int)val;
    return STATUS_OK;
}

StatusCode parse_price_string(const char *str, int64_t max_paise, int64_t *out_paise)
{
    if (str == NULL || out_paise == NULL)
    {
        return STATUS_ERR_NULL_PTR;
    }

    while (*str != '\0' && isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str == '\0')
    {
        return STATUS_ERR_EMPTY;
    }

    if (*str == '-')
    {
        return STATUS_ERR_NEGATIVE;
    }

    if (*str == '+')
    {
        str++;
    }

    if (*str == '\0')
    {
        return STATUS_ERR_INVALID_FORMAT;
    }

    /* Reject non-finite keyword literals like "nan", "inf", "infinity" */
    if (case_insensitive_compare(str, "nan") == 0 ||
        case_insensitive_compare(str, "inf") == 0 ||
        case_insensitive_compare(str, "infinity") == 0)
    {
        return STATUS_ERR_INVALID_FORMAT;
    }

    int64_t rupees = 0;
    bool has_rupee_digits = false;

    while (*str != '\0' && isdigit((unsigned char)*str))
    {
        has_rupee_digits = true;
        int digit = *str - '0';
        if (rupees > (INT64_MAX - digit) / 10)
        {
            return STATUS_ERR_OVERFLOW;
        }
        rupees = rupees * 10 + digit;
        str++;
    }

    int64_t paise = 0;
    if (*str == '.')
    {
        str++; /* Skip '.' */

        int decimal_digits = 0;
        int frac[2] = {0, 0};

        while (*str != '\0' && isdigit((unsigned char)*str))
        {
            if (decimal_digits < 2)
            {
                frac[decimal_digits] = *str - '0';
            }
            decimal_digits++;
            str++;
        }

        if (decimal_digits == 0)
        {
            /* Decimal point with no following digits, e.g. "25." */
            return STATUS_ERR_INVALID_FORMAT;
        }

        if (decimal_digits > 2)
        {
            /* Disallow more than two decimal places */
            return STATUS_ERR_TOO_MANY_DECIMALS;
        }

        if (decimal_digits == 1)
        {
            paise = (int64_t)frac[0] * 10;
        }
        else if (decimal_digits == 2)
        {
            paise = (int64_t)frac[0] * 10 + frac[1];
        }
    }
    else
    {
        if (!has_rupee_digits)
        {
            return STATUS_ERR_INVALID_FORMAT;
        }
    }

    /* Check for trailing non-whitespace characters */
    while (*str != '\0' && isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str != '\0')
    {
        return STATUS_ERR_TRAILING_GARBAGE;
    }

    /* Multiply rupees by 100 with overflow check */
    if (rupees > (INT64_MAX - paise) / 100)
    {
        return STATUS_ERR_OVERFLOW;
    }

    int64_t total_paise = rupees * 100 + paise;

    if (total_paise < 0)
    {
        return STATUS_ERR_NEGATIVE;
    }

    if (max_paise >= 0 && total_paise > max_paise)
    {
        return STATUS_ERR_OUT_OF_RANGE;
    }

    *out_paise = total_paise;
    return STATUS_OK;
}

StatusCode calculate_subtotal(int quantity, int64_t unit_price_paise, int64_t *out_subtotal_paise)
{
    if (out_subtotal_paise == NULL)
    {
        return STATUS_ERR_NULL_PTR;
    }

    if (quantity <= 0)
    {
        return STATUS_ERR_OUT_OF_RANGE;
    }

    if (unit_price_paise < 0)
    {
        return STATUS_ERR_NEGATIVE;
    }

    if (unit_price_paise > 0 && (int64_t)quantity > INT64_MAX / unit_price_paise)
    {
        return STATUS_ERR_OVERFLOW;
    }

    *out_subtotal_paise = (int64_t)quantity * unit_price_paise;
    return STATUS_OK;
}

StatusCode accumulate_total(int64_t current_total_paise, int64_t subtotal_paise, int64_t *out_new_total_paise)
{
    if (out_new_total_paise == NULL)
    {
        return STATUS_ERR_NULL_PTR;
    }

    if (current_total_paise < 0 || subtotal_paise < 0)
    {
        return STATUS_ERR_NEGATIVE;
    }

    if (INT64_MAX - current_total_paise < subtotal_paise)
    {
        return STATUS_ERR_OVERFLOW;
    }

    *out_new_total_paise = current_total_paise + subtotal_paise;
    return STATUS_OK;
}

bool format_paise_to_rupees(int64_t paise, char *buffer, size_t size)
{
    if (buffer == NULL || size < 8 || paise < 0)
    {
        return false;
    }

    int64_t rupees = paise / 100;
    int rem_paise = (int)(paise % 100);

    /* Format rupees into string safely without non-portable printf specifiers */
    char rupee_str[32];
    if (rupees == 0)
    {
        rupee_str[0] = '0';
        rupee_str[1] = '\0';
    }
    else
    {
        char temp[32];
        int pos = 0;
        int64_t temp_r = rupees;
        while (temp_r > 0 && pos < 31)
        {
            temp[pos++] = (char)('0' + (temp_r % 10));
            temp_r /= 10;
        }
        int out_idx = 0;
        while (pos > 0 && out_idx < 31)
        {
            rupee_str[out_idx++] = temp[--pos];
        }
        rupee_str[out_idx] = '\0';
    }

    int written = snprintf(buffer, size, "%s.%02d", rupee_str, rem_paise);
    return (written > 0 && (size_t)written < size);
}
