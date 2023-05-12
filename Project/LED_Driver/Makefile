KDIR = /lib/modules/$(shell uname -r)/build
<<<<<<< HEAD
MODULE_NAME = LED_Driver
=======
MOULDE_NAME = LED_Driver
CC = clang
>>>>>>> 128fd28fae70afcdb06250493ddb6cbceeb16a38
obj-m := $(MODULE_NAME).o
all:	driver
driver:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
