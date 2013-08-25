gcc --version
gcc inject.c -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,-subsystem,windows -Wall -O0 -s -o release/eqmac_buff_timers.exe
pause