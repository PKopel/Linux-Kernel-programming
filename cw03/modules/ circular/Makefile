obj-m := circular.o

KDIR  := ../../../cw02/uml/linux-5.9.6
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=um modules

clean:
	$(MAKE)  -C $(KDIR) M=$(PWD) clean