#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "color_lib.h"


static const int SIGNALS_TO_HANDLE[] = {
    SIGINT,
    SIGQUIT,
    SIGTERM,
    SIGABRT,
    SIGSEGV,
    SIGFPE,
    SIGILL
};


/* --- Garbage Collector (gc) --- */
static t_gc_node *g_list_active = NULL;
static t_gc_node *g_list_trash = NULL;


static void free_list(t_gc_node **head) {
    t_gc_node *current = *head;
    t_gc_node *next;
    while (current) {
        next = current->next;
        free(current->ptr);
        free(current);
        current = next;
    }
    *head = NULL;
}


void gc_add(void *ptr) {
    if (!ptr) return;
    t_gc_node *node = malloc(sizeof(t_gc_node));
    if (!node) {free(ptr);return;}
    
    node->ptr = ptr;
    node->next = g_list_active;
    g_list_active = node;
}


void gc_clean_all(void) {
    free_list(&g_list_active);
    free_list(&g_list_trash);
}


const char *gc_reset(void) {
    free_list(&g_list_trash);

    g_list_trash = g_list_active;
    g_list_active = NULL;

    return "\033[0m";
}


void print(char *msg) {
    printf("%s", msg);
}


static const char *g_ansi_esc_char = "\033";
static unsigned char g_cursor_auto_show = 1;
static unsigned char g_auto_clean = 1;


const char *get_ansi_esc_char(void) {
    return (char *)g_ansi_esc_char;
};


static char *gc_asprintf(const char *format, int arg) {
    int size = snprintf(NULL, 0, format, get_ansi_esc_char(), arg) + 1;
    char *str = malloc(size);
    if (!str) return NULL;
    
    snprintf(str, size, format, get_ansi_esc_char(), arg);
    
    gc_add(str);
    
    return str;
}


static char *gc_asprintf2(const char *format, int arg1, int arg2) {
    int size = snprintf(NULL, 0, format, get_ansi_esc_char(), arg1, arg2) + 1;
    char *str = malloc(size);
    if (!str) return NULL;
    
    snprintf(str, size, format, get_ansi_esc_char(), arg1, arg2);
    
    gc_add(str);
    
    return str;
}


static char *gc_asprintf3(const char *format, int arg1, int arg2, int arg3) {
    int size = snprintf(NULL, 0, format, get_ansi_esc_char(), arg1, arg2, arg3) + 1;
    char *str = malloc(size);
    if (!str) return NULL;
    
    snprintf(str, size, format, get_ansi_esc_char(), arg1, arg2, arg3);
    
    gc_add(str);
    
    return str;
}


unsigned char get_cursor_auto_show(void) {
    return g_cursor_auto_show;
}


unsigned char get_auto_clean(void) {
    return g_auto_clean;
}


void auto_clean(void) {
    printf("%s", (char *)get_ansi_esc_char());
    printf("[0m");

    gc_clean_all();

    if (get_cursor_auto_show()) {
        printf("%s", (char *)get_ansi_esc_char());
        printf("[?25h");
    }
}


void handle_signal(int sig) {
    const char *msg = "\033[0m\n";
    write(STDERR_FILENO, msg, 5);
    _exit(128 + sig);
}


void setup_signals(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handle_signal;

    sigfillset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;

    size_t count = sizeof(SIGNALS_TO_HANDLE) / sizeof(int);
    
    for (size_t i = 0; i < count; i++) {
        if (sigaction(SIGNALS_TO_HANDLE[i], &sa, NULL) == -1) {

        }
    }
}


