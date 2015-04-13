# Makefile: servo
#

MODULENAME 	:= servo


KDIR		:= /home/mark/bb-kernel/KERNEL
PWD		:= $(shell pwd)

CC		:= ${CROSS_COMPILE}gcc -Wall

ifeq ($(KERNELRELEASE),)
all:
	$(MAKE) V=0 -C $(KDIR) SUBDIRS=$(PWD) modules
else
  obj-m	:= $(MODULENAME).o
endif


clean:
	-rm -f *.o *.ko modules.order Module.symvers *.mod.c .*.cmd
	-rm -rf .tmp_versions

copy:
	scp servo.ko root@192.168.0.81:

