
# for each lithtech component do a make, exe's last
all client standalone clean install depend: 
	$(MAKE) -C io/src $@
ifeq ($(TARGET_PLATFORM), __PS2)
	$(MAKE) -C io/src/sys/ps2_iop $@
endif
	$(MAKE) -C mem/src $@
	$(MAKE) -C net/src $@
	$(MAKE) -C toport/src $@
	$(MAKE) -C src $@
