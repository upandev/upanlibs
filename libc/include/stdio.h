/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa_prajwal@yahoo.co.in'
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
#ifndef _STDIO_H
#define _STDIO_H 1

# include <ctype.h>
# include <stdarg.h>

#if defined __cplusplus
extern "C" {
#endif

extern void SysDisplay_ClearScreen() ;

#define __UCLIBC_HAS_FLOATS__ 1

#define clrscr()			SysDisplay_ClearScreen()

#define OFFSET_TYPE	long int

# define P_tmpdir	"/tmp"

typedef struct __STDIO_FILE_STRUCT FILE;

typedef struct
{
	char* buf ;
	int pos ;
} STDBUF ;

typedef void cecho_func(char ch, void*) ;

//extern FILE *stdin ;
//extern FILE *stdout ;
//extern FILE *stderr ;

# include <sys/types.h>

# include <features.h>

// #if !defined __need_FILE && !defined __need___FILE

# define __need_size_t
# define __need_NULL
# include <stddef.h>

//# define __need_FILE
//# define __need___FILE

#ifndef L_vfprintf
#define L_vfprintf 1
#define VFPRINTF vfprintf
#define FMT_TYPE char
#endif 

#ifndef L_scanf
#define L_scanf
#endif

#ifndef L_vfscanf
#define L_vfscanf
#endif

//#ifndef NL_ARGMAX
//#define NL_ARGMAX 9
//#endif
//
//#ifndef L_vsscanf
//#define L_vsscanf
//#endif

#define EOF -1

#define STDIN_FD	0
#define STDOUT_FD	1
#define STDERR_FD	2

#define BUFSIZ 1024

#define IS_STD_STREAM(S) (S->__filedes == 0 || S->__filedes == 1 || S->__filedes == 2)

int printf(const char * __restrict format, ...) ;
int snprintf(char *__restrict buf, size_t size, const char * __restrict format, ...) ;
int asprintf(char **__restrict buf, const char * __restrict format, ...);
OFFSET_TYPE ftell(register FILE *stream) ;
int fseek(register FILE *stream, OFFSET_TYPE offset, int whence) ;

int sprintf(char *__restrict buf, const char * __restrict format, ...) ;
int scanf(const char * __restrict format, ...) ;
int sscanf(const char * __restrict str, const char * __restrict format, ...) ;
int vsscanf(const char* __restrict buf, const char * __restrict format, va_list arg) ;
int fprintf(FILE * __restrict stream, const char * __restrict format, ...) ;
int fgetc(register FILE *stream) ;
int getc(register FILE *stream) ;
char* fgets(char *__restrict s, int n, register FILE * __restrict stream) ;

void putsl(const char* s, size_t len) ;

int fflush(FILE *stream);
FILE *fopen (__const char *__restrict __filename, __const char *__restrict __modes);
int fileno(register FILE *stream) ;
int fputs(register const char * __restrict s, FILE * __restrict stream) ;
int puts(register const char * __restrict s) ;
int fputc(int c, register FILE *stream) ;
int putc(int c, register FILE *stream) ;
size_t fwrite(const void * __restrict ptr, size_t size, size_t nmemb, register FILE * __restrict stream) ;
size_t fread(void * __restrict ptr, size_t size, size_t nmemb, FILE * __restrict stream) ;
int fclose(register FILE *stream) ;
int remove(register const char *filename) ;

extern int feof(register FILE *stream) ;
extern int vfprintf (FILE * __restrict stream, register const FMT_TYPE * __restrict format, va_list arg) ;
extern int vsprintf(char *__restrict buf, const char * __restrict format, va_list arg) ;
extern void perror(register const char *s) ;
extern int vsnprintf(char *__restrict buf, size_t size, const char * __restrict format, va_list arg) ;
extern int vprintf(const char * __restrict format, va_list arg);
void setbuf(FILE * __restrict stream, register char * __restrict buf) ;

OFFSET_TYPE ftell(register FILE *stream) ;
int fseek(register FILE *stream, OFFSET_TYPE offset, int whence) ;

# include <bits/uClibc_stdio.h>
# include <bits/stdio_lim.h>
# include <cdisplay.h>

typedef __STDIO_fpos_t fpos_t;

#define _IOFBF __STDIO_IOFBF 		/* Fully buffered.  */
#define _IOLBF __STDIO_IOLBF		/* Line buffered.  */
#define _IONBF __STDIO_IONBF		/* No buffering.  */

FILE* _stdio_get_stdin() ;
FILE* _stdio_get_stdout() ;
FILE* _stdio_get_stderr() ;

#define stdin	_stdio_get_stdin()
#define stdout	_stdio_get_stdout()
#define stderr	_stdio_get_stderr()

extern int vsscanf_cecho(cecho_func* cechoFunc, void* private_data, const char* __restrict buf, const char * __restrict format, va_list arg) ;
extern int scanf_cecho(cecho_func* cecho_func, void* private_data, const char * __restrict format, ...) ;

extern void _stdio_init(void) ;
extern void _stdio_term(void) ;

void rewind(register FILE *stream) ;
int ferror(register FILE *stream) ;

int close(int fd) ;
int getc_unlocked(FILE *stream) ; 
int ungetc(int c, register FILE *stream) ;
int setvbuf(register FILE * __restrict stream, register char * __restrict buf, int mode, size_t size) ;
FILE *fdopen(int filedes, const char *mode) ;
void uiwritec(const char ch, unsigned uiAttr, bool bUpdateCursorOnScreen);
void uiwritea(const MChar* src, uint32_t rows, uint32_t cols, int curPos);

#if defined __cplusplus
}
#endif

#endif
