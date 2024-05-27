struct vs_input
{
  float3 Pos    : POSITION;
  float3 Normal : NORMAL;
  float2 UV     : TEXCOORD;
  float3 Color  : COLOR;
};

struct ps_input
{
  float4 Pos   : SV_POSITION;
  float4 Color : COLOR;
  float2 UV    : TEXCOORD;
};

cbuffer cbuffer0 : register(b0)
{
  float4x4 M;
  float4 Color;
}

ps_input VS(vs_input Input)
{
  ps_input Output;
  Output.Pos = mul(float4(Input.Pos, 1), M);
  Output.Pos.z = 0;
  Output.Color = float4(Input.Color, 1)*Color;
  Output.UV = Input.UV;
  
  return Output;
}

sampler sampler0 : register(s0);
Texture2D<float4> texture0 : register(t0);

float4 PS(ps_input Input) : SV_TARGET
{
  float4 Tex = texture0.Sample(sampler0, Input.UV);
  return Tex*Input.Color;
  
  return Tex*Input.Color;
}