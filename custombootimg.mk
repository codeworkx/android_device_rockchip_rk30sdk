#
# Copyright (C) 2012 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

RKBOOTIMG := $(HOST_OUT_EXECUTABLES)/rkbootimg

INSTALLED_BOOTIMAGE_TARGET := $(PRODUCT_OUT)/boot.img
INSTALLED_RECOVERYIMAGE_TARGET := $(PRODUCT_OUT)/recovery.img

$(INSTALLED_BOOTIMAGE_TARGET): $(PRODUCT_OUT)/kernel $(INSTALLED_RAMDISK_TARGET) $(RKBOOTIMG)
	$(call pretty,"Boot image: $@")
	$(hide) $(RKBOOTIMG) --kernel $(PRODUCT_OUT)/kernel --ramdisk $(INSTALLED_RAMDISK_TARGET) --base $(BOARD_KERNEL_BASE) --pagesize $(BOARD_KERNEL_PAGESIZE) --ramdiskaddr $(BOARD_FORCE_RAMDISK_ADDRESS) --output $@

$(INSTALLED_RECOVERYIMAGE_TARGET): $(PRODUCT_OUT)/kernel $(recovery_ramdisk) $(RKBOOTIMG)
	@echo ----- Making recovery image ------
	$(hide) $(RKBOOTIMG) --kernel $(PRODUCT_OUT)/kernel --ramdisk $(recovery_ramdisk) --base $(BOARD_KERNEL_BASE) --pagesize $(BOARD_KERNEL_PAGESIZE) --ramdiskaddr $(BOARD_FORCE_RAMDISK_ADDRESS) --output $@
	@echo ----- Made recovery image -------- $@
