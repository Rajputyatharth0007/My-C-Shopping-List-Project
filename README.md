# C Shopping List and Bill Calculator

A lightweight, beginner-friendly console application written in C (Standard C11) that enables users to manage a shopping list, perform case-insensitive item search, calculate subtotals with exact integer-paise arithmetic, and generate a complete itemized bill.

---

## 1. Project Overview

The **C Shopping List and Bill Calculator** is a robust command-line application designed for everyday shopping management. It provides a prompt-driven interface for recording purchased items, specifying quantities, and entering unit prices. The application computes an itemized summary with exact subtotals and a grand total bill while avoiding floating-point rounding errors. It also provides an interactive, case-insensitive search feature to quickly check if an item is present on the list.

## 2. Features

- **Case-Insensitive Item Search**: Fast linear lookup that matches items regardless of capitalization (e.g., `Milk`, `milk`, `MILK`, and `mIlK` all match), while preserving original casing in the final bill.
- **Exact Monetary Precision (Paise-Based)**: All prices, subtotals, and grand totals are calculated and stored internally as 64-bit integer paise (`int64_t`, where ₹1.00 = 100 paise). This completely eliminates floating-point rounding inaccuracies.
- **Overflow-Protected Arithmetic**: Checked integer multiplication for item subtotals and checked accumulation for the total bill protect against numeric overflow.
- **Strict Input Validation & Overlong Line Flushing**:
  - Detects and rejects overlong lines, automatically flushing remaining characters so subsequent prompts are never corrupted.
  - Rejects empty and whitespace-only entries.
  - Rejects non-numeric text (`abc`), partially numeric entries (`2abc`), and decimal numbers where integers are required (`2.5`).
  - Rejects unsupported precision for prices (prices with more than two decimal places like `25.555` are rejected rather than silently rounded).
  - Rejects negative prices, negative quantities, zero quantities, and invalid keywords (`nan`, `inf`).
  - Safely handles End-Of-File (EOF) conditions without crashes or infinite loops.
- **Clean Table Formatting**: Clear itemized bill summary with aligned columns: Serial Number (starting at 1), Item Name, Quantity, Unit Price, and Subtotal.
- **Safe Memory Management**: Validates item count before allocation, dynamically allocates memory using `malloc()`, and cleanly frees all resources on every exit path.
- **Automated Test Suite**: Unit and integration test suite covering search, validation, arithmetic, and shopping list logic with 100% pass rate.

## 3. Requirements

- **Operating System**: Windows, Linux, or macOS
- **Compiler**: GCC (e.g., MinGW on Windows) supporting C11 (`-std=c11`) or later
- **Standard Libraries**: `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<ctype.h>`, `<errno.h>`, `<stdint.h>`, `<stdbool.h>`

## 4. Project Structure

```text
project/
├── shopping_list.c             # Main console application and user interface
├── shopping_logic.c            # Core business logic: search, parsing, and arithmetic
├── shopping_logic.h            # Header defining data structures, limits, and function prototypes
├── tests/
│   └── test_shopping_list.c    # Automated unit and integration test suite
├── README.md                   # Comprehensive project documentation
└── .gitignore                  # Git ignore rules for binaries and temporary artifacts
```

## 5. Compilation Instructions

To compile the application using GCC with strict warning flags enabled, navigate to the project directory and execute:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic shopping_list.c shopping_logic.c -o shopping_list.exe
```

### Compiler Flags Explained:
- `-std=c11`: Compiles against the ISO C11 standard.
- `-Wall`: Enables standard compiler warning diagnostics.
- `-Wextra`: Enables extra warning diagnostics.
- `-Wpedantic`: Enforces strict ISO C compliance.
- `-o shopping_list.exe`: Specifies the output binary name.

## 6. Running the Application

In Windows Command Prompt or PowerShell:

```bat
shopping_list.exe
```

On Linux or macOS:

```bash
./shopping_list.exe
```

## 7. Automated Test Suite Compilation and Execution

The project includes an automated test runner (`test_shopping_list.c`) that exercises the core search, input validation, monetary parsing, arithmetic, and basket integration logic.

### Compile the Test Suite:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -I. tests/test_shopping_list.c shopping_logic.c -o test_shopping_list.exe
```

