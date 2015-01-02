gcc --version
gcc eqmac_inject_dll.c -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,-subsystem,console -Wall -O0 -s -o release/eqmac_inject_dll.exe
pause