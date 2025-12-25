# ANSI Color Library

[🇫🇷 Lire en Français](README.fr.md)

A comprehensive C library for terminal manipulation: color management, text styling, and cursor positioning.

This library features a built-in **Garbage Collector (GC)**. This mechanism automatically handles the allocation and deallocation of formatting strings, allowing for fluid use of dynamic colors without memory leaks or the need for manual management (no `free` needed).

## Key Features

* **Color Management**: Full support for standard colors (ANSI 4-bit), extended colors (8-bit), and TrueColor (RGB 24-bit).
* **Text Styles**: Easily apply styles such as bold, italic, underline, blink, etc.
* **Cursor Control**: Absolute and relative positioning, hiding, and showing the cursor.
* **Automated Memory Management**: Dynamically generated strings are automatically cleaned up.
* **Stability & Signals**: Native interception of system signals (`SIGINT`, `SIGSEGV`, etc.) ensures the terminal is always reset to a clean state, even if the program stops abruptly.

## Integration

The library is designed to be dropped directly into your source code.

1.  Copy `color_lib.c` and `color_lib.h` to your project root or source folder.
2.  Include the header in your files:
    ```c
    #include "color_lib.h"
    ```
3.  Compile the files along with your project:
    ```bash
    gcc main.c color_lib.c -o my_app
    ```

## Usage Guide

Library initialization is automatic. You can use the features immediately at the start of your `main` function.
*Optional: You can manually call `init_color` to customize behavior (see the [Advanced Initialization](#advanced-initialization) section).*

### 1. Static Styles

For standard colors and styles, use the global structures `Fore` (foreground), `Back` (background), and `Style`.

```c
printf("%sError message in red%s\n", Fore.RED, Style.RESET);
printf("%s%sWhite text on blue background%s\n", Back.BLUE, Fore.WHITE, Style.RESET);
printf("%sBold and underlined text%s\n", Style.BOLD, Style.UNDERLINE);

// Don't forget to reset the style at the end

```

> :warning: `Style.RESET_ALL` will also empty the GC.

### 2. Dynamic Colors (TrueColor & 8-bit)

Use generator functions for specific colors (e.g., RGB).
**Important:** You do not need to free the returned strings; the Garbage Collector handles it.

```c
// Custom Orange Text (RGB: 255, 165, 0)
printf("%sCustom colored title%s\n", fore_color24(255, 165, 0), Style.RESET);

// Background using 8-bit palette (Index 200)
printf("%sBackground index 200%s\n", back_color8(200), Style.RESET);

```

### 3. Cursor Manipulation

Create TUIs (Text User Interfaces) or simple animations.

```c
print(Cursor.HIDE);             // Hides cursor for a cleaner interface
print(cursor_cup(10, 20));      // Moves cursor to row 10, col 20
printf("Main Menu");
print(Screen.CLEAR);            // Clears the entire screen

```

### 4. Memory Management in Loops

If your program runs in an infinite loop (game loop, rendering server), memory allocated for dynamic colors must be cleaned periodically.
Use `gc_reset()`. It performs two actions:

1. Returns the ANSI reset string (`\033[0m`).
2. Marks memory used in the previous cycle for deletion.

```c
while (running) {
    // Generate a random color each tick
    char *color = fore_color24(rand() % 255, 0, 0);
    
    // gc_reset() closes the style and cleans memory from the previous loop
    printf("%sCurrent Score: %d%s\n", color, score, gc_reset());
    
    sleep(1);
}

```

## API Reference

### Global Structures

| Structure | Description | Usage Examples |
| --- | --- | --- |
| `Fore` | Text colors (16 basic colors) | `Fore.RED`, `Fore.BRIGHT_CYAN` |
| `Back` | Background colors | `Back.BLACK`, `Back.YELLOW` |
| `Style` | Text modifiers | `Style.BOLD` (Bold), `Style.DIM` (Dim), `Style.RESET` |
| `Cursor` | Cursor actions | `Cursor.HIDE`, `Cursor.SHOW`, `Cursor.HOME` |
| `Screen` | Screen actions | `Screen.CLEAR` (Clear all), `Screen.LINE_ERASE_ALL` |

### Utility Functions

| Function | Description |
| --- | --- |
| `cursor_cup(row, col)` | Moves cursor to the specified position (Row, Column). |
| `fore_color24(r, g, b)` | Generates an RGB text color string (TrueColor). |
| `back_color24(r, g, b)` | Generates an RGB background color string (TrueColor). |
| `gc_reset()` | Resets style and cleans memory from the previous cycle. |

*More functions are available in `color_lib.h`.*

## Technical Notes

* **Lifecycle**: The library uses `atexit` to ensure the terminal is restored (cursor visible, default colors) when the program ends normally.
* **Error Handling**: An internal signal handler intercepts interruptions (Ctrl+C) or crashes (Segfault) to restore the terminal state before exiting.
* **Threading**: The Garbage Collector uses global resources without mutexes. For multi-threaded applications, restrict dynamic color usage to the main thread or implement your own locking mechanism.

## Advanced Initialization

By default, the library initializes itself before `main` runs. To customize this, you can manually call `init_color`.

**Default call equivalent:**

```c
init_color(NULL, 1, 1, 1, COLOR_FLAG_INIT_DEFAULT);

```

### Parameters

1. **`escape_char`** (`char*`): The escape character to use.
* Defaults to `\033` if `NULL`.
* Alternatives: `\x1b` or `\e`.


2. **`auto_show_cursor`** (`bool` / `int`):
* `1` (true): Sends `[?25h` at exit to ensure the cursor is visible.
* `0` (false): Cursor state is not modified at exit.


3. **`auto_clean`** (`bool` / `int`):
* `1` (true): Calls `gc_clean_all` and `auto_clean` at exit (`atexit`).
* `0` (false): You must manually `free` memory to avoid leaks.


4. **`catch_signals`** (`bool` / `int`):
* `1` (true): Intercepts crash signals and sends `\033[0m` to reset colors.
* `0` (false): Does nothing on crash.


5. **`flags`** (`int`): Defines which structures are initialized/accessible.
* See header file for flag list. Use `COLOR_FLAG_INIT_DEFAULT` for standard usage.



### Compatibility Test

You can run `color_support_test()` to check feature compatibility. It takes no arguments and returns nothing; check the terminal output to see which features render correctly on your system.