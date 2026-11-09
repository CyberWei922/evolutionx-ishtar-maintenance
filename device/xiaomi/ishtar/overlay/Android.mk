# SPDX-License-Identifier: Apache-2.0
# The device Android.mk enumerates one level; include the Camera RRO below overlay/.
LOCAL_PATH := $(call my-dir)
include $(call all-makefiles-under,$(LOCAL_PATH))
