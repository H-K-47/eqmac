g++ --version
g++ dll.cpp -Id:\code\_libraries\boost\include -Id:\code\vk_keys\include -Id:\code\memory\include -Id:\code\everquest\eqmac\include -Ld:\code\_libraries\boost\lib -lkernel32 -luser32 -lgdi32 -Wl,-subsystem,windows -w -O0 -s -shared -o release/eqmac_warp.dll
copy release\eqmac_warp.dll c:\eqmac\
copy release\eqmac_warp.ini c:\eqmac\
copy release\warps c:\eqmac\warps\
pause