g++ --version
g++ eqmac_test.cpp -Id:\code\boost\include -Ld:\code\boost\lib -Id:\code\memory\include -Id:\code\everquest\eqmac\include -lfreeglut -lglu32 -lopengl32 -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,--enable-auto-import -Wl,-subsystem,console -Wall -O0 -s -o release/eqmac_test.exe
pause
