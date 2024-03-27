@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

set CompilerFlags=-GS- -UTF8 -MTd -nologo -Gm- -EHa- -Od -Oi -WX -W4 -wd4221 -wd4505 -wd4201 -wd4100 -wd4189 -wd4115 -wd4101 -wd4996 -wd4702 -wd4701 -wd4703 -FAsc -Z7 -DUNICODE=1 -DHORIZONS_INTERNAL=1 -DHORIZONS_DEBUG=1
set CommonLinkerFlags=-incremental:no -opt:ref
set LinkerFlags=kernel32.lib user32.lib gdi32.lib winmm.lib dxguid.lib dxgi.lib d3d11.lib shlwapi.lib

echo "Compiling New Horizons game code"
del *.pdb > NUL 2> NUL
echo WAITING FOR PDB > lock.tmp
cl %CompilerFlags% w:\new-horizons\code\horizons.c -LD /link %CommonLinkerFlags% -Export:GameUpdateAndRender
del lock.tmp
echo "Done compiling New Horizons game code"

echo -

echo "Compiling New Horizons shaders"
fxc /T vs_5_0 /E VS /Fo shader_vs.fxc /Ges /nologo /Od /WX /Zi /Zpr /Qstrip_reflect /Qstrip_debug /Qstrip_priv w:\new-horizons\code\shaders.hlsl
fxc /T ps_5_0 /E PS /Fo shader_ps.fxc /Ges /nologo /Od /WX /Zi /Zpr /Qstrip_reflect /Qstrip_debug /Qstrip_priv w:\new-horizons\code\shaders.hlsl
echo "Done compiling New Horizons shaders"

echo -

echo "Compiling New Horizons platform code"
cl -Gs9999999 %CompilerFlags% w:\new-horizons\code\win32.c /link -subsystem:windows -nodefaultlib %CommonLinkerFlags% %LinkerFlags%
echo "Done compiling New Horizons platform code"

popd
