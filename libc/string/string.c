/*
 * This string-include defines all string functions as inline
 * functions. Use gcc. It also assumes ds=es=data space, this should be
 * normal. Most of the string-functions are rather heavily hand-optimized,
 * see especially strtok,strstr,str[c]spn. They should work, but are not
 * very easy to understand. Everything is done entirely within the register
 * set, making the functions fast and clean. String instructions have been
 * used through-out, making for "slightly" unclear code :-)
 *
 *		NO Copyright (C) 1991, 1992 Linus Torvalds,
 *		consider these trivial functions to be PD.
 *
 * Modified for uClibc by Erik Andersen <andersen@codepoet.org>
 * These make no attempt to use nifty things like mmx/3dnow/etc.
 * These are not inline, and will therefore not be as fast as
 * modifying the headers to use inlines (and cannot therefore
 * do tricky things when dealing with const memory).  But they
 * should (I hope!) be faster than their generic equivalents....
 *
 * More importantly, these should provide a good example for
 * others to follow when adding arch specific optimizations.
 *  -Erik
 */

#define _STDIO_UTILITY
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#undef TOLOWER
#define TOLOWER(C)     ((C)|0x20)

void strtok_m(const char* src, char chToken, tok_copy_func* tc_copy, int* iListSize)
{
    int index ;
    int iStartIndex = 0 ;
    int iTokenIndex = 0 ;

    for(index = 0; src[index] != '\0'; index++)
    {
        if(src[index] == chToken)
        {
            if((index - iStartIndex) > 0)
            {
				tc_copy(iTokenIndex, src + iStartIndex, (index - iStartIndex), NULL) ;
                iTokenIndex++ ;
            }

            iStartIndex = index + 1 ;
        }
    }

    if((index - iStartIndex) > 0)
    {
		tc_copy(iTokenIndex, src + iStartIndex, (index - iStartIndex), NULL) ;
        iTokenIndex++ ;
    }

    *iListSize = iTokenIndex ;
}

void strtok_c(const char* src, 
			tok_compare_func* tc_tok_func, 
			tok_group_func* tc_group_func, 
			tok_copy_func* tc_copy, 
			int* iListSize,
			void* tc_copy_context)
{
    int index ;
    int iStartIndex = 0 ;
    int iTokenIndex = 0 ;
	  bool bGroupStarted = false;

    for(index = 0; src[index] != '\0'; index++)
    {
		if(tc_group_func(src[index]))
		{
            if((index - iStartIndex) > 0)
            {
				tc_copy(iTokenIndex, src + iStartIndex, (index - iStartIndex), tc_copy_context) ;
                iTokenIndex++ ;
            }

			if(!bGroupStarted)
				bGroupStarted = true;
			else
				bGroupStarted = false;
			
			iStartIndex = index + 1 ;
			continue ;
		}

		if(bGroupStarted)
			continue ;

        if(tc_tok_func(src[index]))
        {
            if((index - iStartIndex) > 0)
            {
				tc_copy(iTokenIndex, src + iStartIndex, (index - iStartIndex), tc_copy_context) ;
                iTokenIndex++ ;
            }

            iStartIndex = index + 1 ;
        }
    }

    if((index - iStartIndex) > 0)
    {
		tc_copy(iTokenIndex, src + iStartIndex, (index - iStartIndex), tc_copy_context) ;
        iTokenIndex++ ;
    }

    *iListSize = iTokenIndex ;
}

int strcnt(const char* str, const char ch)
{
	int iCnt = 0 ;
	int i ;

	for(i = 0; str[i] != '\0'; i++)
		if(str[i] == ch)
			iCnt++ ;

	return iCnt ;
}

char *strpbrk(const char *s1, const char *s2)
{
	register const char *s;
	register const char *p;

	for ( s=s1 ; *s ; s++ ) {
		for ( p=s2 ; *p ; p++ ) {
			if (*p == *s) return (char *) s; /* silence the warning */
		}
	}
	return NULL;
}

char* strdup(const char* s)
{
	char* ns = (char*)malloc(strlen(s)) ;
	strcpy(ns, s) ;
	return ns ;
}

int strcasecmp(register const char *s1, register const char *s2)
{
	int r = 0;

	while ( ((s1 == s2) || !(r = ((int)( TOLOWER(*((unsigned char *)s1)))) - TOLOWER(*((unsigned char *)s2)))) && (++s2, *s1++));

	return r;
}

char* strtok(char * __restrict s1, const char * __restrict s2)
{
	static char *next_start = NULL ;	/* Initialized to 0 since in bss. */
	return strtok_r(s1, s2, &next_start);
}

int strncasecmp(register const char *s1, register const char *s2, size_t n)
{
	int r = 0;

	while ( n
			&& ((s1 == s2) ||
				!(r = ((int)( TOLOWER(*((unsigned char *)s1))))
				  - TOLOWER(*((unsigned char *)s2))))
			&& (--n, ++s2, *s1++));
	return r;
}

void* mempcpy(void * s1, const void * s2, size_t n)
{
	register char *r1 = s1;
	register const char *r2 = s2;

	while (n) {
		*r1++ = *r2++;
		--n;
	}
	return r1;
}

void strreverse(char* str)
{	
	unsigned len = strlen(str) ;
	unsigned i ;
	char temp ;

	for(i = 0; i < len / 2; i++)
	{
		temp = str[i] ;
		str[i] = str[len - i - 1] ;
		str[len - i - 1] = temp ;
	}	
}

int ffs(int i)
{
#if 1
	/* inlined binary search method */
	char n = 1;
#if UINT_MAX == 0xffffU
	/* nothing to do here -- just trying to avoiding possible problems */
#elif UINT_MAX == 0xffffffffU
	if (!(i & 0xffff)) {
		n += 16;
		i >>= 16;
	}
#else
#error ffs needs rewriting!
#endif

	if (!(i & 0xff)) {
		n += 8;
		i >>= 8;
	}
	if (!(i & 0x0f)) {
		n += 4;
		i >>= 4;
	}
	if (!(i & 0x03)) {
		n += 2;
		i >>= 2;
	}
	return (i) ? (n + ((i+1) & 0x01)) : 0;

#else
	/* linear search -- slow, but small */
	int n;

	for (n = 0 ; i ; ++n) {
		i >>= 1;
	}
	
	return n;
#endif
}

char* strerror (int __errnum)
{
	//stub
	return "" ;
}
