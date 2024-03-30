@echo off
if not exist ..\build mkdir ..\build
pushd ..\build

set VSOptions=/T vs_5_0 /E VS
set PSOptions=/T ps_5_0 /E PS
set Options=/Ges /nologo /Od /WX /Zi /Zpr /Qstrip_reflect /Qstrip_debug /Qstrip_priv

echo "Compiling New Horizons shaders"
del *.fxc
fxc %VSOptions% /Fo shader_vs.fxc %Options% w:\new-horizons\code\shaders.hlsl
fxc %PSOptions% /Fo shader_ps.fxc %Options% w:\new-horizons\code\shaders.hlsl
echo "Done compiling New Horizons shaders"

popd