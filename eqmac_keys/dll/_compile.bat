g++ --version
g++ dll.cpp -Id:\code\_libraries\boost\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -Ld:\code\_libraries\boost\lib -lkernel32 -luser32 -lgdi32 -lpsapi -Wl,-subsystem,windows -Wall -O0 -s -shared -o release/eqmac_keys.dll
pause