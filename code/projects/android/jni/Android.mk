# Copyright (C) 2009 The Android Open Source Project
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
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

ANDROID_PROJECT_SRC :=

ANDROID_PLATFORM_PREFIX:=../../../src/platform/android
ANDROID_PLATFORM_SRC:= \
	${ANDROID_PLATFORM_PREFIX}/time.c \
	${ANDROID_PLATFORM_PREFIX}/gl.c

COMMON_PREFIX:=../../../src/common
COMMON_SRC:=${COMMON_PREFIX}/testrunner.c \
    ${COMMON_PREFIX}/scene_setup.c \
    ${COMMON_PREFIX}/post_setup.c \
    ${COMMON_PREFIX}/gl.c

TESTS_PREFIX:=../../../src/tests
TESTS_SRC:=${TESTS_PREFIX}/trival_pass.c \
    ${TESTS_PREFIX}/vertex_lighting.c \
    ${TESTS_PREFIX}/five_tap_post.c \
    ${TESTS_PREFIX}/one_tap_post.c \
    ${TESTS_PREFIX}/triangle_throughput.c \
    ${TESTS_PREFIX}/cpu_writes.c \
    ${TESTS_PREFIX}/cpu_read_accum.c \
    ${TESTS_PREFIX}/clears.c \
    ${TESTS_PREFIX}/test_registry.c 

LOCAL_MODULE    := libgl2jni
LOCAL_CFLAGS    := -Werror -std=c99 -DANDROID -O2
LOCAL_SRC_FILES := \
	${ANDROID_PROJECT_SRC} \
	${ANDROID_PLATFORM_SRC} \
	${COMMON_SRC} \
	${TESTS_SRC} \
	gl_code.c
LOCAL_C_INCLUDES :=../../../src
LOCAL_LDLIBS    := -llog -lGLESv2

APP_OPTIM        := release
APP_ABI          := armeabi-v7a

include $(BUILD_SHARED_LIBRARY)
