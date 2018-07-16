/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
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

#include <debug.h>
#include <mmio.h>
#include <ccmu.h>
#include "sunxi_private.h"

#define PLL_CPUX_1008MHZ    0x1410
#define PLL_CPUX_816MHZ     0x1010
#define PLL_CPUX_408MHZ     0x1000

static void mmio_clrsetbits32(uintptr_t addr, uint32_t mask, uint32_t bits)
{
	uint32_t regval = mmio_read_32(addr);

	regval &= ~mask;
	regval |= bits;
	mmio_write_32(addr, regval);
}

static void mmio_setbits32(uintptr_t addr, uint32_t bits)
{
	uint32_t regval = mmio_read_32(addr);

	regval |= bits;
	mmio_write_32(addr, regval);
}

/* TODO (prt): we should have a timeout and return an error/success... */
static int pll_wait_until_stable(uintptr_t addr)
{
	while ((mmio_read_32(addr) & PLL_STABLE_BIT) != PLL_STABLE_BIT) {
		/* spin */
	}

	return 0;
}

int sunxi_setup_clocks(uint16_t socid, const char *dt_name)
{
	uint32_t reg;

	/* Avoid reprogramming PERIPH0 if not necessary */
	reg = mmio_read_32(CCMU_PLL_PERIPH0_CTRL_REG);
	if ((reg & 0x0fffffff) != 0x41811)		/* is not at 600 MHz? */
		mmio_write_32(CCMU_PLL_PERIPH0_CTRL_REG, 0x80041811);

	/* Set up dividers (suitable for the target clock frequency)
	   and switch CPUX (and thus AXI & APB) to the LOSC24 clock */
	mmio_write_32(CCMU_CPUX_AXI_CFG_REG, ( CPUX_SRCSEL_OSC24M |
					       APB_CLKDIV(4) |
					       AXI_CLKDIV(3) ));
	udelay(20);

	/* Set to 816MHz, but don't enable yet. */
	mmio_write_32(CCMU_PLL_CPUX_CTRL_REG, PLL_CPUX_816MHZ);

	/* Enable PLL_CPUX again */
	mmio_setbits32(CCMU_PLL_CPUX_CTRL_REG, PLL_ENABLE_BIT);
	/* Wait until the PLL_CPUX becomes stable */
	pll_wait_until_stable(CCMU_PLL_CPUX_CTRL_REG);

	/* Wait another 20us, because Allwinner does so... */
	udelay(20);

	/* Switch AXI clock back to PLL_CPUX, dividers are set up already. */
	mmio_clrsetbits32(CCMU_CPUX_AXI_CFG_REG,
			  CPUX_SRCSEL_MASK, CPUX_SRCSEL_PLLCPUX);

	/* Wait 1000us, because Allwiner does so... */
	udelay(1000);

	/* AHB1 = PERIPH0 / (3 * 1) = 200MHz, APB1 = AHB1 / 2 */
	mmio_write_32(CCMU_AHB1_APB1_CFG_REG, 0x00003180);
	mmio_write_32(CCMU_APB2_CFG_GREG,     0x01000000); /* APB2 =>  24 MHz */
	mmio_write_32(CCMU_AHB2_CFG_GREG,     0x00000001); /* AHB2 => 300 MHz */

	return 0;
}
