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

#ifndef __SUNXI_DEF_H__
#define __SUNXI_DEF_H__

#define SUNXI_CCM_BASE			0x01c20000
#define SUNXI_PIO_BASE			0x01c20800
#define SUNXI_UART0_BASE		0x01c28000
#define GICD_BASE			0x01c81000
#define GICC_BASE			0x01c82000

/* Firmware Image Package */
#define FIP_IMAGE_NAME			"fip.bin"
#define SUNXI_PRIMARY_CPU			0x0

/* Memory location options for Shared data and TSP in sunxi */
#define SUNXI_IN_TRUSTED_SRAM		0
#define SUNXI_IN_TRUSTED_DRAM		1

/*******************************************************************************
 * sunxi memory map related constants
 ******************************************************************************/

#define SUNXI_MAX_DRAM_SIZE           (2ull<<30)     /*2G*/

/*
 * This puts ATF into SRAM A2. The first 16KB (@0x40000) are used by the
 * OpenRISC exception vectors and are actually only sparsely implemented
 * to match the OpenRISC vector table layout (one word every 256 Bytes).
 * According to the manual SRAM A2 should be hardwired to be secure only,
 * but this is apparently not true.
 * This SRAM is tightly coupled to the OpenRISC controller, so it's not the
 * ideal place to put ATF into, but worked better than SRAM C for me.
 */
#define SUNXI_TRUSTED_MONITOR_BASE	0x00044000	/* 16KB into SRAM A2 */
#define SUNXI_TRUSTED_MONITOR_SIZE	(64 << 10)	/* 64 KByte */

//atf code limit
#define SUNXI_TRUSTED_MONITOR_LIMIT	(SUNXI_TRUSTED_MONITOR_BASE + SUNXI_TRUSTED_MONITOR_SIZE)


#define DRAM1_BASE		0x40000000ull
#define DRAM1_SIZE		0x40000000ull   //1G
#define DRAM1_END		(DRAM1_BASE + DRAM1_SIZE - 1)
#define DRAM1_SEC_SIZE		0x01000000ull

#define DRAM_BASE		DRAM1_BASE
#define DRAM_SIZE		DRAM1_SIZE

/* Load address of BL33 in the sunxi */
#define NS_IMAGE_OFFSET		(DRAM1_BASE + 0xA000000) /* DRAM + 160MB */

/* Special value used to verify platform parameters from BL2 to BL3-1 */
#define SUNXI_BL31_PLAT_PARAM_VAL	0x12345678 //0x0f1e2d3c4b5a6978ULL


/*******************************************************************************
 * PL011 related constants
 ******************************************************************************/
#define UART0_BAUDRATE  115200

#define UART0_CLK_IN_HZ 24000000

#endif /* __SUNXI_DEF_H__ */
