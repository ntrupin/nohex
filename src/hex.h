#ifndef HEX_H
#define HEX_H

#include <stdio.h>

// public functions

/**
 * \brief Outputs the hex dump of file
 * \param fname The name of the file to dump
 * \param bytes
 *     The number of bytes to read. If bytes is
 *     greater than the length of file, the
 *     whole file will be read.
 * \param cols The number of columns per output row
 * \return 1 on success, 0 on failure
 */
int output_hex(const char *, long, int);

#endif