### Run the Tests:

```bat
test_shopping_list.exe
```

### Test Suite Output:
The test runner executes 66 automated test assertions covering:
- Exact and case-insensitive item search variants (`milk`, `MILK`, `mIlK`, `whole wheat bread`).
- Missing and boundary search queries.
- Integer parsing, whitespace handling, and range enforcement.
- Detection and rejection of partial numerics (`2abc`) and decimals in integer fields (`2.5`).
- Exact decimal price parsing to paise (`25.50` -> 2550 paise, `40` -> 4000 paise).
- Rejection of prices with >2 decimal places (`25.555`), negative prices, and keywords (`nan`, `inf`).
- Multiplication and addition overflow detection.
- Full shopping basket integration with single, multiple, and maximum-capacity (100) items.

## 8. Case-Insensitive Search Behavior

Item search is performed using a portable standard C helper function:

```c
int case_insensitive_compare(const char *s1, const char *s2);
```

- Each character is compared using `tolower((unsigned char)character)`.
- If a user enters `Milk` during item entry, queries such as `milk`, `MILK`, `mIlK`, or `Milk` will all successfully locate the item.
- Original item names retain their entered casing for display in the final itemized bill.
- If an item is not found, the program displays a clear notification: `No <search_term> is not in your shopping list`.

## 9. Monetary Representation and Rounding Rules

Traditional floating-point representations (`float` and `double`) suffer from binary representation errors (e.g., `0.1 + 0.2 != 0.3`). To guarantee exact accounting:

- **Internal Unit**: Monetary values are represented internally as integer paise (`int64_t`).
  - ₹1.00 = 100 paise.
  - ₹25.50 is stored as `2550` paise.
  - ₹40.00 is stored as `4000` paise.
- **Parsing Precision**: The parser accepts whole numbers and numbers with up to two decimal places.
  - `25` -> `2500` paise.
  - `25.5` -> `2550` paise.
  - `25.50` -> `2550` paise.
  - `0.05` -> `5` paise.
- **Unsupported Precision Rejection**: Inputs specifying more than two decimal places (e.g., `25.555`) are rejected with an explicit error rather than being silently rounded or truncated.
- **Display Output**: Formatted using `format_paise_to_rupees()`, producing exact two-decimal rupee strings (e.g., `265.25`).

## 10. Input Validation and Overlong Line Handling

The application features a line-reading helper (`read_console_line()`) that guarantees safe console input:

- **Overlong Line Detection**: If user input exceeds the buffer size, the helper detects the overflow, flushes the remaining characters from `stdin` until `\n` or `EOF`, and flags the line as overlong.
- **No Residual Corruption**: Flushing prevents leftover characters from leaking into subsequent prompts.
- **Strict Format Checking**:
  - `parse_int_string()` enforces that the entire trimmed string represents an integer; trailing characters like `2abc` or `.5` are rejected.
  - `parse_price_string()` validates digits, decimal points, and bounds.
- **EOF Handling**: Clean detection of `EOF` (Ctrl+D / Ctrl+Z) terminates the program safely with memory freed, avoiding infinite loops.

## 11. Supported Limits

| Parameter | Minimum | Maximum | Description |
| :--- | :--- | :--- | :--- |
| **Item Count ($n$)** | `1` | `100` | Number of items in a shopping list |
| **Item Name Length** | `1` char | `49` chars | Maximum characters per item name |
| **Quantity** | `1` | `1,000,000` | Strictly positive integer quantity |
| **Unit Price** | `₹0.00` | `₹10,000,000.00` | Valid non-negative price in rupees |
| **Price Precision** | `0` decimals | `2` decimals | Max 2 decimal places allowed |
| **Search Option** | `0` | `1` | `0` = No, `1` = Yes |

