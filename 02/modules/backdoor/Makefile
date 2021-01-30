obj-m := backdoor.o

umKDIR  := ../../../cw02/uml/linux-5.9.6
KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

um:
	$(MAKE) -C $(umKDIR) M=$(PWD) ARCH=um modules

clean:
	$(MAKE)  -C $(KDIR) M=$(PWD) clean

clean-um:
	$(MAKE)  -C $(umKDIR) M=$(PWD) clean