t_fore Fore;
static const char *RAW_FORE_CODES[NB_FORE_COLORS] = {
    "[30m", // BLACK
    "[31m", // RED
    "[32m", // GREEN
    "[33m", // YELLOW
    "[34m", // BLUE
    "[35m", // MAGENTA
    "[36m", // CYAN
    "[37m", // WHITE
    /* BRIGHT */
    "[90m", // BRIGHT_BLACK
    "[91m", // BRIGHT_RED
    "[92m", // BRIGHT_GREEN
    "[93m", // BRIGHT_YELLOW
    "[94m", // BRIGHT_BLUE
    "[95m", // BRIGHT_MAGENTA
    "[96m", // BRIGHT_CYAN
    "[97m"  // BRIGHT_WHITE
};


t_back Back;
static const char *RAW_BACK_CODES[NB_BACK_COLORS] = {
    "[40m", // BLACK
    "[41m", // RED
    "[42m", // GREEN
    "[43m", // YELLOW
    "[44m", // BLUE
    "[45m", // MAGENTA
    "[46m", // CYAN
    "[47m", // WHITE
    /* BRIGHT */
    "[100m", // BRIGHT_BLACK
    "[101m", // BRIGHT_RED
    "[102m", // BRIGHT_GREEN
    "[103m", // BRIGHT_YELLOW
    "[104m", // BRIGHT_BLUE
    "[105m", // BRIGHT_MAGENTA
    "[106m", // BRIGHT_CYAN
    "[107m"  // BRIGHT_WHITE
};


t_style Style;
static const char *RAW_STYLE_CODES[NB_STYLE] = {
    "[0m", // RESET
    "[1m", // BOLD
    "[1m", // BRIGHT
    "[2m", // DIM
    "[2m", // LOW
    "[3m", // ITALIC
    "[4m", // UNDERLINE
    "[5m", // BLINK
    "[6m", // BLINK_SPEED
    "[7m", // REVERSE
    "[8m", // HIDDEN
    "[8m", // INVISIBLE
    "[9m", // STRIKETHROUGH
    "[21m", // UNDERLINE_DOUBLE
    // RESET_ALL
};


t_disable Disable;
static const char *RAW_DISABLE_CODES[NB_STYLE] = {
    "[21m", // BOLD
    "[22m", // INTENSITY
    "[23m", // ITALIC
    "[23m", // FRAKTUR
    "[24m", // UNDERLINE
    "[25m", // BLINK
    "[27m", // REVERSE
    "[28m", // HIDDEN
    "[28m", // INVISIBLE
    "[29m", // STRIKETHROUGH
    "[50m", // PROPORTIONAL_SPACING
    "[54m", // FRAMED_ENCIRCLED
    "[55m", // OVERLINED
};


t_default Default;
static const char *RAW_DEFAULT_CODES[NB_DEFAULT] = {
    "[10m", // FONT
    "[39m", // FORE
    "[49m", // BACK
    "[59m", // UNDERLINE
};


t_font Font;
static const char *RAW_FONT_CODES[NB_FONT] = {
    "[11m", // ALTENATIVE_11
    "[12m", // ALTENATIVE_12
    "[13m", // ALTENATIVE_13
    "[14m", // ALTENATIVE_14
    "[15m", // ALTENATIVE_15
    "[16m", // ALTENATIVE_16
    "[17m", // ALTENATIVE_17
    "[18m", // ALTENATIVE_18
    "[19m", // ALTENATIVE_19
    "[20m" // FRAKTUR
};


t_misc Misc;
static const char *RAW_MISC_CODES[NB_MISC] = {
    "[26m", // PROPORTIONAL_SPACE
    "[51m", // FRAMED
    "[52m", // ENCIRCLED
    "[53m", // OVERLINED

    "[60m", // IDEOGRAMME_UNDERLINE
    "[60m", // IDEOGRAMME_RIGHT_SIDE_LINE

    "[61m", // IDEOGRAMME_DOUBLE_UNDERLINE
    "[61m", // IDEOGRAMME_DOUBLE_LINE_ON_THE_RIGHT_SIDE
        
    "[62m", // IDEOGRAMME_OVERLINE
    "[62m", // IDEOGRAMME_LEFT_SIDE_LINE

    "[63m", // IDEOGRAMME_DOUBLE_OVERLINE
    "[63m", // IDEOGRAMME_DOUBLE_LINE_ON_THE_LEFT_SIDE
        
    "[64m", // IDEOGRAMME_STRESS_MARKING
        
    "[65m", // NO_IDEOGRAM_ATTRIBUTES
    "[65m", // IDEOGRAM_RESET_ATTRIBUTES
};


