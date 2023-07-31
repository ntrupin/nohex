#ifndef NOHEX_H
#define NOHEX_H

#include <stdio.h>
#include "flags.h"

#define NOHEX_VERSION "2023.07.31"

// public functions

/**
 * \brief Outputs the hex dump of file
 * \param fname The name of the file to dump
 * \param flags Function behavior flags
 * \return 1 on success, 0 on failure
 */
int output_hex(const char *, nohex_flags *);

#endif
