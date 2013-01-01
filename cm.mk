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

# Specify phone tech before including full_phone
$(call inherit-product, vendor/cm/config/gsm.mk)

# Release name
PRODUCT_RELEASE_NAME := rk30sdk

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# Inherit device configuration
$(call inherit-product, device/rockchip/rk30sdk/full_rk30sdk.mk)

# Device identifier. This must come after all inclusions
PRODUCT_DEVICE := rk30sdk
PRODUCT_NAME := cm_rk30sdk
PRODUCT_BRAND := rk30sdk
PRODUCT_MODEL := Rikomagic MK802III
PRODUCT_MANUFACTURER := rockchip

# Set build fingerprint / ID / Prduct Name ect.
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=rk30sdk TARGET_DEVICE=rk30sdk BUILD_FINGERPRINT=rk30sdk/rk30sdk/rk30sdk:4.1.1/JRO03H/eng.ant.20121129.102749:eng/test-keys PRIVATE_BUILD_DESC="rk30sdk-eng 4.1.1 JRO03H eng.ant.20121129.102749 test-keys"
