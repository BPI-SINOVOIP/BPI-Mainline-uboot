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
#include <assert.h>
#include <bl_common.h>
#include <bl31.h>
#include <console.h>
#include <mmio.h>
#include <platform.h>
#include <stddef.h>
#include "sunxi_def.h"
#include "sunxi_private.h"

#include <debug.h>

/*******************************************************************************
 * Declarations of linker defined symbols which will help us find the layout
 * of trusted SRAM
 ******************************************************************************/
extern unsigned long __RO_START__;
extern unsigned long __RO_END__;

extern unsigned long __COHERENT_RAM_START__;
extern unsigned long __COHERENT_RAM_END__;

/*
 * The next 2 constants identify the extents of the code & RO data region.
 * These addresses are used by the MMU setup code and therefore they must be
 * page-aligned.  It is the responsibility of the linker script to ensure that
 * __RO_START__ and __RO_END__ linker symbols refer to page-aligned addresses.
 */
#define BL31_RO_BASE (unsigned long)(&__RO_START__)
#define BL31_RO_LIMIT (unsigned long)(&__RO_END__)

/*
 * The next 2 constants identify the extents of the coherent memory region.
 * These addresses are used by the MMU setup code and therefore they must be
 * page-aligned.  It is the responsibility of the linker script to ensure that
 * __COHERENT_RAM_START__ and __COHERENT_RAM_END__ linker symbols
 * refer to page-aligned addresses.
 */
#define BL31_COHERENT_RAM_BASE (unsigned long)(&__COHERENT_RAM_START__)
#define BL31_COHERENT_RAM_LIMIT (unsigned long)(&__COHERENT_RAM_END__)


#if 0
#if RESET_TO_BL31
static entry_point_info_t bl33_image_ep_info;
#else
/*******************************************************************************
 * Reference to structure which holds the arguments that have been passed to
 * BL31 from BL2.
 ******************************************************************************/
static bl31_params_t *bl2_to_bl31_params;
#endif
#else
static entry_point_info_t bl33_image_ep_info;
#endif


/*******************************************************************************
 * Return a pointer to the 'entry_point_info' structure of the next image for the
 * security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 ******************************************************************************/
entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
//#if RESET_TO_BL31
#if 1

	assert(sec_state_is_valid(type));
	assert(type == NON_SECURE);

	return &bl33_image_ep_info;
#else
	entry_point_info_t *next_image_info;

	assert(sec_state_is_valid(type));

	next_image_info = (type == NON_SECURE) ?
		bl2_to_bl31_params->bl33_ep_info :
		bl2_to_bl31_params->bl32_ep_info;

	/* None of the images on this platform can have 0x0 as the entrypoint */
	if (next_image_info->pc)
		return next_image_info;
	else
		return NULL;
#endif
}

static unsigned long get_pc(void)
{
	unsigned long pc;

	__asm__ volatile ("adr %0, .\n" : "=r" (pc));
	return pc;
}

/*******************************************************************************
 * Perform any BL31 specific platform actions. Here is an opportunity to copy
 * parameters passed by the calling EL (S-EL1 in BL2 & S-EL3 in BL1) before they
 * are lost (potentially). This needs to be done before the MMU is initialized
 * so that the memory layout can be used while creating page tables. On the FVP
 * we know that BL2 has populated the parameters in secure DRAM. So we just use
 * the reference passed in 'from_bl2' instead of copying. The 'data' parameter
 * is not used since all the information is contained in 'from_bl2'. Also, BL2
 * has flushed this information to memory, so we are guaranteed to pick up good
 * data
 ******************************************************************************/
