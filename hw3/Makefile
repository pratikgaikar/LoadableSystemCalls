obj-m = modules/trace_module.o  modules/protect_module.o  modules/anti_module.o sys_clone2.o 

obj-m +=hw3.o

hw3-y := syscall_new.o main.o bitmap.o syscall_intercept.o ioctl_device.o 

INC=/lib/modules/$(shell uname -r)/build/arch/x86/include

all: clean hw3

hw3:
	        make -Wall -Werror -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
