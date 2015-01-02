g++ --version
windres eqmac_play_nice.rc resource.o
g++ eqmac_play_nice.cpp -Id:\code\_libraries\boost\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -Ld:\code\_libraries\boost\lib -lcomctl32 -lcomdlg32 -lgdi32 -lkernel32 -lole32 -lshell32 -luser32 -lpsapi -Wl,-subsystem,windows -w -O0 -s -o release/eqmac_play_nice.exe resource.o
del resource.o
pause