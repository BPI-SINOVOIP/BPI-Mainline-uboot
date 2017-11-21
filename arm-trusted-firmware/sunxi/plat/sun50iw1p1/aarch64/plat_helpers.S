/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <asm_macros.S>
#include "../sunxi_def.h"

	.globl	plat_crash_console_init
	.globl	plat_crash_console_putc
	.globl	platform_mem_init
	.globl	platform_smp_init

	/* -----------------------------------------------------
	 * void platform_mem_init(void);
	 *
	 * We don't need to carry out any memory initialization.
	 * -----------------------------------------------------
	 */
func platform_mem_init
	ret

func platform_smp_init
	mrs	x0, ACTLR_EL3
	orr	x0, x0, #(1 << 1)	// Set CPUECTLR_EL1 access control bit
	msr	ACTLR_EL3, x0

	mrs	x0, ACTLR_EL2
	orr	x0, x0, #(1 << 1)	// Set CPUECTLR_EL1 access control bit
	msr	ACTLR_EL2, x0

	mrs	x0, S3_1_c15_c2_1	// Read CPUECTLR_EL1
	orr	x0, x0, #(1 << 6)	// Set the SMPEN bit
	msr	S3_1_c15_c2_1, x0	// Write CPUECTLR_EL1
	mov	x0, #0x0
	msr	cntvoff_el2, x0
	ret

#define UART_TX			0	/* Out: Transmit buffer */
#define UART_LSR        	5       /* In:  Line Status Register */
#define UART_LSR_TEMT_BIT	6	/* Transmitter empty */

/* Use UART0 both for the normal console and for the crash console as well. */
plat_crash_console_putc:
func console_core_putc
	ldr	x1, =SUNXI_UART0_BASE
1:
	ldr	w2, [x1, #(UART_LSR * 4)]
	tbz	w2, #UART_LSR_TEMT_BIT, 1b
	str	w0, [x1, #0]
	ret

/* nothing to do here, just return 1 to indicate success */
func plat_crash_console_init
	mov w0, #1
	ret
