/**
 * @file color_lib.h
 * @brief A comprehensive C library for ANSI color codes, text styling, and cursor manipulation.
 *
 * This library includes a built-in Garbage Collector (GC) to manage memory for 
 * dynamic format strings, ensuring easy usage without memory leaks.
 * It also handles system signals to reset the terminal before exiting.
 */

#ifndef COLOR_LIB_H
#define COLOR_LIB_H

/* --- Type Definitions --- */

#ifndef UINT8_MAX
    typedef unsigned char      uint8_t;
#endif

#ifndef UINT16_MAX
    typedef unsigned short     uint16_t;
#endif

/**
 * @brief Node structure for the internal Garbage Collector.
 */
typedef struct s_gc_node {
    void *ptr;              /**< Pointer to the allocated memory. */
    struct s_gc_node *next; /**< Pointer to the next node in the list. */
} t_gc_node;

/* --- Garbage Collector Functions --- */

/**
 * @brief Adds a pointer to the Garbage Collector.
 * * The pointer will be automatically freed when gc_clean_all() is called.
 * * @param ptr Pointer to the allocated memory (must be heap-allocated).
 * @warning If internal allocation fails, 'ptr' is freed immediately to prevent leaks.
 */
void gc_add(void *ptr);

/**
 * @brief Frees all memory tracked by the Garbage Collector.
 * * Cleans both the active list and the trash list.
 * Should be called at the end of execution (handled automatically by atexit).
 */
void gc_clean_all(void);

/**
 * @brief Resets the current color style and moves active GC nodes to trash.
 * * This allows "soft" cleaning: memory is not freed yet (to allow using strings 
 * in the current print statement), but will be freed on the next cycle.
 * * @return const char* The ANSI reset code ("\033[0m").
 */
const char *gc_reset(void);


/* --- Core Library Functions --- */

/**
 * @brief Prints a message to stdout (wrapper).
 * @param msg The string to print.
 */
void print(char *msg);

/**
 * @brief Gets the current ANSI escape character used.
 * @return The escape string (usually "\033").
 */
const char *get_ansi_esc_char(void);

/**
 * @brief Checks if the cursor is set to auto-show on exit.
 * @return 1 if true, 0 otherwise.
 */
unsigned char get_cursor_auto_show(void);

/**
 * @brief Checks if the library is set to auto-clean on exit.
 * @return 1 if true, 0 otherwise.
 */
unsigned char get_auto_clean(void);

/**
 * @brief Performs cleanup, resets terminal, and shows cursor.
 * * Registered with atexit() to ensure terminal state is restored
 * when the program ends.
 */
void auto_clean(void);


typedef enum {
    COLOR_FLAG_NONE         = 0,

    COLOR_FLAG_INIT_FORE    = (1 << 0),
    COLOR_FLAG_INIT_BACK    = (1 << 1),
    COLOR_FLAG_INIT_STYLE   = (1 << 2),
    COLOR_FLAG_INIT_DISABLE = (1 << 3),
    COLOR_FLAG_INIT_DEFAULT = (1 << 4),
    COLOR_FLAG_INIT_FONT    = (1 << 5),
    COLOR_FLAG_INIT_MISC    = (1 << 6),
    COLOR_FLAG_INIT_CURSOR  = (1 << 7),
    COLOR_FLAG_INIT_SCREEN  = (1 << 8),

    COLOR_FLAG_INIT_ALL     = (COLOR_FLAG_INIT_FORE | COLOR_FLAG_INIT_BACK | COLOR_FLAG_INIT_STYLE | COLOR_FLAG_INIT_DISABLE | COLOR_FLAG_INIT_DEFAULT | COLOR_FLAG_INIT_FONT | COLOR_FLAG_INIT_MISC | COLOR_FLAG_INIT_CURSOR | COLOR_FLAG_INIT_SCREEN),

    COLOR_FLAG_DEFAULT      = COLOR_FLAG_INIT_ALL
} ColorInitFlags;