t_cursor Cursor;
static const char *RAW_CURSOR_CODES[NB_CURSOR] = {
    "[H", // HOME
    "[6n", // DSR
    "[s", // SCP
    "[u", // RCP
    "[?25l", // HIDE
    "[?25h", // SHOW
};


t_screen Screen;
static const char *RAW_SCREEN_CODES[NB_SCREEN] = {
    "[2J", // CLEAR
    "[3J", // CLEAR_BUFFER
    "[K", // LINE_ERASE_CUR
    "[2K", // LINE_ERASE_ALL
};


char *custom_code(unsigned char code) {
    return gc_asprintf("%s[%dm", code);
}


char *cursor_cup(uint16_t row, uint16_t column) {
    if (!(1 <= row && row <= 999)) return NULL;
    if (!(1 <= column && column <= 999)) return NULL;

    return gc_asprintf2("%s[%d;%dH", row, column);
}


char *cursor_cuu(uint16_t n) {
    if (!(1 <= n && n <= 999)) return NULL;

    return gc_asprintf("%s[%dA", n);
}


char *cursor_cud(uint16_t n) {
    if (!(1 <= n && n <= 999)) return NULL;
    
    return gc_asprintf("%s[%dB", n);
}


char *cursor_cuf(uint16_t n) {
    if (!(1 <= n && n <= 999)) return NULL;
    
    return gc_asprintf("%s[%dC", n);
}


char *cursor_cub(uint16_t n) {
    if (!(1 <= n && n <= 999)) return NULL;
    
    return gc_asprintf("%s[%dD", n);
}


char *fore_color8(uint8_t color) {
    return gc_asprintf("%s[38;5;%dm", color);
}


char *back_color8(uint8_t color) {
    return gc_asprintf("%s[48;5;%dm", color);
}


char *underline_color8(uint8_t color) {
    return gc_asprintf("%s[58;5;%dm", color);
}


char *fore_color24(uint8_t r, uint8_t g, uint8_t b) {
    return gc_asprintf3("%s[38;2;%d;%d;%dm", r, g, b);
}

char *back_color24(uint8_t r, uint8_t g, uint8_t b) {
    return gc_asprintf3("%s[48;2;%d;%d;%dm", r, g, b);
}

char *underline_color24(uint8_t r, uint8_t g, uint8_t b) {
    return gc_asprintf3("%s[58;2;%d;%d;%dm", r, g, b);
}


void init_fore(void) {
    /* Init Fore */
    for (int i = 0; i < NB_FORE_COLORS; i++) {
        snprintf(Fore.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_FORE_CODES[i]);
    }
}


void init_back(void) {
    /* Init Back */
    for (int i = 0; i < NB_BACK_COLORS; i++) {
        snprintf(Back.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_BACK_CODES[i]);
    }
}


void init_style(void) {
    /* Init Style */
    for (int i = 0; i < NB_STYLE; i++) {
        snprintf(Style.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_STYLE_CODES[i]);
    }
    
    Style.RESET_ALL = gc_reset;
}


void init_disable(void) {
    /* Init Disable */
    for (int i = 0; i < NB_DISABLE; i++) {
        snprintf(Disable.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_DISABLE_CODES[i]);
    }
}


void init_default(void) {
    /* Init Default */
    for (int i = 0; i < NB_DEFAULT; i++) {
        snprintf(Default.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_DEFAULT_CODES[i]);
    }
}


void init_font(void) {
    /* Init Font */
    for (int i = 0; i < NB_FONT; i++) {
        snprintf(Font.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_FONT_CODES[i]);
    }
}


