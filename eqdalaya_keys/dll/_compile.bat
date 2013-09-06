g++ --version
g++ dll.cpp -Id:\code\_libraries\boost\include -Id:\code\memory\include -Id:\code\everquest\eqdalaya\include -Ld:\code\_libraries\boost\lib -lkernel32 -luser32 -lgdi32 -lpsapi -Wl,-subsystem,windows -w -O0 -s -shared -o release/eqdalaya_keys.dll
copy release\eqdalaya_keys.dll c:\eqdalaya\
pause