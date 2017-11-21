/*
 * Copyright (c) 2014, ARM Limited and Contributors. All rights reserved.
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

#include <assert.h>
#include <debug.h>
#include <plat_config.h>
#include <tzc400.h>
#include <mmio.h>
#include "sunxi_def.h"
#include "sunxi_private.h"

#define SPC_BASE	0x1c23400ULL

#define SPC_DECPORT_STA_REG(p) (SPC_BASE + ((p) * 0x0c) + 0x4)
#define SPC_DECPORT_SET_REG(p) (SPC_BASE + ((p) * 0x0c) + 0x8)
#define SPC_DECPORT_CLR_REG(p) (SPC_BASE + ((p) * 0x0c) + 0xc)

/*
 * For the moment we assume that all security programming is done by the
 * primary core.
 * TODO:
 * Might want to enable interrupt on violations when supported?
 */
void sunxi_security_setup(void)
{
	int i;

	NOTICE("Configuring SPC Controller\n");

	/* set all devices to non-secure */
	for (i = 0; i < 6; i++)
		mmio_write_32(SPC_DECPORT_SET_REG(i), 0xff);

	/* switch RSB to secure */
	mmio_write_32(SPC_DECPORT_CLR_REG(3), 0x08);
	
	/* set CCMU mbus_sec, bus_sec, pll_sec to non-secure */
	mmio_write_32(0x01c20000+0x2f0, 0x7);

	/* set R_PRCM power_sec, pll_sec, cpus_clk to non-secure */
	mmio_write_32(0x01f01400+0x1d0, 0x7);
	
	/* Set DMA channels 0-7 to non-secure */
	mmio_write_32(0x01c02000+0x20, 0xff);
}