void init_misc(void) {
    /* Init Misc */
    for (int i = 0; i < NB_MISC; i++) {
        snprintf(Misc.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_MISC_CODES[i]);
    }
}


void init_cursor(void) {
    /* Init Cursor */
    for (int i = 0; i < NB_CURSOR; i++) {
        snprintf(Cursor.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_CURSOR_CODES[i]);
    }
}


void init_screen(void) {
    /* Init Screen */
    for (int i = 0; i < NB_SCREEN; i++) {
        snprintf(Screen.array[i], COLOR_STR_SIZE, "%s%s", get_ansi_esc_char(), RAW_SCREEN_CODES[i]);
    }
}


void init_all(void) {
    init_fore();
    init_back();
    init_style();
    init_disable();
    init_default();
    init_font();
    init_misc();
    init_cursor();
    init_screen();
}


typedef void (*InitFunction)(void);

typedef struct {
    ColorInitFlags flag;
    InitFunction function;
} InitEntry;


const InitEntry init_map[] = {
    {COLOR_FLAG_INIT_FORE, init_fore},
    {COLOR_FLAG_INIT_BACK, init_back},
    {COLOR_FLAG_INIT_STYLE, init_style},
    {COLOR_FLAG_INIT_DISABLE, init_disable},
    {COLOR_FLAG_INIT_DEFAULT, init_default},
    {COLOR_FLAG_INIT_FONT, init_font},
    {COLOR_FLAG_INIT_MISC, init_misc},
    {COLOR_FLAG_INIT_CURSOR, init_cursor},
    {COLOR_FLAG_INIT_SCREEN, init_screen},
    {COLOR_FLAG_INIT_ALL, init_all}
};


void init_color(const char *o_ansi_esc_char, const unsigned char o_cursor_auto_show, const unsigned char o_auto_clean, const unsigned char o_intercept_sig, int o_flags) {
    g_ansi_esc_char = (o_ansi_esc_char) ? o_ansi_esc_char : "\033";
    g_cursor_auto_show = o_cursor_auto_show;
    g_auto_clean = o_auto_clean;

    for (size_t i = 0; i < (sizeof(init_map) / sizeof(init_map[0])); i++) {
        if (o_flags & init_map[i].flag) {
            init_map[i].function();
        }
    }
    
    if (o_intercept_sig) {
        setup_signals();
    }
    
    if (o_auto_clean) {
        atexit(gc_clean_all);
        atexit(auto_clean);
    }
}


void __attribute__((constructor)) auto_init(void) {
    init_color(NULL, 1, 1, 1, COLOR_FLAG_INIT_DEFAULT);
}


