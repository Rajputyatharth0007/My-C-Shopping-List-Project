# C Shopping List and Bill Calculator

A lightweight, beginner-friendly console application written in C that enables users to manage a shopping list, search for items, calculate subtotals, and generate a complete itemized bill.

---

## 1. Project Overview

The **C Shopping List and Bill Calculator** is a command-line utility designed for everyday shopping management. It provides an intuitive interface for entering purchased items, specifying quantities, entering unit prices, and instantly computing an itemized summary with subtotals and the total bill. It also includes an interactive search feature to quickly check if an item is present on the list.

## 2. Objectives

- Provide a reliable, crash-free console application for managing shopping lists.
- Demonstrate safe C programming practices including robust input validation, dynamic memory management, and buffer overflow prevention.
- Fix common beginner bugs present in early iterations (e.g., incorrect string comparisons, hardcoded loops, unvalidated `scanf` calls, and array index errors).
- Serve as an educational reference for students learning C fundamentals.

## 3. Features

- **Itemized Shopping Entry**: Prompt-driven entry for item names, positive quantities, and unit prices.
- **Interactive Item Search**: Fast linear lookup verifying whether an item is on the shopping list, iterating over all entered items.
- **Accurate Bill Calculation**: Computes subtotals (`quantity × price`) and accumulates the grand total with two-decimal precision.
- **Clean Table Formatting**: Formatted bill summary with aligned columns: Serial Number (starting at 1), Item Name, Quantity, Price, and Subtotal.
- **Robust Input Validation**:
  - Rejects non-numeric, negative, or out-of-range item counts.
  - Enforces strictly positive quantities.
  - Rejects negative or non-finite prices.
  - Prevents buffer overflow by safely capturing string inputs.
  - Validates search confirmation prompts (allowing only 0 or 1).
  - Gracefully handles unexpected End-Of-File (EOF) conditions without infinite loops or undefined behavior.
- **Safe Memory Management**: Allocates memory dynamically based on the validated item count and cleanly releases all resources before program termination.

## 4. Technologies Used

- **Language**: C (Standard C11)
- **Standard Libraries**: `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<ctype.h>`, `<errno.h>`, `<math.h>`
- **Compiler Support**: Compatible with GCC, Clang, and MSVC

## 5. Requirements

- **Operating System**: Windows, Linux, or macOS
- **Compiler**: GCC (e.g., MinGW on Windows) supporting C11 (`-std=c11`) or later

## 6. Project Structure

```text
project/
├── shopping_list.c    # Main C source file
├── README.md          # Project documentation
└── .gitignore         # Build artifacts and editor ignore rules
```

## 7. Compilation Instructions

To compile the application using GCC with comprehensive warning flags enabled, navigate to the project directory in your terminal and execute:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic shopping_list.c -o shopping_list.exe
```

### Compiler Flags Explained:
- `-std=c11`: Compiles against the ISO C11 standard.
- `-Wall`: Enables all common compiler warnings.
- `-Wextra`: Enables additional compiler warning diagnostics.
- `-Wpedantic`: Enforces strict compliance with standard ISO C.
- `-o shopping_list.exe`: Specifies the output executable name.

## 8. Running the Application

In Windows Command Prompt or PowerShell:

```bat
shopping_list.exe
```

On Linux or macOS:

```bash
./shopping_list
```

## 9. Sample Session

Below is an example interaction with the program:

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
Bread
----- Result of Search item -----
Yes Bread is in your shopping list

----- Your Shopping Bill List -----

 SNO | Item Name        | Quantity   | Price      | Subtotal  
---------------------------------------------------------------
   1 | Bread            |          3 |      40.00 |     120.00
   2 | Butter           |          1 |      55.25 |      55.25
   3 | Eggs             |         12 |       7.50 |      90.00
---------------------------------------------------------------
Your total bill is 265.25
```

## 10. How the Program Works

1. **Initialization**: The program prompts the user for the number of items (`n`) to purchase, validating that \(1 \le n \le 100\).
2. **Memory Allocation**: Dynamic memory for `n` `ShoppingItem` structs is allocated via `malloc()`. If allocation fails, the program logs an error and exits cleanly.
3. **Data Collection Loop**: For each item, the program safely collects the name, quantity, and unit price using dedicated input validation functions.
4. **Search Operation**: If requested (`1`), the user enters a search term. The search function traverses the array using `strcmp()`. If an exact match is found, it reports success; otherwise, it reports that the item is absent.
5. **Bill Output**: The program iterates through all items, prints each row with serial numbers starting at `1`, computes the subtotal (`quantity × price`), and accumulates the grand total.
6. **Cleanup**: Dynamically allocated memory is deallocated using `free()`, preventing memory leaks.

## 11. Input Validation and Error Handling

- **Safe Line Reading (`fgets`)**: Avoids buffer overflows and trailing newline anomalies commonly associated with unconstrained `scanf()` calls.
- **Checked Numeric Conversion**:
  - `strtol()` validates integers, detecting invalid non-numeric characters, overflow (`ERANGE`), and trailing garbage.
  - `strtof()` validates floating-point numbers, rejecting non-finite values (`isnan`, `isinf`) and negative prices.
- **Range Constraints**:
  - Item count: `1` to `100` items.
  - Quantity: `1` to `1,000,000`.
  - Price: `0.00` to `10,000,000.00`.
  - Search prompt: strictly `0` or `1`.
- **EOF Detection**: All read operations detect EOF conditions and exit gracefully without crashing or entering infinite loops.

## 12. Possible Future Improvements

The following enhancements could be considered in future releases:
- **File Persistence**: Ability to save bills and shopping history to a CSV or text file and reload them later.
- **Case-Insensitive Search**: Support search comparisons that ignore case differences (e.g., matching "bread" with "Bread").
- **Quantity Updates**: Allow updating or removing items from the list before final bill calculation.
- **Discounts and Taxes**: Add options for applying discount percentages and sales tax to the total bill.
- **Multiple Currency Symbols**: Support customizable currency indicators (e.g., `$`, `€`, `₹`).
