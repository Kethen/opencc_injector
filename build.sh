set -xe
gcc -g -fPIC -c opencc_injector.c -I/usr/include/opencc -o opencc_injector.o
gcc -g -shared -o libopencc_injector.so opencc_injector.o -ldl -lopencc -Wl,--version-script=opencc_injector.ver
