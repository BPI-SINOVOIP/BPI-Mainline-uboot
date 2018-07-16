#!/bin/bash

export CROSS_COMPILE=aarch64-linux-gnu-
make PLAT=sun50iw1p1 DEBUG=1 bl31
