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
#include <plat_config.h>
#include <mmio.h>
#include <string.h>
#include <sys/errno.h>
#include "sunxi_def.h"
#include "sunxi_private.h"

#define R_PRCM_BASE	0x1f01400ULL
#define R_TWI_BASE	0x1f02400ULL
#define R_PIO_BASE	0x1f02c00ULL

#define RSB_BASE	0x1f03400ULL
#define RSB_CTRL	0x00
#define RSB_CCR		0x04
#define RSB_INTE	0x08
#define RSB_STAT	0x0c
#define RSB_DADDR0	0x10
#define RSB_DLEN	0x18
#define RSB_DATA0	0x1c
#define RSB_LCR		0x24
#define RSB_PMCR	0x28
#define RSB_CMD		0x2c
#define RSB_SADDR	0x30

#define RSBCMD_SRTA	0xE8
#define RSBCMD_RD8	0x8B
#define RSBCMD_RD16	0x9C
#define RSBCMD_RD32	0xA6
#define RSBCMD_WR8	0x4E
#define RSBCMD_WR16	0x59
#define RSBCMD_WR32	0x63

#define BIT(n) (1U << (n))

#define RUNTIME_ADDR	0x2d
#define AXP803_HW_ADDR	0x3a3

/* Initialize the RSB controller and its pins. */
static int init_rsb(void)
{
	uint32_t reg;

	/* un-gate PIO clock */
	reg = mmio_read_32(R_PRCM_BASE + 0x28);
	mmio_write_32(R_PRCM_BASE + 0x28, reg | 0x01);

	/* get currently configured function for pins PL0 and PL1 */
	reg = mmio_read_32(R_PIO_BASE + 0x00);
	if ((reg & 0xff) == 0x33) {
		NOTICE("already configured for TWI\n");
		return -EBUSY;
	}

	if ((reg & 0xff) == 0x22) {
		NOTICE("PMIC: already configured for RSB\n");
		return -EEXIST;	/* configured for RSB mode already */
	}

	/* switch pins PL0 and PL1 to RSB */
	mmio_write_32(R_PIO_BASE + 0, (reg & ~0xff) | 0x22);

	/* level 2 drive strength */
	reg = mmio_read_32(R_PIO_BASE + 0x14);
	mmio_write_32(R_PIO_BASE + 0x14, (reg & ~0x0f) | 0xa);

	/* set both ports to pull-up */
	reg = mmio_read_32(R_PIO_BASE + 0x1c);
	mmio_write_32(R_PIO_BASE + 0x1c, (reg & ~0x0f) | 0x5);

	/* assert & de-assert reset of RSB */
	reg = mmio_read_32(R_PRCM_BASE + 0xb0);
	mmio_write_32(R_PRCM_BASE + 0xb0, reg & ~0x08);
	reg = mmio_read_32(R_PRCM_BASE + 0xb0);
	mmio_write_32(R_PRCM_BASE + 0xb0, reg | 0x08);

	/* un-gate RSB clock */
	reg = mmio_read_32(R_PRCM_BASE + 0x28);
	mmio_write_32(R_PRCM_BASE + 0x28, reg | 0x08);

	mmio_write_32(RSB_BASE + RSB_CTRL, 0x01);	/* soft reset */

	mmio_write_32(RSB_BASE + RSB_CCR, 0x11d);	/* clock to 400 KHz */

	do {
		reg = mmio_read_32(RSB_BASE + RSB_CTRL);
	} while (reg & 1);			/* transaction in progress */

	return 0;
}

int sunxi_pmic_read(uint8_t address)
{
	uint32_t reg;

	mmio_write_32(RSB_BASE + RSB_DLEN, 0x10); /* read a byte, snake oil? */
	mmio_write_32(RSB_BASE + RSB_CMD, RSBCMD_RD8);	/* read a byte */
	mmio_write_32(RSB_BASE + RSB_DADDR0, address);
	mmio_write_32(RSB_BASE + RSB_CTRL, 0x80);	/* start transaction */
	do {
		reg = mmio_read_32(RSB_BASE + RSB_CTRL);
	} while (reg & 0x80);			/* transaction in progress */

	reg = mmio_read_32(RSB_BASE + RSB_STAT);
	if (reg == 0x01) {			/* transaction complete */
		reg = mmio_read_32(RSB_BASE + RSB_DATA0); /* result register */
		return reg & 0xff;
	}

	return -reg;
}

