/*
 * AXP221 and AXP223 driver
 *
 * IMPORTANT when making changes to this file check that the registers
 * used are the same for the axp221 and axp223.
 *
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <errno.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pmic_bus.h>
#include <axp221.h>

static u8 axp221_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return (mvolt - min) / div;
}

int axp221_set_dcdc1(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 1600, 3400, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_DCDC1_EN);

	ret = pmic_bus_write(AXP221_DCDC1_CTRL, cfg);
	if (ret)
		return ret;

	ret = pmic_bus_setbits(AXP221_OUTPUT_CTRL2,
			       AXP221_OUTPUT_CTRL2_DCDC1SW_EN);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_DCDC1_EN);
}

int axp221_set_dcdc2(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 600, 1540, 20);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_DCDC2_EN);

	ret = pmic_bus_write(AXP221_DCDC2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_DCDC2_EN);
}

int axp221_set_dcdc3(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 600, 1860, 20);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_DCDC3_EN);

	ret = pmic_bus_write(AXP221_DCDC3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_DCDC3_EN);
}

int axp221_set_dcdc4(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 600, 1540, 20);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_DCDC4_EN);

	ret = pmic_bus_write(AXP221_DCDC4_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_DCDC4_EN);
}

int axp221_set_dcdc5(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 1000, 2550, 50);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_DCDC5_EN);

	ret = pmic_bus_write(AXP221_DCDC5_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_DCDC5_EN);
}

int axp221_set_dldo1(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL2,
					AXP221_OUTPUT_CTRL2_DLDO1_EN);

	ret = pmic_bus_write(AXP221_DLDO1_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL2,
				AXP221_OUTPUT_CTRL2_DLDO1_EN);
}

int axp221_set_dldo2(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL2,
					AXP221_OUTPUT_CTRL2_DLDO2_EN);

	ret = pmic_bus_write(AXP221_DLDO2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL2,
				AXP221_OUTPUT_CTRL2_DLDO2_EN);
}

int axp221_set_dldo3(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL2,
					AXP221_OUTPUT_CTRL2_DLDO3_EN);

	ret = pmic_bus_write(AXP221_DLDO3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL2,
				AXP221_OUTPUT_CTRL2_DLDO3_EN);
}

int axp221_set_dldo4(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL2,
					AXP221_OUTPUT_CTRL2_DLDO4_EN);

	ret = pmic_bus_write(AXP221_DLDO4_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL2,
				AXP221_OUTPUT_CTRL2_DLDO4_EN);
}

int axp221_set_aldo1(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_ALDO1_EN);

	ret = pmic_bus_write(AXP221_ALDO1_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_ALDO1_EN);
}

int axp221_set_aldo2(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL1,
					AXP221_OUTPUT_CTRL1_ALDO2_EN);

	ret = pmic_bus_write(AXP221_ALDO2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL1,
				AXP221_OUTPUT_CTRL1_ALDO2_EN);
}

int axp221_set_aldo3(unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL3,
					AXP221_OUTPUT_CTRL3_ALDO3_EN);

	ret = pmic_bus_write(AXP221_ALDO3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL3,
				AXP221_OUTPUT_CTRL3_ALDO3_EN);
}

int axp221_set_eldo(int eldo_num, unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);
	u8 addr, bits;

	switch (eldo_num) {
	case 3:
		addr = AXP221_ELDO3_CTRL;
		bits = AXP221_OUTPUT_CTRL2_ELDO3_EN;
		break;
	case 2:
		addr = AXP221_ELDO2_CTRL;
		bits = AXP221_OUTPUT_CTRL2_ELDO2_EN;
		break;
	case 1:
		addr = AXP221_ELDO1_CTRL;
		bits = AXP221_OUTPUT_CTRL2_ELDO1_EN;
		break;
	default:
		return -EINVAL;
	}

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP221_OUTPUT_CTRL2, bits);

	ret = pmic_bus_write(addr, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP221_OUTPUT_CTRL2, bits);
}

/* bpi, set gldo power */
int axp221_set_gldo(int gldo_num, unsigned int mvolt)
{
	int ret;
	u8 cfg = axp221_mvolt_to_cfg(mvolt, 700, 3300, 100);
	u8 addr;
	u8 ldoaddr;

	switch (gldo_num) {
	case 0:
		addr = AXP221_GPIO0_CTRL;
		ldoaddr = AXP221_GPIO0_LDO;
		break;
	case 1:
		addr = AXP221_GPIO1_CTRL;
		ldoaddr = AXP221_GPIO1_LDO;
		break;
	default:
		return -EINVAL;
	}

	if (mvolt == 0)
		return pmic_bus_write(addr, 0x07); //OFF

	ret = pmic_bus_write(ldoaddr, cfg);
	if (ret)
		return ret;

	return pmic_bus_write(addr, 0x03); //ON
}

int axp221_init(void)
{
	/* This cannot be 0 because it is used in SPL before BSS is ready */
	static int needs_init = 1;
	u8 axp_chip_id;
	int ret;

	if (!needs_init)
		return 0;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP221_CHIP_ID, &axp_chip_id);
	if (ret)
		return ret;

	if (!(axp_chip_id == 0x6 || axp_chip_id == 0x7 || axp_chip_id == 0x17))
		return -ENODEV;

	needs_init = 0;
	return 0;
}

int axp221_get_sid(unsigned int *sid)
{
	u8 *dest = (u8 *)sid;
	int i, ret;

	ret = axp221_init();
	if (ret)
		return ret;

	ret = pmic_bus_write(AXP221_PAGE, 1);
	if (ret)
		return ret;

	for (i = 0; i < 16; i++) {
		ret = pmic_bus_read(AXP221_SID + i, &dest[i]);
		if (ret)
			return ret;
	}

	pmic_bus_write(AXP221_PAGE, 0);

	for (i = 0; i < 4; i++)
		sid[i] = be32_to_cpu(sid[i]);

	return 0;
}
