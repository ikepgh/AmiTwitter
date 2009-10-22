################################################################
# $Id: makefile,v 2.3 2009/06/18 20:58:06 ShInKurO Exp $
#
# :ts=4
#
# Makefile for AmiTwitter using either gcc or vbcc and for targets:
#   gcc: AmigaOS4, MorphOS, AmigaOS3, AROS
#  vbcc: AmigaOS4, MorphOS, AmigaOS3
#
#
# Calling convention:
# make os=[os4/morphos/os3/aros] comp=[gcc/vbcc] [debug=1]
# Defaults: AmigaOS4 and gcc compiler, no debug information.
#
################################################################

OBJ		  = amitwitter.o

#
## Default, AmigaOS4 gcc part
#
BIN	   = NoWinED_OS4

ifeq ($(debug),)
  CFLAGS	= -s -O1 -I./include -I../include -mcrt=newlib -fomit-frame-pointer -funroll-loops -mcpu=powerpc -mstring -mmultiple \
			  -fstrict-aliasing -D__USE_INLINE__ -D__USE_BASETYPE__ -D__USE_OLD_TIMEVAL__ -Wall \
			  -Wno-pointer-sign -mregnames
  LDFLAGS   =
else
  CFLAGS	= -I./include -I../include -mcpu=powerpc  -fomit-frame-pointer  -Wno-pointer-sign  -D__USE_INLINE__ \
			  -D__USE_BASETYPE__ -D__USE_OLD_TIMEVAL__ -D_DBUG -Wall -ggdb
  LDFLAGS   = -gstabs
endif

LIBS	= 

RM		= rm -f

LINK	= ppc-amigaos-gcc

CC		= ppc-amigaos-gcc

STRIP	= ppc-amigaos-strip


#
## Set the defaults we'll use in case nothing was chosen
#
ifeq ($(comp),)
 comp = gcc
endif

ifeq ($(os),)
  os = os4
endif


#
## Default-strip invocation: OS3, MorphOS, OS4.
## AROS needs an other call to avoid that the executable is corrupted.
#
ifneq ($(os), aros)
  STRIPINV = --strip-debug --strip-unneeded -R.comment
else
  STRIPINV = --strip-unneeded -R.comment
endif


#
## Choose macros accordingly to parameters given on the command line
## NOTE: Do not use tabs here - use ordinary spaces, otherwise commands will
## be delivered to the console and not interpreted by 'make'.
#
ifeq ($(comp), vbcc)
  #
  ## Default, OS4 (vbcc setup)
  #
  BIN  = NoWinED_OS4
  CC   = vc +aosppc
  LINK = vc +aosppc
  ifeq ($(debug),)
    CFLAGS = -O1 -Iinclude -I/include -D__USE_INLINE__ -D__USE_BASETYPE__ -D__USE_OLD_TIMEVAL__
    LDFLAGS =
  else
    CFLAGS = -Iinclude -I/include -D__USE_INLINE__ -D__USE_BASETYPE__ -D__USE_OLD_TIMEVAL__ -D_DBUG
    LDFLAGS = -gstabs
  endif  # <~debug>

  ifeq ($(os), morphos)
    # vbcc & MorphOS
    BIN  = NoWinED_MOS
    CC   = vc +morphos
    LINK = vc +morphos
    ifeq ($(debug),)
      CFLAGS = -O1 -Iinclude -I/include -O1
      LDFLAGS =
      LIBS   = -lvc
    else
      CFLAGS = -Iinclude -I/include -DDBUG -DDEBUG
      LDFLAGS = -gstabs
      LIBS   = -lvc -ldebug
    endif  # <~debug>
  else # <~morphos>
    ifeq ($(os), os3)
      # vbcc & AmigaOS3
      # [NOTE: Disable optimisations when using the initial 0.9 version of vbcc (only 68k version)
      # Ask Frank Wille for the updated vbcc 0.9 version where several bugs were fixed (compiler AND assembler)
      BIN  = NoWinED_OS3
      CC   = vc +aos68k
      LINK = vc +aos68k
      ifeq ($(debug),)
        CFLAGS = -O1 -Iinclude -I/include  -D__AMIGA__ -c99 -const-in-data -cpu=68020 -c
        LDFLAGS =
        LIBS   = -lvc -lmieee -lamiga
      else
        CFLAGS = --Iinclude -I/include -D__AMIGA__ -c99 -const-in-data -cpu=68020 -c -D_DBUG
        LDFLAGS = -g -hunkdebug
        LIBS   = -lvc -lmieee -lamiga -ldebug
      endif  # <~debug>
    endif  # <~vbcc, os3>
  endif  # <~vbcc, morphos>
else # <~vbcc>
  # Can only be gcc
  # Since the OS4 gcc setup is already complete, do it for other targets
  ifeq ($(os), morphos)
    # gcc & MorphOS
    BIN   = NoWinED_MOS
    CC    = ppc-morphos-gcc
    LINK  = ppc-morphos-gcc
    STRIP = ppc-morphos-strip
    LIBS = -ldebug
      ifeq ($(debug),)
        CFLAGS = -I./include -I../include  -s -O1
        LDFLAGS =
      else
        CFLAGS = -I./include -I../include -D_DBUG
        LDFLAGS = -gstabs
      endif  # <~debug>
  else
    ifeq ($(os), os3)
      # gcc & AmigaOS3
      BIN   = NoWinED_OS3
      CC    = m68k-amigaos-gcc
      LINK  = m68k-amigaos-gcc
      STRIP = strip
      ifeq ($(debug),)
        CFLAGS = -noixemul -I./include -I../include -O1 -s -m68020-60 -fomit-frame-pointer -funroll-loops -fstrict-aliasing -Wall
        LDFLAGS = -noixemul
        LIBS   = -lm -lmui
      else
        CFLAGS =  -noixemul -I./include -I../include -fstrict-aliasing -Wall -D_DBUG
        LDFLAGS = -g -noixemul
        LIBS   = -lm -lmui -ldebug
      endif  # <~debug>
    else
      ifeq ($(os), aros)
        # gcc & AROS
        BIN   = NoWinED_AROS
        CC    = i686-pc-aros-gcc
        LINK  = i686-pc-aros-gcc
        STRIP = i686-pc-aros-gcc
        ifeq ($(debug),)
          CFLAGS = -I./include -I../include -s -O1
          LDFLAGS =
          LIBS   = -larossupport -lmui -lamiga
        else
          CFLAGS = -I./include -I../include -DDBUG 
          LDFLAGS = -gstabs
          LIBS   = -larossupport -lmui -lamiga
        endif  # <~debug>
      endif  # <~aros>
    endif  # <~os3>
  endif  # <~morphos>
endif  # <~gcc> 



#
## Normal stuff takes place
#

.PHONY: all all-before all-after clean clean-custom
all: all-before $(BIN) all-after


clean: clean-custom
	$(RM) $(OBJ) $(BIN)

strip: $(BIN)
ifneq ($(comp), vbcc)
  ifeq ($(debug),)
	@$(STRIP) $(STRIPINV) $(BIN)
  else
	@echo " - Are you sure you want to have debug-information"
	@echo "   stripped from an executable program explicitly"
	@echo "   compiled using debug information???"
  endif
else
	@echo "vbcc needs no STRIP command."
endif

$(BIN): $(OBJ)
	$(LINK) $(LDFLAGS) $(OBJ) -o $(BIN) $(LIBS)

amitwitter.o:  amitwitter.c amitwitter.h
	$(CC) -c amitwitter.c -o amitwitter.o $(CFLAGS)

