g++ --version
g++ -D FREEGLUT_STATIC eqmac_map.cpp -Id:\code\_libraries\boost\include -Id:\code\_libraries\freeglut\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -Ld:\code\_libraries\boost\lib -Ld:\code\_libraries\freeglut\lib -static -lfreeglut_static -lglu32 -lopengl32 -lwinmm -lgdi32 -lkernel32 -luser32 -lpsapi -Wl,--enable-auto-import -Wl,-subsystem,windows -Wall -O0 -g -o release/eqmac_map.exe
pause