int sunxi_pmic_write(uint8_t address, uint8_t value)
{
	uint32_t reg;

	mmio_write_32(RSB_BASE + RSB_DLEN, 0x00); /* write a byte, snake oil? */
	mmio_write_32(RSB_BASE + RSB_CMD, RSBCMD_WR8);	/* write a byte */
	mmio_write_32(RSB_BASE + RSB_DADDR0, address);
	mmio_write_32(RSB_BASE + RSB_DATA0, value);
	mmio_write_32(RSB_BASE + RSB_CTRL, 0x80);	/* start transaction */
	do {
		reg = mmio_read_32(RSB_BASE + RSB_CTRL);
	} while (reg & 0x80);			/* transaction in progress */

	reg = mmio_read_32(RSB_BASE + RSB_STAT);
	if (reg == 0x01)			/* transaction complete */
		return 0;

	return -reg;
}

static void rsb_wait(const char *desc)
{
	uint32_t reg;
	int cnt = 0;

	do {
		reg = mmio_read_32(RSB_BASE + RSB_CTRL);
		cnt++;
	} while (reg & 0x80);			/* transaction in progress */

	reg = mmio_read_32(RSB_BASE + RSB_STAT);
	if (reg == 0x01)
		return;

	ERROR("%s: 0x%x\n", desc, reg);
}

/* Initialize the RSB PMIC connection. */
static int pmic_init(uint16_t hw_addr, uint8_t rt_addr)
{
	int ret;

	/* Switch PMIC to RSB mode */
	mmio_write_32(RSB_BASE + RSB_PMCR,
		      0x00 | (0x3e << 8) | (0x7c << 16) | BIT(31));
	do {
		ret = mmio_read_32(RSB_BASE + RSB_PMCR);
	} while (ret & (1U << 31));		/* transaction in progress */

	mmio_write_32(RSB_BASE + RSB_CCR, 0x103);	/* 3 MHz */

	mmio_write_32(RSB_BASE + RSB_SADDR, hw_addr | (rt_addr << 16));
	mmio_write_32(RSB_BASE + RSB_CMD, RSBCMD_SRTA);
	mmio_write_32(RSB_BASE + RSB_CTRL, 0x80);
	rsb_wait("set run-time address");

	/* Set slave runtime address */
	mmio_write_32(RSB_BASE + RSB_SADDR, rt_addr << 16);

	ret = sunxi_pmic_read(0x03);
	if (ret < 0) {
		ERROR("PMIC: error %d reading PMIC type\n", ret);
		return -2;
	}

	if ((ret & 0xcf) != 0x41) {
		ERROR("PMIC: unknown PMIC type number 0x%x\n", ret);
		return -3;
	}

	return 0;
}