/**
 * @brief Initializes all necessary color, style, and control sequences based on provided flags.
 *
 * This function sets the global escape character, cursor behavior, and initializes
 * various escape sequence categories (Fore, Back, Style, Cursor, etc.) or sets up
 * runtime behavior (signal handling, atexit cleanup).
 * @param o_ansi_esc_char The ANSI escape character prefix (e.g., "\\033"). NULL uses the default ("\\033"). Ex :  "\1xb" or "\e"
 * @param o_cursor_auto_show If non-zero, the cursor will be automatically shown/hidden by certain functions.
 * @param o_auto_clean If non-zero, the program will clean-up automaticaly after the end of the program
 * @param o_flags Bitwise OR of ColorInitFlags to control which components are initialized or enabled.
 */
void init_color(const char *o_ansi_esc_char, const unsigned char o_cursor_auto_show, const unsigned char o_auto_clean, const unsigned char o_intercept_sig, int active_flags);


/* --- Dynamic String Generators (Added from .c) --- */
/* These return strings managed by the GC */

char *custom_code(unsigned char code);
char *cursor_cup(uint16_t row, uint16_t column);
char *cursor_cuu(uint16_t n); // Cursor Up
char *cursor_cud(uint16_t n); // Cursor Down
char *cursor_cuf(uint16_t n); // Cursor Forward
char *cursor_cub(uint16_t n); // Cursor Back

/* 8-bit Colors (256 colors) */
char *fore_color8(uint8_t color);
char *back_color8(uint8_t color);
char *underline_color8(uint8_t color);

/* TrueColor (RGB 24-bit) */
char *fore_color24(uint8_t r, uint8_t g, uint8_t b);
char *back_color24(uint8_t r, uint8_t g, uint8_t b);
char *underline_color24(uint8_t r, uint8_t g, uint8_t b);


/* --- Constants & Structures --- */

#define COLOR_STR_SIZE 8

#define NB_FORE_COLORS 16
#define NB_BACK_COLORS 16
#define NB_STYLE 14
#define NB_DISABLE 11
#define NB_DEFAULT 4
#define NB_FONT 10
#define NB_MISC 17
#define NB_CURSOR 6
#define NB_SCREEN 4


/**
 * @brief Union for Foreground Colors.
 * Access via name (e.g., Fore.RED) or index (Fore.array[i]).
 */
typedef union u_fore {
    struct {
        char BLACK[COLOR_STR_SIZE];
        char RED[COLOR_STR_SIZE];
        char GREEN[COLOR_STR_SIZE];
        char YELLOW[COLOR_STR_SIZE];
        char BLUE[COLOR_STR_SIZE];
        char MAGENTA[COLOR_STR_SIZE];
        char CYAN[COLOR_STR_SIZE];
        char WHITE[COLOR_STR_SIZE];
        /* BRIGHT */
        char BRIGHT_BLACK[COLOR_STR_SIZE];
        char BRIGHT_RED[COLOR_STR_SIZE];
        char BRIGHT_GREEN[COLOR_STR_SIZE];
        char BRIGHT_YELLOW[COLOR_STR_SIZE];
        char BRIGHT_BLUE[COLOR_STR_SIZE];
        char BRIGHT_MAGENTA[COLOR_STR_SIZE];
        char BRIGHT_CYAN[COLOR_STR_SIZE];
        char BRIGHT_WHITE[COLOR_STR_SIZE];
    };
    char array[NB_FORE_COLORS][COLOR_STR_SIZE];
} t_fore;

/**
 * @brief Union for Background Colors.
 */
