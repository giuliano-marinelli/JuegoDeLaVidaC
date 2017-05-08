#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/openmp/main.o \
	${OBJECTDIR}/serie/main.o \
	${OBJECTDIR}/vectorial/main.o


# C Compiler Flags
CFLAGS=-msse2

# CC Compiler Flags
CCFLAGS=-fopenmp -msse2
CXXFLAGS=-fopenmp -msse2

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/juegodelavidac

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/juegodelavidac: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/juegodelavidac ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/openmp/main.o: openmp/main.c 
	${MKDIR} -p ${OBJECTDIR}/openmp
	${RM} "$@.d"
	$(COMPILE.c) -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/openmp/main.o openmp/main.c

${OBJECTDIR}/serie/main.o: serie/main.c 
	${MKDIR} -p ${OBJECTDIR}/serie
	${RM} "$@.d"
	$(COMPILE.c) -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/serie/main.o serie/main.c

${OBJECTDIR}/vectorial/main.o: vectorial/main.c 
	${MKDIR} -p ${OBJECTDIR}/vectorial
	${RM} "$@.d"
	$(COMPILE.c) -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/vectorial/main.o vectorial/main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/juegodelavidac

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
