KDIR = /lib/modules/$(shell uname -r)/build
MOULDE_NAME = LED_Driver
CC = clang
obj-m := $(MODULE_NAME).o
all:	driver
driver:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
