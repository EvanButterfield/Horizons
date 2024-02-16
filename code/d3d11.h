/* date = February 5th 2024 8:24 am */

#ifndef D3D11_H
#define D3D11_H

#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

typedef struct d3d11_state
{
  ID3D11Device *Device;
  ID3D11DeviceContext *Context;
  
  IDXGISwapChain1 *SwapChain;
  ID3D11Buffer *VBuffer;
  ID3D11Buffer *IBuffer;
  
  ID3D11InputLayout *Layout;
  ID3D11VertexShader *VShader;
  ID3D11PixelShader *PShader;
  ID3D11Buffer *UBuffer;
  
  ID3D11SamplerState* Sampler;
  ID3D11BlendState* BlendState;
  ID3D11RasterizerState* RasterizerState;
  ID3D11DepthStencilState *DepthState;
  
  ID3D11RenderTargetView *RTView;
  ID3D11DepthStencilView *DSView;
} d3d11_state;

typedef struct d3d11_sprite
{
  ID3D11ShaderResourceView *TextureView;
} d3d11_sprite;

#endif //D3D11_H
