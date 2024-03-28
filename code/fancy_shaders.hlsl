struct vs_input
{
  float2 Pos   : POSITION;
  float2 UV    : TEXCOORD;
  float3 Color : COLOR;
};

struct ps_input
{
  float4 Pos   : SV_POSITION;
  float2 UV    : TEXCOORD;
  float4 Color : COLOR;
};

cbuffer cbuffer0 : register(b0)
{
  float4x4 Transform;
}

sampler sampler0 : register(s0);

Texture2D<float4> texture0 : register(t0);

ps_input VS(vs_input Input)
{
  ps_input Output;
  Output.Pos = mul(float4(Input.Pos, 0, 1), Transform);
  Output.Pos.z = 1;
  Output.UV = Input.UV;
  Output.Color = float4(Input.Color, 1);
  
  return Output;
}

float4 PS(ps_input Input) : SV_TARGET
{
  float4 Tex = texture0.Sample(sampler0, Input.UV);
  return Input.Color*Tex*float4(0.5, 1, 0.5, 1);
}