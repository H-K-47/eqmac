g++ --version
g++ -D FREEGLUT_STATIC eqmac_map.cpp -Wl,-subsystem,windows -w -O0 -s -o release/eqmac_map.exe -Lc:\mingw\lib -Ic:\mingw\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -static -lfreeglut_static -lglu32 -lopengl32 -lwinmm -lgdi32 -lkernel32 -luser32 -lpsapi
pause
