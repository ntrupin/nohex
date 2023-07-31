#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>
#include "flags.h"
#include "nohex.h"

static nohex_flags flags;

// command line options
static struct option long_options[] = {
    // show help menu
    { "help", no_argument, &(flags.help), 1 },
    // show version
    { "version", no_argument, &(flags.version), 1 },
    // turn off colors in output
    { "no-color", no_argument, &(flags.nocolor), 1 },
    // bytes to read
    { "length", required_argument, 0, 'n' },
    // columns per row
    { "cols", required_argument, 0, 'c' },
    // show offset
    { "offset", no_argument, &(flags.offset), 1 },
    // use ascii box chars
    { "ascii", no_argument, &(flags.ascii), 1 },
    // skip bytes
    { "skip", required_argument, 0, 1 }
};

// read an integer from optarg and check for errors
int read_int(char *optarg, int *x) {
    char *endptr = NULL;
    long tmp;

    // set errno before strtol
    errno = 0;
    tmp = strtol(optarg, &endptr, 0);

    // check for strtol errors
    if (endptr == optarg || *endptr != '\0' 
        || ((tmp == LONG_MIN || tmp == LONG_MAX) && errno == ERANGE)) {
        fprintf(stderr, "option expected an integer, got '%s' instead", optarg);
        return 0;
    }

    if (tmp > INT_MAX || tmp < INT_MIN) {
        fprintf(stderr, "provided value '%lu' exceeds integer bounds", tmp);
        return 0;
    }

    *x = (int)tmp;
    return 1;
}

void output_help() {
    char *text = "\n                Noah's Hexdump                \n"
                 "==============================================\n"
                 "\n"
                 "version "NOHEX_VERSION"\n"
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
                 "  --version           show nohex version\n"
                 "  --length, -n <int>  # of bytes to output\n"
                 "  --cols, -c <int>    # of cols per output row\n"
                 "  --no-color          print without color\n"
                 "  --offset            show input offset\n"
                 "  --ascii             use ascii box characters\n";
    printf("%s\n", text);
}

int main(int argc, char **argv) {
    int f, option_index;
    flags.length = INT_MAX;
    flags.cols   = 16;
    flags.skip   = 0;
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
                    if (flags.help) {
                        output_help();
                        exit(0);
                    } else if (flags.version) {
                        printf("%s\n", NOHEX_VERSION);
                        exit(0);
                    }
                    break;
                }
                break;

            // --length, -n
            case 'n':
                if (!read_int(optarg, &(flags.length)))
                    exit(1);
                break;

            // --cols, -c
            case 'c':
                if (!read_int(optarg, &(flags.cols)))
                    exit(1);
                break;

            // --skip, -s
            case 's':
                if (!read_int(optarg, &(flags.skip)))
                    exit(1);
                break;

            case '?':
                output_help();
                exit(1);
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

    output_hex(fname, &flags);

    return 0;
}
