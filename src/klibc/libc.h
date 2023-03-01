#ifndef LIBC_H
#define LIBC_H

#include <stddef.h>

//// ctype.h ////

/// @brief  test for an alphanumeric character
/// @return non-zero if c is the thing, 0 if it isn't
int isalnum(int c);

/// @brief  test for an alphabetical character
/// @return non-zero if c is the thing, 0 if it isn't
int isalpha(int c);

/// @brief  test for a control character
/// @return non-zero if c is the thing, 0 if it isn't
int iscntrl(int c);

/// @brief  test for a digit (0-9)
/// @return non-zero if c is the thing, 0 if it isn't
int isdigit(int c);

/// @brief  test for lowercase
/// @return non-zero if c is the thing, 0 if it isn't
int islower(int c);

/// @brief  test if printable
/// @return non-zero if c is the thing, 0 if it isn't
int isprint(int c);

/// @brief  test for space (space, form-feed ('\f'), newline ('\n'), carriage
/// return ('\r'), horizontal tab ('\t'), and vertical tab ('\v'))
/// @return non-zero if c is the thing, 0 if it isn't
int isspace(int c);

/// @brief  test for uppercase
/// @return non-zero if c is the thing, 0 if it isn't
int isupper(int c);

/// @brief  test for hexadecimal digit
/// @return non-zero if c is the thing, 0 if it isn't
int isxdigit(int c);

/// @brief test for a 7-bit US-ASCII character
/// @return non-zero if c is the thing, 0 if it isn't
int isascii(int c);

/// @brief test for a blank character (tab or space)
/// @return non-zero if c is the thing, 0 if it isn't
int isblank(int c);

//// strings.h ////

/// @brief convert a string to an int
/// @param nptr pointer to a string that starts with a number
/// @return converted value or 0 on error
int atoi(const char *nptr);

/// @brief zero a byte string
/// @param s location to zero
/// @param n numbers of bytes to zero
void bzero(void *s, size_t n);

/// @brief fill memory with a constant byte
/// @param s memory area to fill
/// @param c character to fill
/// @param n number of bytes to fill
/// @return pointer to the memory area s
void *memset(void *s, int c, size_t n);

/// @brief returns the length of a string
/// @param s string to get the size of
/// @return size of the string
size_t strlen(const char *s);

/// @brief find character in string
/// @param s string to find the character c in
/// @param c character to find
/// @return return a pointer to the matched character or NULL if the character
/// is not found. The terminating null byte is considered part of the string,
/// so that if c is specified as '\0', these functions return a pointer to the
/// terminator.char *
char *strchr(const char *s, int c);

//// strings nonstandard //

/// @brief Convert integer to string
/// @param s destination, should be long enough to contain any value in the
/// selected base (65)
/// @param value integer to convert
/// @param base base to convert to
/// @return pointer to str on success, NULL with a non-valid base argument
char *itoa(char *str, int value, unsigned char base);

#endif
