#include "shopping_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef enum
{
    READ_LINE_OK = 0,
    READ_LINE_EOF,
    READ_LINE_OVERLONG,
    READ_LINE_ERROR
} ReadLineStatus;

/* Helper function to safely read a line from stdin, detect overlong lines, and flush remainder */
static ReadLineStatus read_console_line(char *buffer, size_t size)
{
    if (buffer == NULL || size < 2)
    {
        return READ_LINE_ERROR;
    }

    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        return READ_LINE_EOF;
    }

    size_t len = strlen(buffer);
    if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
    {
        buffer[len - 1] = '\0';
        len--;
        if (len > 0 && buffer[len - 1] == '\r')
        {
            buffer[len - 1] = '\0';
        }
        return READ_LINE_OK;
    }

    /* If line ended exactly at EOF */
    if (feof(stdin))
    {
        return READ_LINE_OK;
    }

    /* Line exceeded buffer capacity: flush remainder until newline or EOF */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        /* discard characters of overlong line */
    }

    return READ_LINE_OVERLONG;
}

/* Prompt and read a validated non-empty string, rejecting overlong or blank input */
static bool read_validated_string(const char *prompt, char *buffer, size_t size)
{
    while (1)
    {
        printf("%s", prompt);
        ReadLineStatus status = read_console_line(buffer, size);

        if (status == READ_LINE_EOF)
        {
            return false;
        }

        if (status == READ_LINE_OVERLONG)
        {
            printf("Error: Input is too long. Maximum %zu characters allowed. Please try again.\n", size - 1);
            continue;
        }

        if (status == READ_LINE_ERROR)
        {
            printf("Error: Failed to read input. Please try again.\n");
            continue;
        }

        StatusCode code = validate_item_name(buffer);
        if (code == STATUS_ERR_EMPTY)
        {
            printf("Error: Input cannot be empty. Please try again.\n");
            continue;
        }

        return true;
    }
}

/* Prompt and read a validated integer within a specified range */
static bool read_validated_int(const char *prompt, int min_val, int max_val, int *out_val)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("%s", prompt);
        ReadLineStatus status = read_console_line(buffer, sizeof(buffer));

        if (status == READ_LINE_EOF)
        {
            return false;
        }

        if (status == READ_LINE_OVERLONG)
        {
            printf("Error: Input is too long. Please enter a valid number.\n");
            continue;
        }

        if (status == READ_LINE_ERROR)
        {
            printf("Error: Failed to read input. Please try again.\n");
            continue;
        }

        StatusCode code = parse_int_string(buffer, min_val, max_val, out_val);

        if (code == STATUS_ERR_EMPTY)
        {
            printf("Error: Input cannot be empty. Please enter an integer.\n");
            continue;
        }

        if (code == STATUS_ERR_INVALID_FORMAT)
        {
            printf("Error: Invalid numeric input. Please enter an integer.\n");
            continue;
        }

        if (code == STATUS_ERR_TRAILING_GARBAGE)
        {
            printf("Error: Trailing characters or decimal detected. Please enter only a whole integer.\n");
            continue;
        }

        if (code == STATUS_ERR_OUT_OF_RANGE)
        {
            printf("Error: Value must be between %d and %d. Please try again.\n", min_val, max_val);
            continue;
        }

        return true;
    }
}

/* Prompt and read a validated price string into integer paise */
static bool read_validated_price(const char *prompt, int64_t max_paise, int64_t *out_paise)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("%s", prompt);
        ReadLineStatus status = read_console_line(buffer, sizeof(buffer));

        if (status == READ_LINE_EOF)
        {
            return false;
        }

        if (status == READ_LINE_OVERLONG)
        {
            printf("Error: Input is too long. Please enter a valid price.\n");
            continue;
        }

        if (status == READ_LINE_ERROR)
        {
            printf("Error: Failed to read input. Please try again.\n");
            continue;
        }

        StatusCode code = parse_price_string(buffer, max_paise, out_paise);

        if (code == STATUS_ERR_EMPTY)
        {
            printf("Error: Input cannot be empty. Please enter a price.\n");
            continue;
        }

        if (code == STATUS_ERR_NEGATIVE)
        {
            printf("Error: Price cannot be negative. Please enter a valid price.\n");
            continue;
        }

        if (code == STATUS_ERR_TOO_MANY_DECIMALS)
        {
            printf("Error: Unsupported precision. Prices can have at most two decimal places.\n");
            continue;
        }

        if (code == STATUS_ERR_TRAILING_GARBAGE)
        {
            printf("Error: Trailing characters detected. Please enter only a valid numeric price.\n");
            continue;
        }

        if (code == STATUS_ERR_INVALID_FORMAT)
        {
            printf("Error: Invalid numeric input. Please enter a valid price (e.g., 25.50).\n");
            continue;
        }

        if (code == STATUS_ERR_OUT_OF_RANGE || code == STATUS_ERR_OVERFLOW)
        {
            char max_buf[32];
            format_paise_to_rupees(max_paise, max_buf, sizeof(max_buf));
            printf("Error: Price exceeds maximum allowed limit of %s. Please try again.\n", max_buf);
            continue;
        }

        return true;
    }
}

