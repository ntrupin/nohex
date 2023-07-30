#include <stdio.h>
#include <string.h>

// ansi colors
#define RESET   "\e[0m"
#define DEFAULT "\e[0;00m"
#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define CYAN    "\e[0;36m"

static inline void putcolor(unsigned char ch, const char *color) {
    printf("%s%02X%s ", color, ch, RESET);
}

static inline void fputcolor(FILE *f, char ch, const char *color) {
    fprintf(f, "%s%c%s", color, ch, RESET);
}

static inline void putboxend(const char *start, const char *end, int length) { 
    printf("%s", start);
    for (int i=0;i<length;i++) {
        printf("─");
    }
    printf("%s", end);
}

static inline int is_whitespace(unsigned char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\r'
            || ch == '\n' || ch == '\v' || ch == '\f');
}

static inline int is_ascii(unsigned char ch) {
    return (ch >= 0x00 && ch <= 0x7F);
}

int output_hex(const char *fname, long bytes, int cols) {

    // open file, check for error
    FILE *fp;
    if ((fp = fopen(fname, "rb")) == NULL) {
       fprintf(stderr, "failed to open file '%s'", fname);
       return 0;
    }

    // byte to read into
    unsigned char ch;

    // temp buffer
    char buffer[cols * 12];
    FILE *bufptr = fmemopen(buffer, sizeof(buffer), "wb");

    long bytes_read = 0;
    int excess;

    putboxend("┌", "┐", cols * 4 + 4);
    while (fread(&ch, sizeof(ch), 1, fp) == 1 
            && bytes_read < bytes) {

        if (bytes_read % cols == 0)
            printf("\n│ ");

        switch (ch) {
            // null character
            case 0x00:
                putcolor(ch, DEFAULT);
                fputcolor(bufptr, '0', DEFAULT);
                break;

            // printable chars
            case 0x21 ... 0x7E:
                putcolor(ch, CYAN);
                fputcolor(bufptr, ch, CYAN);
                break;

            // handle others
            default:
                if (is_whitespace(ch)) {
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
                break;
        }

        bytes_read += 1;

        // if end of col, output text
        if (bytes_read % cols == 0 || bytes_read == bytes) {
            excess = bytes_read % cols;

            printf("%*s┆ %.*s%*s │",
                    // padding after bytes
                    excess * 3, "",
                    // number of characters
                    (cols - excess) * 12, buffer,
                    // padding after characters
                    excess, "");

            rewind(bufptr);
        }
    }
    putboxend("\n└", "┘", cols * 4 + 4);

    // close and return success
    fclose(fp);
    fclose(bufptr);
    return 1;
}
