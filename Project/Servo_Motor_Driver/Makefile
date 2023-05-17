KDIR = /lib/modules/$(shell uname -r)/build
MODULE_NAME = motor_driver
obj-m := $(MODULE_NAME).o

all:	driver

driver:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