/* Display the complete shopping bill summary */
static void display_bill(const ShoppingItem *items, int count)
{
    int64_t total_paise = 0;
    char price_buf[32];
    char subtotal_buf[32];
    char total_buf[32];

    printf("\n----- Your Shopping Bill List -----\n\n");
    printf(" SNO | %-16s | %-10s | %-12s | %-12s\n", "Item Name", "Quantity", "Price", "Subtotal");
    printf("-------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        int64_t subtotal_paise = 0;
        StatusCode sub_code = calculate_subtotal(items[i].quantity, items[i].price_paise, &subtotal_paise);
        if (sub_code != STATUS_OK)
        {
            fprintf(stderr, "Error: Failed to calculate subtotal for item %d.\n", i + 1);
            return;
        }

        StatusCode tot_code = accumulate_total(total_paise, subtotal_paise, &total_paise);
        if (tot_code != STATUS_OK)
        {
            fprintf(stderr, "Error: Total accumulation overflow detected.\n");
            return;
        }

        format_paise_to_rupees(items[i].price_paise, price_buf, sizeof(price_buf));
        format_paise_to_rupees(subtotal_paise, subtotal_buf, sizeof(subtotal_buf));

        printf("%4d | %-16s | %10d | %12s | %12s\n",
               i + 1,
               items[i].item,
               items[i].quantity,
               price_buf,
               subtotal_buf);
    }

    format_paise_to_rupees(total_paise, total_buf, sizeof(total_buf));

    printf("-------------------------------------------------------------------\n");
    printf("Your total bill is %s\n", total_buf);
}

int main(void)
{
    printf("-----Welcome to Your Shopping list -----\n");

    int n = 0;
    if (!read_validated_int("Enter the number of items you purchase: \n", 1, MAX_ITEMS, &n))
    {
        fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
        return 1;
    }

    ShoppingItem *shopping = (ShoppingItem *)malloc((size_t)n * sizeof(ShoppingItem));
    if (shopping == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        printf("----- Entering the item %d of %d -----\n", i + 1, n);

        char prompt_name[64];
        snprintf(prompt_name, sizeof(prompt_name), "Enter the itemname %d \n", i + 1);
        if (!read_validated_string(prompt_name, shopping[i].item, sizeof(shopping[i].item)))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }

        if (!read_validated_int("Enter the quantity of item \n", 1, MAX_QUANTITY, &shopping[i].quantity))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }

        if (!read_validated_price("Enter the item price \n", MAX_PRICE_PAISE, &shopping[i].price_paise))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }
    }

    int c = 0;
    if (!read_validated_int("Can You search any item in the list bring or not bring ? Yes(1) / No(0)\n", 0, 1, &c))
    {
        fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
        free(shopping);
        return 1;
    }

    if (c == 1)
    {
        char search_name[MAX_NAME_LEN];
        if (!read_validated_string("Enter the search item name ?\n", search_name, sizeof(search_name)))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }

        printf("----- Result of Search item -----\n");
        int idx = find_item_index(shopping, n, search_name);
        if (idx >= 0)
        {
            printf("Yes %s is in your shopping list\n", search_name);
        }
        else
        {
            printf("No %s is not in your shopping list\n", search_name);
        }
    }

    display_bill(shopping, n);

    free(shopping);
    shopping = NULL;

    return 0;
}
