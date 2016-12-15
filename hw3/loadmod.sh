make clean
make
cp blacklist /tmp/blacklist
cp antivirus_test.txt /tmp/antivirus_test.txt 
gcc call_fork.c -o call_fork
gcc user_ioctl.c -o user
gcc org_clone.c -o org_clone 
gcc newclone.c -o newclone
gcc test.c -o test
gcc antivirusTest.c -o antivirusTest
gcc test_protected.c -o test_protected 
insmod hw3.ko
insmod modules/trace_module.ko
insmod modules/protect_module.ko
insmod modules/anti_module.ko
insmod sys_clone2.ko
