@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

set CompilerFlags=-GS- -UTF8 -MTd -nologo -Gm- -EHa- -Od -Oi -WX -W4 -wd4221 -wd4505 -wd4201 -wd4100 -wd4189 -wd4115 -wd4101 -wd4996 -wd4702 -wd4701 -wd4703 -FAsc -Z7 -DUNICODE=1 -DHORIZONS_INTERNAL=1 -DHORIZONS_DEBUG=1
set LinkerFlags=-incremental:no -opt:ref

echo "Compiling New Horizons game code"
del horizons*.pdb > NUL 2> NUL
echo WAITING FOR PDB > lock.tmp
cl %CompilerFlags% w:\new-horizons\code\horizons.c -LD /link %LinkerFlags% -Export:GameUpdateAndRender
del lock.tmp
echo "Done compiling New Horizons game code"

echo -

echo "Compiling New Horizons shaders"
rem /Qstrip_reflect /Qstrip_debug
fxc /T vs_5_0 /E VS /Fo shader_vs.fxc /Ges /nologo /Od /WX /Zi /Zpr /Qstrip_priv  w:\new-horizons\code\shaders.hlsl
fxc /T ps_5_0 /E PS /Fo shader_ps.fxc /Ges /nologo /Od /WX /Zi /Zpr /Qstrip_priv w:\new-horizons\code\shaders.hlsl
echo "Done compiling New Horizons shaders"

popd
