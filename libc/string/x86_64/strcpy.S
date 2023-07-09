/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2015 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
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

#include <x86_64/_deps.h>

.text
ENTRY (BP_SYM (strcpy))
	movq %rsi, %rcx		/* Source register. */
	andl $7, %ecx		/* mask alignment bits */
	movq %rdi, %rdx		/* Duplicate destination pointer.  */

	jz 5f			/* aligned => start loop */

	neg %ecx		/* We need to align to 8 bytes.  */
	addl $8,%ecx
	/* Search the first bytes directly.  */
0:
	movb	(%rsi), %al	/* Fetch a byte */
	testb	%al, %al	/* Is it NUL? */
	movb	%al, (%rdx)	/* Store it */
	jz	4f		/* If it was NUL, done! */
	incq	%rsi
	incq	%rdx
	decl	%ecx
	jnz	0b

5:
	movq $0xfefefefefefefeff,%r8

	/* Now the sources is aligned.  Unfortunatly we cannot force
	   to have both source and destination aligned, so ignore the
	   alignment of the destination.  */

	/* Next 3 insns are 10 bytes total, make sure we decode them in one go */
	.p2align 4,,10
1:
	/* 1st unroll.  */
	movq	(%rsi), %rax	/* Read double word (8 bytes).  */
	addq	$8, %rsi	/* Adjust pointer for next word.  */
	movq	%rax, %r9	/* Save a copy for NUL finding.  */
	addq	%r8, %r9	/* add the magic value to the word.  We get
				   carry bits reported for each byte which
				   is *not* 0 */
	jnc	3f		/* highest byte is NUL => return pointer */
	xorq	%rax, %r9	/* (word+magic)^word */
	orq	%r8, %r9	/* set all non-carry bits */
	incq	%r9		/* add 1: if one carry bit was *not* set
				   the addition will not result in 0.  */

	jnz	3f		/* found NUL => return pointer */

	movq	%rax, (%rdx)	/* Write value to destination.  */
	addq	$8, %rdx	/* Adjust pointer.  */

	/* 2nd unroll.  */
	movq	(%rsi), %rax	/* Read double word (8 bytes).  */
	addq	$8, %rsi	/* Adjust pointer for next word.  */
	movq	%rax, %r9	/* Save a copy for NUL finding.  */
	addq	%r8, %r9	/* add the magic value to the word.  We get
				   carry bits reported for each byte which
				   is *not* 0 */
	jnc	3f		/* highest byte is NUL => return pointer */
	xorq	%rax, %r9	/* (word+magic)^word */
	orq	%r8, %r9	/* set all non-carry bits */
	incq	%r9		/* add 1: if one carry bit was *not* set
				   the addition will not result in 0.  */

	jnz	3f		/* found NUL => return pointer */

	movq	%rax, (%rdx)	/* Write value to destination.  */
	addq	$8, %rdx	/* Adjust pointer.  */

	/* 3rd unroll.  */
	movq	(%rsi), %rax	/* Read double word (8 bytes).  */
	addq	$8, %rsi	/* Adjust pointer for next word.  */
	movq	%rax, %r9	/* Save a copy for NUL finding.  */
	addq	%r8, %r9	/* add the magic value to the word.  We get
				   carry bits reported for each byte which
				   is *not* 0 */
	jnc	3f		/* highest byte is NUL => return pointer */
	xorq	%rax, %r9	/* (word+magic)^word */
	orq	%r8, %r9	/* set all non-carry bits */
	incq	%r9		/* add 1: if one carry bit was *not* set
				   the addition will not result in 0.  */

	jnz	3f		/* found NUL => return pointer */

	movq	%rax, (%rdx)	/* Write value to destination.  */
	addq	$8, %rdx	/* Adjust pointer.  */

	/* 4th unroll.  */
	movq	(%rsi), %rax	/* Read double word (8 bytes).  */
	addq	$8, %rsi	/* Adjust pointer for next word.  */
	movq	%rax, %r9	/* Save a copy for NUL finding.  */
	addq	%r8, %r9	/* add the magic value to the word.  We get
				   carry bits reported for each byte which
				   is *not* 0 */
	jnc	3f		/* highest byte is NUL => return pointer */
	xorq	%rax, %r9	/* (word+magic)^word */
	orq	%r8, %r9	/* set all non-carry bits */
	incq	%r9		/* add 1: if one carry bit was *not* set
				   the addition will not result in 0.  */

	jnz	3f		/* found NUL => return pointer */

	movq	%rax, (%rdx)	/* Write value to destination.  */
	addq	$8, %rdx	/* Adjust pointer.  */
	jmp	1b		/* Next iteration.  */

	/* Do the last few bytes. %rax contains the value to write.
	   The loop is unrolled twice.  */

	/* Next 3 insns are 6 bytes total, make sure we decode them in one go */
	.p2align 3,,6
3:
	/* Note that stpcpy needs to return with the value of the NUL
	   byte.  */
	movb	%al, (%rdx)	/* 1st byte.  */
	testb	%al, %al	/* Is it NUL.  */
	jz	4f		/* yes, finish.  */
	incq	%rdx		/* Increment destination.  */
	movb	%ah, (%rdx)	/* 2nd byte.  */
	testb	%ah, %ah	/* Is it NUL?.  */
	jz	4f		/* yes, finish.  */
	incq	%rdx		/* Increment destination.  */
	shrq	$16, %rax	/* Shift...  */
	jmp	3b		/* and look at next two bytes in %rax.  */

4:
#ifdef USE_AS_STPCPY
	movq	%rdx, %rax	/* Destination is return value.  */
#else
	movq	%rdi, %rax	/* Source is return value.  */
#endif
	retq
END (BP_SYM (strcpy))
