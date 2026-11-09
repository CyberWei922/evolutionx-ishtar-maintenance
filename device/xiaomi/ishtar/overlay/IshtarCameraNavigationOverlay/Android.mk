# SPDX-License-Identifier: Apache-2.0
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PACKAGE_NAME := IshtarCameraNavigationOverlay
LOCAL_PRODUCT_MODULE := true
LOCAL_SDK_VERSION := current
LOCAL_MIN_SDK_VERSION := 29
LOCAL_CERTIFICATE := platform
LOCAL_RESOURCE_DIR := $(LOCAL_PATH)/res
# Camera uses 0x7f. Keep target references separate from this overlay (0x7e).
ishtar_camera_navbar_target_apk := vendor/xiaomi/ishtar-camera/proprietary/system/priv-app/MiuiCamera/MiuiCamera.apk
LOCAL_AAPT_FLAGS := --allow-reserved-package-id --package-id 0x7e -I $(ishtar_camera_navbar_target_apk)
LOCAL_ADDITIONAL_DEPENDENCIES := $(ishtar_camera_navbar_target_apk)
include $(BUILD_RRO_PACKAGE)
# Resource references are linked against the exact Camera APK, so relink if it changes.
$(my_res_package): $(ishtar_camera_navbar_target_apk)
ishtar_camera_navbar_target_apk :=