typedef union u_back {
    struct {
        char BLACK[COLOR_STR_SIZE];
        char RED[COLOR_STR_SIZE];
        char GREEN[COLOR_STR_SIZE];
        char YELLOW[COLOR_STR_SIZE];
        char BLUE[COLOR_STR_SIZE];
        char MAGENTA[COLOR_STR_SIZE];
        char CYAN[COLOR_STR_SIZE];
        char WHITE[COLOR_STR_SIZE];
        /* BRIGHT */
        char BRIGHT_BLACK[COLOR_STR_SIZE];
        char BRIGHT_RED[COLOR_STR_SIZE];
        char BRIGHT_GREEN[COLOR_STR_SIZE];
        char BRIGHT_YELLOW[COLOR_STR_SIZE];
        char BRIGHT_BLUE[COLOR_STR_SIZE];
        char BRIGHT_MAGENTA[COLOR_STR_SIZE];
        char BRIGHT_CYAN[COLOR_STR_SIZE];
        char BRIGHT_WHITE[COLOR_STR_SIZE];
    };
    char array[NB_BACK_COLORS][COLOR_STR_SIZE];
} t_back;

/**
 * @brief Union for Text Styles.
 */
typedef union u_style {
    struct {
        char RESET[COLOR_STR_SIZE];
        char BOLD[COLOR_STR_SIZE];
        char BRIGHT[COLOR_STR_SIZE];
        char DIM[COLOR_STR_SIZE];
        char LOW[COLOR_STR_SIZE];
        char ITALIC[COLOR_STR_SIZE];
        char UNDERLINE[COLOR_STR_SIZE];
        char BLINK[COLOR_STR_SIZE];
        char BLINK_SPEED[COLOR_STR_SIZE];
        char REVERSE[COLOR_STR_SIZE];
        char HIDDEN[COLOR_STR_SIZE];
        char INVISIBLE[COLOR_STR_SIZE];
        char STRIKETHROUGH[COLOR_STR_SIZE];
        char UNDERLINE_DOUBLE[COLOR_STR_SIZE];
        
        /**
         * @brief Function pointer to reset logic.
         * Usage: print((char*)Style.RESET_ALL());
         */
        const char *(*RESET_ALL)(void);
    };
    char array[NB_STYLE][COLOR_STR_SIZE];
} t_style;

/**
 * @brief Union for Disabling specific styles.
 */
typedef union u_disable {
    struct {
        char BOLD[COLOR_STR_SIZE];
        char INTENSITY[COLOR_STR_SIZE];
        char ITALIC[COLOR_STR_SIZE];
        char FRAKTUR[COLOR_STR_SIZE];
        char UNDERLINE[COLOR_STR_SIZE];
        char BLINK[COLOR_STR_SIZE];
        char REVERSE[COLOR_STR_SIZE];
        char HIDDEN[COLOR_STR_SIZE];
        char INVISIBLE[COLOR_STR_SIZE];
        char STRIKETHROUGH[COLOR_STR_SIZE];
        char PROPORTIONAL_SPACING[COLOR_STR_SIZE];
        char FRAMED_ENCIRCLED[COLOR_STR_SIZE];
        char OVERLINED[COLOR_STR_SIZE];
        char SUB_SUP_SCRIPT[COLOR_STR_SIZE];
    };
    char array[NB_DISABLE][COLOR_STR_SIZE];
} t_disable;

/**
 * @brief Union for Default terminal settings.
 */
typedef union u_default {
    struct {
        char FONT[COLOR_STR_SIZE];
        char FORE[COLOR_STR_SIZE];
        char BACK[COLOR_STR_SIZE];
        char UNDERLINE[COLOR_STR_SIZE];
    };
    char array[NB_DEFAULT][COLOR_STR_SIZE];
} t_default;

/**
 * @brief Union for Alternative Fonts.
 */
