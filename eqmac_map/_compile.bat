g++ --version
g++ eqmac_map.cpp -D FREEGLUT_STATIC -Id:\code\boost\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -Ld:\code\boost\lib -lfreeglut_static -lglu32 -lopengl32 -lwinmm -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,--enable-auto-import -Wl,-subsystem,windows -Wall -O0 -s -o release/eqmac_map.exe
pause
