% HEX(1) User Manual
% Noah Trupin
% July 30, 2023

# NAME

nohex - Noah's hexdump utility

# SYNOPSIS

nohex [*options*] [*input-file*]...

# DESCRIPTION

Nohex takes an input file (and assorted options) and produces
a table containing a hexdump alongside a textual representation
of the file.

If no options are provided, Nohex defaults to dumping the entire
provided *input-file* with 16 bytes per line.

# OPTIONS

\--help
:   Display the help menu.

\--no-color
:   Disabled colored output. By default, Nohex uses 8-bit terminal
    colors to enhance output.

-n *INTEGER*, \--bytes *INTEGER*
:   Specify the number of bytes to read and dump from *input-file*.
    Nohex defaults to dumping the entire file.

-c *INTEGER*, \--cols=*INTEGER*
:   Specify the number of bytes to print per output row. Nohex defaults
    to 16 columns per row.

# SEE ALSO

The source code for Nohex at <https://github.com/ntrupin/nohex>