typedef union u_font {
    struct {
        char ALTERNATIVE_11[COLOR_STR_SIZE];
        char ALTERNATIVE_12[COLOR_STR_SIZE];
        char ALTERNATIVE_13[COLOR_STR_SIZE];
        char ALTERNATIVE_14[COLOR_STR_SIZE];
        char ALTERNATIVE_15[COLOR_STR_SIZE];
        char ALTERNATIVE_16[COLOR_STR_SIZE];
        char ALTERNATIVE_17[COLOR_STR_SIZE];
        char ALTERNATIVE_18[COLOR_STR_SIZE];
        char ALTERNATIVE_19[COLOR_STR_SIZE];
        char FRAKTUR[COLOR_STR_SIZE];
    };
    char array[NB_FONT][COLOR_STR_SIZE];
} t_font;

/**
 * @brief Union for Miscellaneous formatting.
 */
typedef union u_misc {
    struct {
        char PROPORTIONAL_SPACE[COLOR_STR_SIZE];
        char FRAMED[COLOR_STR_SIZE];
        char ENCIRCLED[COLOR_STR_SIZE];
        char OVERLINED[COLOR_STR_SIZE];

        char IDEOGRAMME_UNDERLINE[COLOR_STR_SIZE];
        char IDEOGRAMME_RIGHT_SIDE_LINE[COLOR_STR_SIZE];

        char IDEOGRAMME_DOUBLE_UNDERLINE[COLOR_STR_SIZE];
        char IDEOGRAMME_DOUBLE_LINE_ON_THE_RIGHT_SIDE[COLOR_STR_SIZE];
        
        char IDEOGRAMME_OVERLINE[COLOR_STR_SIZE];
        char IDEOGRAMME_LEFT_SIDE_LINE[COLOR_STR_SIZE];

        char IDEOGRAMME_DOUBLE_OVERLINE[COLOR_STR_SIZE];
        char IDEOGRAMME_DOUBLE_LINE_ON_THE_LEFT_SIDE[COLOR_STR_SIZE];
        
        char IDEOGRAMME_STRESS_MARKING[COLOR_STR_SIZE];
        
        char NO_IDEOGRAM_ATTRIBUTES[COLOR_STR_SIZE];
        char IDEOGRAM_RESET_ATTRIBUTES[COLOR_STR_SIZE];

        char SUPERSCRIPT[COLOR_STR_SIZE];
        char SUBSCRIPT[COLOR_STR_SIZE];
    };
    char array[NB_MISC][COLOR_STR_SIZE];
} t_misc;

/**
 * @brief Union for Cursor manipulation codes.
 */
typedef union u_cursor {
    struct {
        char HOME[COLOR_STR_SIZE]; ///< Move cursor to home (0,0)
        char DSR[COLOR_STR_SIZE];  ///< Device Status Report
        char SCP[COLOR_STR_SIZE];  ///< Save Cursor Position
        char RCP[COLOR_STR_SIZE];  ///< Restore Cursor Position
        char HIDE[COLOR_STR_SIZE]; ///< Hide Cursor
        char SHOW[COLOR_STR_SIZE]; ///< Show Cursor
    };
    char array[NB_CURSOR][COLOR_STR_SIZE];
} t_cursor;

/**
 * @brief Union for Screen clearing codes.
 */
typedef union u_screen {
    struct {
        char CLEAR[COLOR_STR_SIZE];          ///< Clear entire screen
        char CLEAR_BUFF[COLOR_STR_SIZE];     ///< Clear scrollback buffer
        char LINE_ERASE_CUR[COLOR_STR_SIZE]; ///< Erase from cursor to end of line
        char LINE_ERASE_ALL[COLOR_STR_SIZE]; ///< Erase entire line
    };
    char array[NB_SCREEN][COLOR_STR_SIZE];
} t_screen;

/* --- Global Instances --- */

extern t_fore Fore;
extern t_back Back;
extern t_style Style;
extern t_disable Disable;
extern t_default Default;
extern t_font Font;
extern t_misc Misc;
extern t_cursor Cursor;
extern t_screen Screen;

/**
 * @brief Runs a full demonstration of the library's capabilities.
 * Prints all colors, styles, and animations to stdout.
 */
void color_support_test(void);


#endif /* COLOR_LIB_H */