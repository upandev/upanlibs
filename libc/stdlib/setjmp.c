/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
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
# include <setjmp.h>

static void __save_sig_mask(sigjmp_buf env, int savesigmask) {
  if (savesigmask) {
    sigprocmask(SIG_SETMASK, NULL, &env->__saved_mask);
    env->__mask_was_saved = 1;
  } else {
    env->__mask_was_saved = 0;
  }
}

__attribute__((naked))
int sigsetjmp(sigjmp_buf env, int savesigmask) {
  __asm__ __volatile__(
  "movq %rbx, 0*8(%rdi);"
  "movq %rbp, 1*8(%rdi);"
  "movq %r12, 2*8(%rdi);"
  "movq %r13, 3*8(%rdi);"
  "movq %r14, 4*8(%rdi);"
  "movq %r15, 5*8(%rdi);"
  "movq %rsp, 6*8(%rdi);"
  "movq (%rsp), %rax;"
  "movq %rax, 7*8(%rdi);"
  "subq $8, %rsp;" // Maintain 16-byte stack alignment before 'call'
  "call __save_sig_mask;"
  "addq $8, %rsp;"
  "xorl %eax, %eax;" // Return 0 on the initial invocation
  "ret");
}

__attribute__((naked))
static void __siglongjmp(sigjmp_buf env, int val) {
  __asm__ __volatile__( // Restore callee-saved regs:
  "movq 0*8(%rdi), %rbx;"
  "movq 1*8(%rdi), %rbp;"
  "movq 2*8(%rdi), %r12;"
  "movq 3*8(%rdi), %r13;"
  "movq 4*8(%rdi), %r14;"
  "movq 5*8(%rdi), %r15;"
  "movq 6*8(%rdi), %rsp;" // Load saved stack, and "return" to saved RIP:
  "movq 7*8(%rdi), %rdx;"
  "test %rsi, %rsi;"
  "movq %rsi, %rax;"
  "movq $1, %rcx;"
  "cmove %rcx, %rax;"
  "movq %rdx, (%rsp);" // place RIP on top of the (restored) stack
  "ret;");
}

void siglongjmp(sigjmp_buf env, int val) {
  if (env->__mask_was_saved) {
    sigprocmask(SIG_SETMASK, &env->__saved_mask, NULL);
  }
  __siglongjmp(env, val);
}