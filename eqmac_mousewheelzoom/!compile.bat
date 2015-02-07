gcc --version
windres resource.rc resource.o
gcc eqmac_mousewheelzoom.c -lcomctl32 -lcomdlg32 -lgdi32 -lkernel32 -lole32 -lshell32 -luser32 -lpsapi -Wl,-subsystem,windows -w -O0 -s -o release/eqmac_mousewheelzoom.exe resource.o
del resource.o
pause