void color_support_test(void) {
    printf("%s%s%s%s", Screen.CLEAR, Screen.CLEAR_BUFF, Screen.LINE_ERASE_ALL, Screen.LINE_ERASE_CUR);
    
    printf("%s=== FULL RENDER TEST OF THE LIB ===%s\n\n", Style.BOLD, Style.RESET);

    printf("%s--- Basic Colors---\n", Style.RESET);
    
    const char *color_names[] = {"BLACK", "RED", "GREEN", "YELLOW", "BLUE", "MAGENTA", "CYAN", "WHITE"};

    printf("       ");
    for (int i = 0; i < 8; i++) printf(" %-7s", color_names[i]);
    printf("\n");

    printf("Classic");
    for (int i = 0; i < 8; i++) {
        printf("%s %-7s%s", Fore.array[i], "Text", Style.RESET);
    }
    printf("\n");

    printf("Bright ");
    for (int i = 8; i < 16; i++) {
        printf("%s %-7s%s", Fore.array[i], "Text", Style.RESET);
    }
    printf("\n\n");

    printf("Back   ");
    for (int i = 0; i < 8; i++) {
        printf("%s%s %-7s%s", Fore.BLACK, Back.array[i], "Text", Style.RESET);
    }
    printf("\n");
    
    printf("Back Br");
    for (int i = 8; i < 16; i++) {
        printf("%s%s %-7s%s", Fore.BLACK, Back.array[i], "Text", Style.RESET);
    }
    printf("\n\n");

    printf("%s--- Styles ---\n", Style.RESET);
    
    struct { const char *code; const char *name; } styles[] = {
        {Style.BOLD, "BOLD"},
        {Style.DIM, "DIM"},
        {Style.ITALIC, "ITALIC"},
        {Style.UNDERLINE, "UNDERLINE"},
        {Style.BLINK, "BLINK"},
        {Style.REVERSE, "REVERSE"},
        {Style.HIDDEN, "HIDDEN"},
        {Style.STRIKETHROUGH, "STRIKE"},
        {Style.UNDERLINE_DOUBLE, "DOUBLE UL"}
    };

    for (int i = 0; i < 9; i++) {
        printf("%s%s%s | ", styles[i].code, styles[i].name, Style.RESET);
        if ((i + 1) % 3 == 0) printf("\n");
    }
    printf("\n\n");

    printf("%s--- Fonts ---\n", Style.RESET);
    for (int i = 0; i < NB_FONT; i++) {
        printf("%sPolice %d%s  ", Font.array[i], i + 1, Style.RESET);
    }
    printf("\n\n");

    printf("%s--- 8-Bit Colors (Compact) ---\n", Style.RESET);
    for (int i = 0; i < 256; i++) {
        printf("%s%s ", back_color8(i), (i % 32 == 0) ? "\n" : ""); 
        if (i == 0) printf(" ");
    }
    printf("%s\n\n", Style.RESET);

    printf("Test %s%sUnderline 8-Bit%s\n\n", underline_color8(60), Style.UNDERLINE, Style.RESET);

    printf("%s--- TrueColor Gradients (RGB) ---\n", Style.RESET);

    printf("Fore : ");
    int r = 255, g = 0, b = 0;
    for (g = 0; g <= 255; g += 5) {
        printf("%s█%s", fore_color24(r, g, b), Style.RESET);
    }
    printf("\n");

    printf("Fore : ");
    r = 0, g = 255, b = 255;
    for (g = 255; g >= 0; g -= 5) {
        printf("%s█%s", fore_color24(r, g, b), Style.RESET);
    }
    printf("\n");

    printf("Back : ");
    r = 0, g = 0, b = 255;
    for (r = 0; r <= 255; r += 5) {
        printf("%s %s", back_color24(r, g, b), Style.RESET);
    }
    printf("\n");

    printf("Back : ");
    for (int i = 0; i <= 255; i += 5) {
        printf("%s %s", back_color24(0, 255 - i, i), Style.RESET);
    }
    printf("\n");

    printf("\nTest %s%sUnderline 24-Bit%s\n\n", underline_color24(255, 0, 255), Style.UNDERLINE, Style.RESET);

    printf("%s--- Cursor And Animation ---\n", Style.RESET);
    fflush(stdout);
    
    printf("%s", Cursor.HIDE);
    printf("%s", Cursor.SCP); 
    
    printf("\n");
    printf("%s", Fore.CYAN);
    printf("+------------+\n");
    printf("| Loading... |\n");
    printf("+------------+\n");
    printf("%s", Style.RESET);

    printf("%s", cursor_cuu(2)); 
    printf("%s", cursor_cuf(2)); 

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000000;

    for (int i = 0; i < 10; i++) {
        printf("%s", Back.GREEN);
        printf(" ");
        fflush(stdout);
        nanosleep(&ts, NULL);
    }

    printf("%s", Cursor.RCP); 
    printf("%s", cursor_cud(4));
    printf("%s", Cursor.SHOW);

    printf("Animation Ended.\n");

    printf("%s", (char *)Style.RESET_ALL()); 
    printf("\n=== END OF TEST ===\n");
}