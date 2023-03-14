/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015, 2021-2022 Tony Dinh <mibodhi@gmail.com>
 * Copyright (C) 2015
 * Gerald Kerma <dreagle@doukki.net>
 * Luka Perkov <luka.perkov@sartura.hr>
 */

#ifndef _CONFIG_NSA310S_H
#define _CONFIG_NSA310S_H

#include "mv-common.h"

/* default environment variables */

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=console=ttyS0,115200\0" \
	"mtdids=nand0=orion_nand\0" \
	"mtdparts="CONFIG_MTDPARTS_DEFAULT \
	"kernel=/boot/zImage\0" \
	"fdt=/boot/nsa310s.dtb\0" \
	"bootargs_root=ubi.mtd=3 root=ubi0:rootfs rootfstype=ubifs rw\0"

/* Ethernet driver configuration */
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	1

/* Support large HDDs for USB and SATA */
#define CONFIG_LBA48
#define CONFIG_SYS_64BIT_LBA

#endif /* _CONFIG_NSA310S_H */
