/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Configuration for Xilinx Versal MINI configuration
 *
 * (C) Copyright 2018-2019 Xilinx, Inc.
 * Michal Simek <michal.simek@xilinx.com>
 * Siva Durga Prasad Paladugu <siva.durga.paladugu@xilinx.com>
 */

#ifndef __CONFIG_VERSAL_MINI_H
#define __CONFIG_VERSAL_MINI_H

#define CONFIG_EXTRA_ENV_SETTINGS

#include <configs/xilinx_versal.h>

/* Undef unneeded configs */
#undef CONFIG_EXTRA_ENV_SETTINGS

#undef CONFIG_SYS_CBSIZE
#define CONFIG_SYS_CBSIZE		1024

#endif /* __CONFIG_VERSAL_MINI_H */
