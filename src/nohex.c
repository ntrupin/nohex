#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "flags.h"

static nohex_flags *flags;

// box characters
static struct box_chars {
    char *top_left;
    char *top_right;
    char *bottom_left;
    char *bottom_right;
    char *horizontal;
    char *vertical;
    char *vertical_dash;
} box_chars;

// ansi colors
#define RESET   "\e[0m"
#define DEFAULT "\e[0;00m"
#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define CYAN    "\e[0;36m"

// print ch to stdout with color
static inline void putcolor(unsigned char ch, const char *color) {
    if (flags->octal) 
        printf("%s%03O%s ", flags->nocolor ? DEFAULT : color, ch, RESET);
    else
        printf("%s%02X%s ", flags->nocolor ? DEFAULT : color, ch, RESET);
}

// print ch to file buffer with color
static inline void fputcolor(FILE *f, char ch, const char *color) {
    fprintf(f, "%s%c%s", flags->nocolor ? DEFAULT : color, ch, RESET);
}

// print full-length border with start and end chars
static inline void putboxend(const char *start, const char *end, int length) { 
    printf("%s", start);
    for (int i=0;i<length;i++) {
        printf(flags->ascii ? "-" : "─");
    }
    printf("%s", end);
}

// check if ch is ascii whitespace
static inline int is_whitespace(unsigned char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\r'
            || ch == '\n' || ch == '\v' || ch == '\f');
}

// check if ch is ascii
static inline int is_ascii(unsigned char ch) {
    return (ch >= 0x00 && ch <= 0x7F);
}

int output_hex(const char *fname, nohex_flags *_flags) {
    flags = _flags;

    // box drawing chars
    box_chars = flags->ascii ? (struct box_chars){
        .top_left = "-",
        .top_right = "-",
        .bottom_left = "-",
        .bottom_right = "-",
        .horizontal = "-",
        .vertical = "|",
        .vertical_dash = "|"
    } : (struct box_chars) {
        .top_left = "┌",
        .top_right = "┐",
        .bottom_left = "└",
        .bottom_right = "┘",
        .horizontal = "─",
        .vertical = "│",
        .vertical_dash = "┆"
    };

    // open file, check for error
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
       fprintf(stderr, "failed to open file '%s'", fname);
       return 0;
    }

    // get file size
    fseek(fp, 0L, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    // byte to read into
    unsigned char ch;

    // byte count trackers
    int bytes_read = 0;
    int excess;

    // skip bytes
    if (flags->skip && fseek(fp, flags->skip, SEEK_SET) != 0) {
        fprintf(stderr, "failed to skip %d bytes", flags->skip);
        fclose(fp);
        return 0;
    } else {
        // setup offset
        if (flags->length < INT_MAX - flags->skip)
            flags->length += flags->skip;
        else
            flags->length = INT_MAX;

        bytes_read += flags->skip;
    }

    // adjust fsize for smaller buffer
    if (flags->length < fsize || fsize == 0)
        fsize = flags->length;

    // temp buffer
    char buffer[flags->cols * 12];
    FILE *bufptr = fmemopen(buffer, sizeof(buffer), "wb");

    // border length
    int border_len = flags->cols * (flags->octal ? 5 : 4) + 4 + (flags->offset ? 10 : 0);

    putboxend(box_chars.top_left, box_chars.top_right, border_len);

    // print skipped bytes
    if ((excess = bytes_read % flags->cols) > 0) {
        printf("\n%s %07X %s %*s", 
                // offset
                box_chars.vertical, bytes_read - excess, box_chars.vertical_dash,
                // spacing
                excess * 3, "");
        for (int i=0;i<excess;i++)
            fputcolor(bufptr, ' ', DEFAULT);
    }

    while (fread(&ch, sizeof(ch), 1, fp) == 1 
            && bytes_read < flags->length) {

        if (bytes_read % flags->cols == 0) {
            printf("\n%s ", box_chars.vertical);

            // show offset if flag set
            if (flags->offset)
                printf("%07X %s ", bytes_read, box_chars.vertical_dash);
        }

        if (ch == 0x00) {
            // null character
            putcolor(ch, DEFAULT);
            fputcolor(bufptr, '0', DEFAULT);
        } else if (ch >= 0x21 && ch <= 0x7E) {
            // printable chars
            putcolor(ch, CYAN);
            fputcolor(bufptr, ch, CYAN);
        } else if (is_whitespace(ch)) {
            // whitespace character
            putcolor(ch, GREEN);
            fputcolor(bufptr, '_', GREEN);
        } else if (is_ascii(ch)) {
            // other ascii
            putcolor(ch, RED);
            fputcolor(bufptr, '*', RED);
        } else {
            // non-ascii
            putcolor(ch, YELLOW);
            fputcolor(bufptr, 'x', YELLOW);
        }

        bytes_read += 1;

        // if end of col, output text
        if (bytes_read % flags->cols == 0 || bytes_read == fsize) {
            excess = bytes_read % flags->cols;
            if (excess == 0)
                excess = flags->cols;

            printf("%*s%s %.*s%*s %s",
                    // padding after bytes
                    (flags->cols - excess) * (flags->octal ? 4 : 3), "",
                    // dash
                    box_chars.vertical_dash,
                    // number of characters
                    excess * 12, buffer,
                    // padding after characters
                    (flags->cols - excess), "",
                    // end bar
                    box_chars.vertical);

            rewind(bufptr);
        }
    }
    putc('\n', stdout);
    putboxend(box_chars.bottom_left, box_chars.bottom_right, border_len);
    putc('\n', stdout);

    // close and return success
    fclose(fp);
    fclose(bufptr);
    return 1;
}
