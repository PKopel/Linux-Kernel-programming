obj-m := linked_buffer.o

debugKDIR  := ../../../../cw02/uml/linux-5.9.6-debug
KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE)  -C $(KDIR) M=$(PWD) clean

debug:
	$(MAKE) -C $(debugKDIR) M=$(PWD) modules

clean-debug:
	$(MAKE)  -C $(debugKDIR) M=$(PWD) clean
