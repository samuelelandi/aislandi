.DEFAULT_GOAL := all
VERSION = 1.0
SRC	= libs/aisparser/c/src/
CC	= gcc
CFLAGS	= -I$(SRC) -DENABLE_TEST
#LIBS	=
OBJS		=	$(SRC)nmea.o $(SRC)vdm_parse.o $(SRC)sixbit.o ais-landi-server.o
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
    LIBS   += -lpthread
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
		$(CC) $(OBJS) -o ais-landi-server $(LIBS)
		
