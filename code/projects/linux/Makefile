CC=gcc

OFLAGS= -O2
#OFLAGS= -O1 -g

CFLAGS=${INCLUDES} \
       ${OFLAGS} \
       -std=c99

DEFINES= -DLINUX

LINUX_PROJECT_PREFIX=.
LINUX_PROJECT_SRC=${LINUX_PROJECT_PREFIX}/main.c

LINUX_PLATFORM_PREFIX=../../src/platform/linux
LINUX_PLATFORM_SRC=${LINUX_PLATFORM_PREFIX}/time.c \
		   ${LINUX_PLATFORM_PREFIX}/gl.c

COMMON_PREFIX=../../src/common
COMMON_SRC=${COMMON_PREFIX}/testrunner.c \
	   ${COMMON_PREFIX}/scene_setup.c \
	   ${COMMON_PREFIX}/post_setup.c \
	   ${COMMON_PREFIX}/gl.c

TESTS_PREFIX=../../src/tests
TESTS_SRC=${TESTS_PREFIX}/trival_pass.c \
	  ${TESTS_PREFIX}/vertex_lighting.c \
	  ${TESTS_PREFIX}/fragment_lighting.c \
	  ${TESTS_PREFIX}/five_tap_post.c \
	  ${TESTS_PREFIX}/one_tap_post.c \
	  ${TESTS_PREFIX}/triangle_throughput.c \
	  ${TESTS_PREFIX}/cpu_writes.c \
	  ${TESTS_PREFIX}/cpu_read_accum.c \
	  ${TESTS_PREFIX}/clears.c \
	  ${TESTS_PREFIX}/test_registry.c 

LIBS = -L/usr/lib -lGLEW -lglut
OUT=out/openglinsights_mccaffrey

INCLUDES=-I../../src

all:
	${CC} \
	${LIBS} \
	-o ${OUT} \
	${CFLAGS} \
	${DEFINES} \
	${LINUX_PROJECT_SRC} \
	${LINUX_PLATFORM_SRC} \
	${COMMON_SRC} \
	${TESTS_SRC}
