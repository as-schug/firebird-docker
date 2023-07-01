# The contents of this file are subject to the Interbase Public
# License Version 1.0 (the "License"); you may not use this file
# except in compliance with the License. You may obtain a copy
# of the License at http://www.Interbase.com/IPL/
#
# Software distributed under the License is distributed on an
# "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express
# or implied. See the License for the specific language governing
# rights and limitations under the License.
#
# The Original Code was created by Interbase Software Corporation
# and its successors. Portions created by Borland/Inprise are
# Copyright (C) 1992-1998 and 1999-2000 Borland/Inprise. Portions
# created by InterBase Software Corporation are Copyright (C)
# 1998-1999 InterBase Software Corporation.
#
# Copyright (C) 2000 InterBase Software Corporation
# All Rights Reserved.
# Contributor(s): ______________________________________.
# ---------------------------------------------------------------------
# InterBase Installation Directory
#
# CHANGE this definition to point to your InterBase installation directory
# ---------------------------------------------------------------------
IBASEINC = "-I/opt/interbase/include"
wIBASEINC = -I/mnt/windows/Progra~1/Firebird/include -I/mnt/windows/Dev-Cpp/inc

# ---------------------------------------------------------------------
# General Compiler and linker Defines for Linux
# ---------------------------------------------------------------------
CC =				gcc
wCC =				i586-pc-mingw32-gcc
CFLAGS =			-Wall -w $(IBASEINC)  -fPIC
wCFLAGS =			-Wall -w $(wIBASEINC) -DBUILDING_DLL=1 -D_WINDOWS_
LIB_LINK_FLAGS =	-shared -lgds
RM =				rm -f

SHELL =				/bin/sh
# ---------------------------------------------------------------------
# Generic Compilation Rules 
#
# Do NOT change anything below this point.
# ---------------------------------------------------------------------
objects = sysdep.o md5c.o uuid.o uuidlib.o
winobj = sysdep.wo md5c.wo uuid.wo uuidlib.wo
testobjects = utest.o $(objects)

.SUFFIXES: .o .c

all: $(objects) uuidlib

$(objects): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

uuidlib: $(objects)
	$(CC) $(objects) $(LIB_LINK_FLAGS) -o $@

$(winobj): %.wo : %.c
	$(wCC) $(wCFLAGS) -c $< -o $@

uuidlib.dll: $(winobj)
	$(wCC) $(winobj) $(LIB_LINK_FLAGS) -o $@

utest.o: %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

utest: $(testobjects)
	$(CC) $(testobjects) -o $@

.PHONY: clean

clean:
	$(RM) *.o *.wo uuidlib uuidlib.dll utest

