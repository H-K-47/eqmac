gcc --version
gcc dll.c -lkernel32 -luser32 -lgdi32 -lpsapi -Wl,-subsystem,windows -Wall -O0 -s -shared -o release/eqmac_keys.dll
pause