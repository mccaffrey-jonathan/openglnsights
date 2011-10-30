CC=gcc

OFLAGS=

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
COMMON_SRC=${COMMON_PREFIX}/testrunner.c

TESTS_PREFIX=../../src/tests
TESTS_SRC=${TESTS_PREFIX}/trival_pass.c \
	  ${TESTS_PREFIX}/triangle_throughput.c \
	  ${TESTS_PREFIX}/cpu_writes.c \
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
