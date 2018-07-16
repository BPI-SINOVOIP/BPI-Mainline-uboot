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

#include <arch.h>
#include <arch_helpers.h>
#include <arm_gic.h>
#include <bl_common.h>
#include <debug.h>
#include <mmio.h>
#include <platform.h>
#include <plat_config.h>
#include <xlat_tables.h>
#include "../sunxi_def.h"
#include "../sunxi_private.h"

/*******************************************************************************
 * plat_config holds the characteristics of the differences between the three
 * FVP platforms (Base, A53_A57 & Foundation). It will be populated during cold
 * boot at each boot stage by the primary before enabling the MMU (to allow cci
 * configuration) & used thereafter. Each BL will have its own copy to allow
 * independent operation.
 ******************************************************************************/
plat_config_t plat_config;

/*
 * Table of regions to map using the MMU.
 * This doesn't include TZRAM as the 'mem_layout' argument passed to
 * configure_mmu_elx() will give the available subset of that,
 */
const mmap_region_t sunxi_mmap[] = {

	// SRAM regions
	{ 0x0010000,			0x0010000,
		0x0030000,			MT_DEVICE | MT_RW | MT_NS },
	// MMI/O region used by peripherals from 0x100.0000 to 0x200.0000
	{ 0x1000000,			0x1000000,
		0x1000000,			MT_DEVICE | MT_RW | MT_SECURE },
	//2G
	{ DRAM1_BASE,			DRAM1_BASE,
		SUNXI_MAX_DRAM_SIZE,		MT_MEMORY | MT_RW | MT_NS},
	{0}
};

void sunxi_configure_mmu_el3(unsigned long total_base, unsigned long total_size,
			     unsigned long ro_start, unsigned long ro_limit,
			     unsigned long coh_start, unsigned long coh_limit)
{
	mmap_add_region(total_base, total_base,
			total_size,
			MT_MEMORY | MT_RW | MT_SECURE);
	mmap_add_region(ro_start, ro_start,
			ro_limit - ro_start,
			MT_MEMORY | MT_RO | MT_SECURE);
	mmap_add_region(coh_start, coh_start,
			coh_limit - coh_start,
			MT_DEVICE | MT_RW | MT_SECURE);
	mmap_add(sunxi_mmap);
	init_xlat_tables();

	enable_mmu_el3(0);
}

unsigned long plat_get_ns_image_entrypoint(void)
{
	return NS_IMAGE_OFFSET;
}

uint64_t plat_get_syscnt_freq(void)
{
	return 24 * 1000 * 1000;
}

static unsigned int get_highest_el(int aarch)
{
	unsigned int el_status;

	/* Figure out whether we have the HYP/EL2 mode implemented */
	if (aarch == 32) {
		el_status = (read_id_pfr1_el1() >> ID_PFR1_VIRTEXT_SHIFT);
		el_status &= ID_PFR1_VIRTEXT_MASK;

		return el_status ? MODE32_hyp : MODE32_svc;
	} else if (aarch == 64) {
		el_status = read_id_aa64pfr0_el1() >> ID_AA64PFR0_EL2_SHIFT;
		el_status &= ID_AA64PFR0_ELX_MASK;

		return el_status ? MODE_EL2 : MODE_EL1;
	} else {
		return -1;
	}
}

/*******************************************************************************
 * Gets SPSR for BL33 entry
 ******************************************************************************/
uint32_t sunxi_get_spsr_for_bl33_entry(int aarch)
{
	unsigned int mode;
	uint32_t spsr;

	mode = get_highest_el(aarch);

	switch (aarch) {
	case 32:
		spsr = SPSR_MODE32(mode, SPSR_T_ARM, SPSR_E_LITTLE,
				   DISABLE_ALL_EXCEPTIONS);
		break;
	case 64:
	default:
		spsr = SPSR_64(mode, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
		break;
	}

	return spsr;
}

#define SRAM_VER_REG 0x01c00024

uint16_t sunxi_get_socid(void)
{
	uint32_t reg;

	reg = mmio_read_32(SRAM_VER_REG);
	mmio_write_32(SRAM_VER_REG, reg | (1 << 15));
	reg = mmio_read_32(SRAM_VER_REG);
	mmio_write_32(0x01c00024, reg & ~(1 << 15));
	return reg >> 16;
}

struct spl_boot_file_head {
	uint32_t  jump_instruction;
	uint32_t  magic[2];
	uint32_t  check_sum;
	uint32_t  length;
	uint8_t   spl_signature[4];
	uint32_t  fel_script_address;
	uint32_t  fel_uEnv_length;
	uint32_t  offset_dt_name;
	uint32_t  reserved1;
	uint32_t  boot_media;
	uint32_t  string_pool[13];
};

#define MAGIC_eGON	0x4e4f4765	/* "eGON" */
#define MAGIC_BT0	0x3054422e	/* ".BT0" */
#define MAGIC_FEL	0x4c45462e	/* ".FEL" */

const char *get_dt_name(void)
{
	const struct spl_boot_file_head *spl_head = (void *)0x10000;

	if (spl_head->magic[0] != MAGIC_eGON)
	       return NULL;

	if (spl_head->magic[1] != MAGIC_BT0 &&
	    spl_head->magic[1] != MAGIC_FEL)
		return NULL;

	/* We need at least U-Boot SPL version 2 for the DT name feature. */
	if (spl_head->spl_signature[3] < 2)
		return NULL;

	return (const char *)spl_head + spl_head->offset_dt_name;
}
