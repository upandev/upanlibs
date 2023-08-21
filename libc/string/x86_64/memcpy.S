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
ENTRY (BP_SYM (memcpy))
	/* Cutoff for the big loop is a size of 32 bytes since otherwise
	   the loop will never be entered.  */
	cmpq	$32, %rdx
	movq	%rdx, %rcx
#if !MEMPCPY_P
	movq	%rdi, %r10	/* Save value. */
#endif

	/* We need this in any case.  */
	cld

	jbe	1f

	/* Align destination.  */
	movq	%rdi, %rax
	negq	%rax
	andq	$7, %rax
	subq	%rax, %rcx
	xchgq	%rax, %rcx

	rep; movsb

	movq	%rax, %rcx
	subq	$32, %rcx
	js	2f

	/* Next 3 insns are 11 bytes total, make sure we decode them in one go */
	.p2align 4,,11
3:
	/* Now correct the loop counter.  Please note that in the following
	   code the flags are not changed anymore.  */
	subq	$32, %rcx

	movq	(%rsi), %rax
	movq	8(%rsi), %rdx
	movq	16(%rsi), %r8
	movq	24(%rsi), %r9
	movq	%rax, (%rdi)
	movq	%rdx, 8(%rdi)
	movq	%r8, 16(%rdi)
	movq	%r9, 24(%rdi)

	leaq	32(%rsi), %rsi
	leaq	32(%rdi), %rdi

	jns	3b

	/* Correct extra loop counter modification.  */
2:	addq	$32, %rcx
1:	rep; movsb

#if MEMPCPY_P
	movq	%rdi, %rax		/* Set return value.  */
#else
	movq	%r10, %rax		/* Set return value.  */

#endif
	ret

END (BP_SYM (memcpy))
