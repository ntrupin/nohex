#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>
#include "flags.h"
#include "hex.h"

static nohex_flags flags;

// command line options
static struct option long_options[] = {
    // show help menu
    { "help", no_argument, &(flags.help), 1 },
    // turn off colors in output
    { "no-color", no_argument, &(flags.nocolor), 1 },
    // bytes to show in output
    { "length", required_argument, 0, 'n' },
    // columns per row
    { "cols", required_argument, 0, 'c' },
    // show offset
    { "offset", no_argument, &(flags.offset), 1 },
    // use ascii box chars
    { "ascii", no_argument, &(flags.ascii), 1 }
};

// read a long from optarg and check for errors
int read_long(char *optarg, long *x) {
    char *endptr = NULL;

    // set errno before strtol
    errno = 0;
    *x = strtol(optarg, &endptr, 0);

    // check for strtol errors
    if (endptr == optarg || *endptr != '\0' 
        || ((*x == LONG_MIN || *x == LONG_MAX) && errno == ERANGE)) {
        fprintf(stderr, "option expected an integer, got '%s' instead", optarg);
        return 0;
    }
    return 1;
}

void output_help() {
    char *text = "                Noah's Hexdump                \n"
                 "==============================================\n"
                 "\n"
                 "A simple hexdump utility.\n"
                 "\n"
                 "For more info see `man docs/hex.1`\n"
                 "\n"
                 "Usage: hex <options> <filename>\n"
                 "----------------------------------------------\n"
                 "\n"
                 "available options:\n"
                 "  --help              show this menu\n"
                 "  --length, -n <int>  # of bytes to output\n"
                 "  --cols, -c <int>    # of cols per output row\n"
                 "  --no-color          print without color\n"
                 "  --offset            show input offset\n"
                 "  --ascii             use ascii box characters\n";
    printf("%s\n", text);
}

int main(int argc, char **argv) {
    int f, option_index;
    long bytes = LONG_MAX,
         cols  = 16;
    char *endptr = NULL, 
         *fname  = NULL;

    while (1) {
        option_index = 0;
        f = getopt_long(argc, argv, "n:c:", long_options, &option_index);

        // end of options
        if (f == -1)
            break;

        switch (f) {
            case 0:
                if (long_options[option_index].flag != 0) {
                    if (flags.help)
                        output_help();
                    break;
                }
                break;

            case 'n':
                if (!read_long(optarg, &bytes)) {
                    fprintf(stderr, "bytes option expected an integer, got '%s' instead", optarg);
                    exit(1);
                }
                break;

            case 'c':
                if (!read_long(optarg, &cols)) {
                    fprintf(stderr, "columns option expected an integer, got '%s' instead", optarg);
                    exit(1);
                }

                if (cols > INT_MAX || cols < INT_MIN) {
                    fprintf(stderr, "columns option expected an integer, got long '%lu' instead", cols);
                    exit(1);
                }
                break;

            case '?':
                output_help();
                break;

            default:
                abort();
        }
    }

    // one remaining option
    if (optind == argc - 1) {
        fname = argv[optind];
    } else {
        fprintf(stderr, "No filename provided\n");
        exit(1);
    }

    output_hex(fname, bytes, cols, &flags);

    return 0;
}
