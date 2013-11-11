#
# Copyright (c) Symbian Software Ltd 2005.  All rights reserved.
# 
# Makefile for building elf2e32 executable. This can be invoked directly
# or it can be invoked via the abld command. 

export PARENT_DIR=$(CURDIR)

# if the makefile is invoked via abld the build directory is defined by EPOCBLD
ifdef EPOCBLD
 export BUILD_DIR=$(EPOCBLD)
else
 export BUILD_DIR=$(EPOCROOT)epoc32\build\s60\mw\graphics\svgtopt\svgtencoder\standalone

endif

MAKEFILE=svgtbinencoder.mk
DEST_MAKEFILE=$(BUILD_DIR)\$(MAKEFILE)

all: $(DEST_MAKEFILE)
	$(MAKE) -C $(BUILD_DIR) -f $(MAKEFILE) 
	@erase /F $(DEST_MAKEFILE) 2>> nul

$(DEST_MAKEFILE) : $(BUILD_DIR)
#	echo $(DEST_MAKEFILE)
	@copy $(MAKEFILE) $(DEST_MAKEFILE) 2>>nul

$(BUILD_DIR) : 
	@mkdir $(BUILD_DIR) 2>> nul

CLEAN: $(DEST_MAKEFILE)
	$(MAKE) -C $(BUILD_DIR) -f $(MAKEFILE) $(CFG)_clean
	@erase /F $(DEST_MAKEFILE) 2>> nul

MAKMAKE: $(DEST_MAKEFILE)

BLD : $(DEST_MAKEFILE) 
	$(MAKE) -C $(BUILD_DIR) -f $(MAKEFILE) $(CFG)
	@erase /F $(DEST_MAKEFILE) 2>> nul

FINAL: $(DEST_MAKEFILE)
	$(MAKE) -C $(BUILD_DIR) -f $(MAKEFILE) $(CFG)_install
	@erase /F $(DEST_MAKEFILE) 2>> nul

FREEZE LIB CLEANLIB RESOURCE RELEASABLES SAVESPACE : 