## 12. Subtotal and Grand Total Calculation Rules

1. **Subtotal**: Calculated as:
   $$\text{Subtotal (paise)} = \text{Quantity} \times \text{Unit Price (paise)}$$
   Before multiplication, overflow is prevented by checking:
   $$\text{Quantity} > \frac{\text{INT64\_MAX}}{\text{Unit Price}}$$
2. **Grand Total**: Accumulated by summing each item's subtotal:
   $$\text{Total (paise)} = \sum \text{Subtotal}_i$$
   Checked via:
   $$\text{INT64\_MAX} - \text{Total} < \text{Subtotal}$$

## 13. Sample Session

Below is a complete interaction demonstrating item entry, case-insensitive search, and exact bill calculation:

```text
-----Welcome to Your Shopping list -----
Enter the number of items you purchase: 
3
----- Entering the item 1 of 3 -----
Enter the itemname 1 
Bread
Enter the quantity of item 
3
Enter the item price 
40.00
----- Entering the item 2 of 3 -----
Enter the itemname 2 
Butter
Enter the quantity of item 
1
Enter the item price 
55.25
----- Entering the item 3 of 3 -----
Enter the itemname 3 
Eggs
Enter the quantity of item 
12
Enter the item price 
7.50
Can You search any item in the list bring or not bring ? Yes(1) / No(0)
1
Enter the search item name ?
bread
----- Result of Search item -----
Yes bread is in your shopping list

----- Your Shopping Bill List -----

 SNO | Item Name        | Quantity   | Price        | Subtotal    
-------------------------------------------------------------------
   1 | Bread            |          3 |        40.00 |       120.00
   2 | Butter           |          1 |        55.25 |        55.25
   3 | Eggs             |         12 |         7.50 |        90.00
-------------------------------------------------------------------
Your total bill is 265.25
```

## 14. Actual Tests Executed and Results

The test suite in [`tests/test_shopping_list.c`](tests/test_shopping_list.c) was compiled with GCC and executed. All 66 tests passed:

- **Search Tests (14/14 Passed)**: Verified exact match, lowercase (`milk`), uppercase (`MILK`), mixed case (`mIlK`), multi-word items, last-item lookup, missing items (`Cheese`), prefix non-matches, and empty/whitespace inputs.
- **Input Validation Tests (17/17 Passed)**: Verified minimum/maximum integers, negative rejection, zero rejection, non-numeric rejection, trailing garbage rejection (`2abc`), decimal rejection (`2.5`), integer overflow, and empty/blank names.
- **Monetary Calculation Tests (24/24 Passed)**: Verified exact paise conversion (`25.50` -> 2550), multiplication ($2 \times ₹25.50 = ₹51.00$, $3 \times ₹40.00 = ₹120.00$, $1 \times ₹55.25 = ₹55.25$, $12 \times ₹7.50 = ₹90.00$), subtotal accumulation ($₹120.00 + ₹55.25 + ₹90.00 = ₹265.25$), zero-price items, 3-decimal precision rejection (`25.555`), keyword rejection (`nan`, `inf`), and arithmetic overflow protections.
- **Integration Tests (11/11 Passed)**: Verified single item lists, multiple item baskets, 100-item maximum capacity lists, and invalid item count rejection.

## 15. Possible Future Improvements

The following improvements could be considered in future releases:
- **File Persistence**: Export and import shopping lists and bill receipts to CSV or JSON format.
- **Item Modifications**: Interactive menu to edit item quantities, prices, or remove items before finalizing the bill.
- **Discount & Tax Support**: Configurable discount percentages and sales tax computation.
- **Multi-Currency Formatting**: Support for configurable currency symbols (`$`, `€`, `₹`, `£`).
