#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#define MAX_NAME_LEN 50
#define MAX_ITEMS 100
#define BUFFER_SIZE 128

typedef struct
{
    char item[MAX_NAME_LEN];
    int quantity;
    float price;
} ShoppingItem;

/* Helper function to safely read a line from stdin and strip trailing newline */
static int read_line(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        return 0; /* EOF or read error */
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
    }
    else if (len == size - 1)
    {
        /* Input was longer than buffer, discard remainder of the line */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            /* discard characters until end of line */
        }
    }

    return 1;
}

/* Helper function to safely read a non-empty string */
static int read_string(const char *prompt, char *buffer, size_t size)
{
    while (1)
    {
        printf("%s", prompt);
        if (!read_line(buffer, size))
        {
            return 0; /* EOF */
        }

        /* Check for empty or whitespace-only input */
        size_t i = 0;
        while (buffer[i] != '\0' && isspace((unsigned char)buffer[i]))
        {
            i++;
        }

        if (buffer[i] == '\0')
        {
            printf("Error: Input cannot be empty. Please try again.\n");
            continue;
        }

        return 1;
    }
}

/* Helper function to read and validate an integer within a range */
static int read_int(const char *prompt, int min_val, int max_val, int *out_val)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("%s", prompt);
        if (!read_line(buffer, sizeof(buffer)))
        {
            return 0; /* EOF */
        }

        char *endptr = NULL;
        errno = 0;
        long val = strtol(buffer, &endptr, 10);

        if (endptr == buffer)
        {
            printf("Error: Invalid numeric input. Please enter an integer.\n");
            continue;
        }

        /* Check for extra non-whitespace trailing characters */
        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        if (*endptr != '\0')
        {
            printf("Error: Trailing characters detected. Please enter only an integer.\n");
            continue;
        }

        if (errno == ERANGE || val < min_val || val > max_val)
        {
            printf("Error: Value must be between %d and %d. Please try again.\n", min_val, max_val);
            continue;
        }

        *out_val = (int)val;
        return 1;
    }
}

/* Helper function to read and validate a float within a range */
static int read_float(const char *prompt, float min_val, float max_val, float *out_val)
{
    char buffer[BUFFER_SIZE];

    while (1)
    {
        printf("%s", prompt);
        if (!read_line(buffer, sizeof(buffer)))
        {
            return 0; /* EOF */
        }

        char *endptr = NULL;
        errno = 0;
        float val = strtof(buffer, &endptr);

        if (endptr == buffer)
        {
            printf("Error: Invalid numeric input. Please enter a valid price.\n");
            continue;
        }

        while (*endptr != '\0' && isspace((unsigned char)*endptr))
        {
            endptr++;
        }

        if (*endptr != '\0')
        {
            printf("Error: Trailing characters detected. Please enter only a valid price.\n");
            continue;
        }

        if (errno == ERANGE || isnan(val) || isinf(val) || val < min_val || val > max_val)
        {
            printf("Error: Price must be between %.2f and %.2f. Please try again.\n", min_val, max_val);
            continue;
        }

        *out_val = val;
        return 1;
    }
}

/* Helper function to search for an item in the shopping list */
static void search_item(const ShoppingItem *items, int count, const char *search_name)
{
    int found = 0;
    printf("----- Result of Search item -----\n");

    for (int i = 0; i < count; i++)
    {
        if (strcmp(search_name, items[i].item) == 0)
        {
            printf("Yes %s is in your shopping list\n", search_name);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("No %s is not in your shopping list\n", search_name);
    }
}

/* Helper function to calculate and display the bill */
static void display_bill(const ShoppingItem *items, int count)
{
    float totalbill = 0.0f;

    printf("\n----- Your Shopping Bill List -----\n\n");
    printf(" SNO | %-16s | %-10s | %-10s | %-10s\n", "Item Name", "Quantity", "Price", "Subtotal");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        float subtotal = (float)items[i].quantity * items[i].price;
        printf("%4d | %-16s | %10d | %10.2f | %10.2f\n",
               i + 1,
               items[i].item,
               items[i].quantity,
               items[i].price,
               subtotal);
        totalbill += subtotal;
    }

    printf("---------------------------------------------------------------\n");
    printf("Your total bill is %.2f\n", totalbill);
}

int main(void)
{
    printf("-----Welcome to Your Shopping list -----\n");

    int n = 0;
    if (!read_int("Enter the number of items you purchase: \n", 1, MAX_ITEMS, &n))
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
        if (!read_string(prompt_name, shopping[i].item, sizeof(shopping[i].item)))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }

        if (!read_int("Enter the quantity of item \n", 1, 1000000, &shopping[i].quantity))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }

        if (!read_float("Enter the item price \n", 0.0f, 10000000.0f, &shopping[i].price))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }
    }

    int c = 0;
    if (!read_int("Can You search any item in the list bring or not bring ? Yes(1) / No(0)\n", 0, 1, &c))
    {
        fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
        free(shopping);
        return 1;
    }

    if (c == 1)
    {
        char search_name[MAX_NAME_LEN];
        if (!read_string("Enter the search item name ?\n", search_name, sizeof(search_name)))
        {
            fprintf(stderr, "Error: Unexpected end of input. Exiting.\n");
            free(shopping);
            return 1;
        }

        search_item(shopping, n, search_name);
    }

    display_bill(shopping, n);

    free(shopping);
    shopping = NULL;

    return 0;
}
