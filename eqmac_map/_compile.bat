g++ --version
g++ -D FREEGLUT_STATIC eqmac_map.cpp -Id:\code\boost\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -Ld:\code\boost\lib -static -lfreeglut_static -lglu32 -lopengl32 -lwinmm -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,--enable-auto-import -Wl,-subsystem,windows -Wall -O0 -s -o release/eqmac_map.exe
pause