void bl31_early_platform_setup(bl31_params_t *from_bl2,
				void *plat_params_from_bl2)
{
	unsigned long load_addr;
	const char *mem_name = "unknown memory region";
	const char *soc_name = "unknown SoC";
	uint16_t soc_id;

	/* Initialize the console to provide early debug support */
	console_init(SUNXI_UART0_BASE, UART0_CLK_IN_HZ, UART0_BAUDRATE);
	load_addr = get_pc() & ~0xfff;
	if (load_addr >= 0x44000 && load_addr < 0x54000) {
		mem_name = "SRAM A2";
	} else if (load_addr >= 0x10000 && load_addr < 0x18000) {
		mem_name = "SRAM A1";
	} else if (load_addr >= 0x18000 && load_addr < 0x40000) {
		mem_name = "SRAM C";
	} else if (load_addr >= 0x40000000)
		mem_name = "DRAM";

	soc_id = sunxi_get_socid();

	switch (soc_id) {
	case 0x1689:
		soc_name = "A64/H64";
		break;
	case 0x1718:
		soc_name = "H5";
		break;
	}

	NOTICE("BL3-1: Running on %s (%x) in %s (@0x%lx)\n",
	       soc_name, soc_id, mem_name, load_addr);

#if 0
#if RESET_TO_BL31
	/* There are no parameters from BL2 if BL31 is a reset vector */
	assert(from_bl2 == NULL);
	assert(plat_params_from_bl2 == NULL);

	/*
	 * Do initial security configuration to allow DRAM/device access. On
	 * Base FVP only DRAM security is programmable (via TrustZone), but
	 * other platforms might have more programmable security devices
	 * present.
	 */
	sunxi_security_setup();

	SET_PARAM_HEAD(&bl33_image_ep_info, PARAM_EP, VERSION_1, 0);
	/*
	 * Tell BL31 where the non-trusted software image
	 * is located and the entry state information
	 */
	bl33_image_ep_info.pc = plat_get_ns_image_entrypoint();
	bl33_image_ep_info.spsr = sunxi_get_spsr_for_bl33_entry(64);
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);

#else
	/* Check params passed from BL2 should not be NULL,
	 * We are not checking plat_params_from_bl2 as NULL as we are not
	 * using it on FVP
	 */
	assert(from_bl2 != NULL);
	assert(from_bl2->h.type == PARAM_BL31);
	assert(from_bl2->h.version >= VERSION_1);
	sunxi_security_setup();
	bl2_to_bl31_params = from_bl2;
	assert(((unsigned long)plat_params_from_bl2) == SUNXI_BL31_PLAT_PARAM_VAL);
#endif
#endif
	/*
	 * Do initial security configuration to allow DRAM/device access. On
	 * Base FVP only DRAM security is programmable (via TrustZone), but
	 * other platforms might have more programmable security devices
	 * present.
	 */
	sunxi_security_setup();

	/*
	 * Tell BL31 where the non-trusted software image
	 * is located and the entry state information
	 */
	bl33_image_ep_info.pc = plat_get_ns_image_entrypoint();
	bl33_image_ep_info.spsr = sunxi_get_spsr_for_bl33_entry(64);
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);



}

/*******************************************************************************
 * Initialize the gic, configure the CLCD and zero out variables needed by the
 * secondaries to boot up correctly.
 ******************************************************************************/
void bl31_platform_setup(void)
{
	uint16_t socid;
	const char *dt_name;

	/* Initialize the gic cpu and distributor interfaces */
	arm_gic_init(GICC_BASE, GICD_BASE, 0, NULL, 0);
	arm_gic_setup();

	socid = sunxi_get_socid();

	dt_name = get_dt_name();

	if (dt_name)
		NOTICE("DT: %s\n", dt_name);
	else
		NOTICE("No DT name found, skipping board specific setup.\n");

	/* Detect if this SoC is a multi-cluster one. */
	plat_setup_topology();

	switch (socid) {
	case 0x1689:
		sunxi_pmic_setup(dt_name);
		break;
	case 0x1718:
		break;
	}

	sunxi_setup_clocks(socid, dt_name);

	NOTICE("SCPI: dummy stub handler, implementation level: 000000\n");
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl31_plat_arch_setup(void)
{
	//set smp bit before cache enable
	platform_smp_init();

	sunxi_configure_mmu_el3(BL31_RO_BASE,
				(BL31_COHERENT_RAM_LIMIT - BL31_RO_BASE),
				BL31_RO_BASE,
				BL31_RO_LIMIT,
				BL31_COHERENT_RAM_BASE,
				BL31_COHERENT_RAM_LIMIT);
}
