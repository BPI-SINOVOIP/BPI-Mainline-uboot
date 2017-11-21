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

#include <stdint.h>
#include <mmio.h>
#include <uart.h>
#include <ccmu.h>

#if DEBUG

uintptr_t serial_ctrl_base;
int uart_locked = 1;

void sunxi_serial_init(int uart_port)
{
	uint32_t reg;
	uint32_t uart_clk;

	/* assert the UART port's reset line */
	reg = mmio_read_32(CCMU_BUS_SOFT_RST_REG4);
	reg &= ~(1 << (CCM_UART_PORT_OFFSET + uart_port));
	mmio_write_32(CCMU_BUS_SOFT_RST_REG4, reg);

	/* release the clock gate */
	reg = mmio_read_32(CCMU_BUS_CLK_GATING_REG3);
	reg |=  (1 << (CCM_UART_PORT_OFFSET + uart_port));
	mmio_write_32(CCMU_BUS_CLK_GATING_REG3, reg);

	/* de-assert the UART port's reset line */
	reg = mmio_read_32(CCMU_BUS_SOFT_RST_REG4);
	reg |=  (1 << (CCM_UART_PORT_OFFSET + uart_port));
	mmio_write_32(CCMU_BUS_SOFT_RST_REG4, reg);


	/* The GPIO pins are already configured */
	serial_ctrl_base = SUNXI_UART0_BASE + uart_port * CCM_UART_ADDR_OFFSET;

	mmio_write_32(serial_ctrl_base + DW_UART_MCR, 0x3);
	uart_clk = (24000000 + 8 * UART_BAUD) / (16 * UART_BAUD);
	mmio_write_32(serial_ctrl_base + DW_UART_LCR,
		      mmio_read_32(serial_ctrl_base + DW_UART_LCR) | 0x80);

	mmio_write_32(serial_ctrl_base + DW_UART_DLH, uart_clk >> 8);
	mmio_write_32(serial_ctrl_base + DW_UART_DLL, uart_clk & 0xff);
	mmio_write_32(serial_ctrl_base + DW_UART_LCR,
		      mmio_read_32(serial_ctrl_base + DW_UART_LCR) & ~0x80);
	mmio_write_32(serial_ctrl_base + DW_UART_LCR,
		      (PARITY << 3) | (STOP << 2) | DLEN);
	mmio_write_32(serial_ctrl_base + DW_UART_FCR, 0x7);
 
	uart_locked = 0;

	return;
}

void sunxi_serial_exit(void)
{
	uart_locked = 1;
}

void sunxi_serial_putc (char c)
{
	if (uart_locked)
		return;

	while (!(mmio_read_32(serial_ctrl_base + DW_UART_LSR) & 0x40))
		;
	mmio_write_32(serial_ctrl_base + DW_UART_THR, c);
}


char sunxi_serial_getc (void)
{
	if (uart_locked)
		return 0;

	while (!(mmio_read_32(serial_ctrl_base + DW_UART_LSR) & 0x01))
		;
	return mmio_read_32(serial_ctrl_base + DW_UART_RBR);
}

int sunxi_serial_tstc (void)
{
	return mmio_read_32(serial_ctrl_base + DW_UART_LSR) & 0x01;
}
#endif /* DEBUG */

int console_init(unsigned long base_addr,
		unsigned int uart_clk, unsigned int baud_rate)
{
	sunxi_serial_init(0);
	return 0;
}

int console_exit()
{
	sunxi_serial_exit();
	return 0;
}

int console_putc(int c)
{
	sunxi_serial_putc(c);
	return 0;
}

int console_getc(void)
{
	return sunxi_serial_getc();
}
