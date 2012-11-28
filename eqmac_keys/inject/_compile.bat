gcc --version
gcc inject.c -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,-subsystem,windows -Wall -O0 -s -o release/eqmac_keys.exe
gcc test.c -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,-subsystem,console -Wall -O0 -s -o test.exe
pause