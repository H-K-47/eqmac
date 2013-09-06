gcc --version
gcc inject.c -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,-subsystem,windows -Wall -O0 -s -o release/eqdalaya_keys.exe
copy release\eqdalaya_keys.exe c:\eqdalaya\
pause