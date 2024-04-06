struct vs_input
{
  float3 Pos    : POSITION;
  float3 Normal : NORMAL;
  float2 UV     : TEXCOORD;
  float3 Color  : COLOR;
};

struct ps_input
{
  float4 Pos          : SV_POSITION;
  float4 Color        : COLOR;
};

cbuffer cbuffer0 : register(b0)
{
  float4x4 M;
  float4x4 TransformM;
  float4 Color;
}

ps_input VS(vs_input Input)
{
  ps_input Output;
  Output.Pos = mul(float4(Input.Pos, 1), M);
  Output.Color = Color;
  
  return Output;
}

cbuffer cbuffer1 : register(b1)
{
  float AmbientStrength;
  float3 LightDirection;
  float3 LightColor;
  
  float Padding;
  float3 CameraPosition;
}

sampler sampler0 : register(s0);

Texture2D<float4> texture0 : register(t0);

float4 PS(ps_input Input) : SV_TARGET
{
  return Input.Color;
}