/* Setup the PMIC: DCDC1 to 3.3V, enable DC1SW and DLDO4 */
static int pmic_setup(const char *dt_name)
{
	unsigned int ret;

	if (sunxi_pmic_read(0x20) != 0x11) {
		/* Set DCDC1 voltage to 3.3 Volts */
		ret = sunxi_pmic_write(0x20, 0x11);
		if (ret < 0) {
			NOTICE("PMIC: error %d writing DCDC1 voltage\n", ret);
			return -2;
		}
	}

	/* Enable DC1SW to power PHY, DLDO4 for WiFi and DLDO1 for HDMI */
	ret = sunxi_pmic_read(0x12);
	if ((ret & 0xd9) != 0xd9) {
		ret = sunxi_pmic_write(0x12, ret | 0xd8);
		if (ret < 0) {
			NOTICE("PMIC: error %d enabling DC1SW/DLDO4/DLDO1\n",
			       ret);
			return -4;
		}
	}

	/*
	 * On the Pine64 the AXP is wired wrongly: to reset DCDC5 to 1.24V.
	 * However the DDR3L chips require 1.36V instead. Fix this up. Other
	 * boards hopefully do the right thing here and don't require any
	 * changes.
	 */
	ret = sunxi_pmic_read(0x24) & 0x7f;	/* read DCDC5 register */
	if (!strcmp(dt_name, "sun50i-a64-pine64-plus")) {
		if (ret == 0x26) {	/* check for 1.24V value */
			NOTICE("PMIC: fixing DRAM voltage from 1.24V to 1.36V\n");
			sunxi_pmic_write(0x24, 0x2c);
			ret = 0x2c;
		}
	}
	/* reg 24h: DCDC5: 0.80-1.12V: 10mv/step, 1.14-1.84V: 20mv/step */
	if (ret > 0x20)
		ret = ((ret - 0x20) * 2) + 112;
	else
		ret = ret + 80;
	INFO("PMIC: DRAM voltage: %u.%s%uV\n", ret / 100,
	     (ret % 100) > 10 ? "" : "0", ret % 100);

	/* Enable the LCD power planes to get the display up early. */
	if (!strcmp(dt_name, "sun50i-a64-pinebook")) {
		sunxi_pmic_write(0x16, 0x12); /* DLDO2 = VCC-MIPI = 2.5V */
		ret = sunxi_pmic_read(0x12);
		sunxi_pmic_write(0x12, ret | 0x10);

		sunxi_pmic_write(0x1c, 0x0a); /* FLDO1 = HSIC = 1.2V */
		ret = sunxi_pmic_read(0x13);
		sunxi_pmic_write(0x13, ret | 0x4);

		INFO("PMIC: enabled Pinebook display\n");
	}

	/* The same thing, but for TERES I */
	if (!strcmp(dt_name, "sun50i-a64-teres-i")) {
		sunxi_pmic_write(0x16, 0x12); /* DLDO2 = VCC-EDP-2V5 = 2.5V */
		sunxi_pmic_write(0x17, 0x05); /* DLDO3 = VCC-EDP-1V2 = 1.2V */

		ret = sunxi_pmic_read(0x12);
		sunxi_pmic_write(0x12, ret | 0x10);	/* enable DLDO2 */
		udelay(1000);				/* wait > 2ms */
		sunxi_pmic_write(0x12, ret | 0x30);	/* enable DLDO3 */

		INFO("PMIC: enabled TERES I display power\n");
	}

	sunxi_pmic_write(0x15, 0x1a);	/* DLDO1 = VCC3V3_HDMI voltage = 3.3V */
	sunxi_pmic_write(0x21, 60);		/* Set DCDC2/CPU voltage to 1.1V */
	sunxi_pmic_write(0x16, 0x12);	/* DLDO2 = VCC2V5_EDP voltage = 2.5V */
	sunxi_pmic_write(0x1c, 0xa);	/* FLDO1 = VCC1V2_EDP voltage = 1.2V */
	sunxi_pmic_write(0x91, 0x1a);	/* GPIO0LDO voltage = 3.3V */
	sunxi_pmic_write(0x90, 0x3);	/* Enable GPIO0LDO */
	sunxi_pmic_write(0x30, sunxi_pmic_read(0x30) | BIT(2)); /* Enable USB at Lime64 */
	ret = sunxi_pmic_read(0x13);
	/* Enable FLDO1 to power up eDP bridge */
	ret = sunxi_pmic_write(0x13, ret | 0x4);

	return 0;
}

/*
 * Program the AXP803 via the RSB bus.
 */
int sunxi_pmic_setup(const char *dt_name)
{
	int ret;

	INFO("Configuring AXP PMIC\n");

	ret = init_rsb();
	if (ret && ret != -EEXIST) {
		ERROR("Could not init RSB controller.\n");
		return -1;
	}

	if (ret != -EEXIST) {
		ret = pmic_init(AXP803_HW_ADDR, RUNTIME_ADDR);
		if (ret) {
			ERROR("Could not connect to AXP PMIC.\n");
			return -2;
		}
	}

	ret = pmic_setup(dt_name);
	if (!ret)
		INFO("PMIC: setup successful\n");
	else
		ERROR("PMIC: setup failed: %d\n", ret);

	return ret;
}
