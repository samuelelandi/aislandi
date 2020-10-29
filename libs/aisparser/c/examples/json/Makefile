.DEFAULT_GOAL := all
VERSION = 1.0
SRC	= /usr/src/uma/ais/libs/aisparser/c/src/
CC	= gcc
CFLAGS	= -I$(SRC) -DENABLE_TEST -g -Wall # -O2
#LIBS	=
OBJS		=	$(SRC)nmea.o $(SRC)vdm_parse.o $(SRC)sixbit.o decode_ais_gps.o
HDRS		= 	$(SRC)nmea.h $(SRC)vdm_parse.h $(SRC)sixbit.h $(SRC)portable.h
# -----------------------------------------------------------------------
# Sort out what operating system is being run and modify CFLAGS and LIBS
#
# If you add a new OSTYPE here please email it to me so that I can add
# it to the distribution in the next release
# -----------------------------------------------------------------------
SYSTYPE := $(shell uname -s)

ifeq ($(SYSTYPE), Linux)
  CFLAGS += -DLINUX
endif

ifeq ($(SYSTYPE), SunOS)
  CFLAGS += -DSOLARIS
  LIBS   += -lposix4
endif

ifeq ($(SYSTYPE), FreeBSD)
  CFLAGS += -DFREEBSD
endif

ifeq ($(SYSTYPE), OpenBSD)
  CFLAGS += -DOPENBSD
endif

# Untested, but should work.
ifeq ($(SYSTYPE), NetBSD)
  CFLAGS += -DNETBSD
endif

ifeq ($(SYSTYPE), Darwin)
  CFLAGS += -DDARWIN
endif

ifeq ($(SYSTYPE), AIX)
  CFLAGS += -DAIX
endif

ifeq ($(LOCK), yes)
  CFLAGS += -DLOCKDEV
  LIBS   += -llockdev
endif
# -----------------------------------------------------------------------
all:	$(OBJS) $(HDRS) $(OBJS)
		$(CC) $(OBJS) -o uma_ais_server $(LIBS)
		
