@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

set CompilerFlags=-GS- -Gs9999999 -UTF8 -MTd -nologo -Gm- -EHa- -Od -Oi -WX -W4 -wd4221 -wd4505 -wd4201 -wd4100 -wd4189 -wd4115 -wd4101 -wd4996 -FAsc -Z7 -DUNICODE=1 -DHORIZONS_INTERNAL=1
set LinkerFlags=-nodefaultlib -incremental:no -opt:ref kernel32.lib user32.lib gdi32.lib winmm.lib

del *.pdb > NUL 2> NUL
echo WAITING FOR PDB > lock.tmp
cl %CompilerFlags% w:\new-horizons\code\horizons.c -LD /link -nodefaultlib -incremental:no -opt:ref -Export:GameUpdateAndRender
del lock.tmp
cl %CompilerFlags% w:\new-horizons\code\win32.c /link -subsystem:windows %LinkerFlags%
popd
