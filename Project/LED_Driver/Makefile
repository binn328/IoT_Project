KDIR = /lib/modules/$(shell uname -r)/build
MODULE_NAME = LED_Driver
obj-m := $(MODULE_NAME).o
all:	driver
driver:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
