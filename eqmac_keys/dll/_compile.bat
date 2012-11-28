g++ --version
g++ dll.cpp -lkernel32 -luser32 -lgdi32 -lpsapi -Wl,-subsystem,windows -Wall -O0 -s -shared -o release/eqmac_keys.dll
pause