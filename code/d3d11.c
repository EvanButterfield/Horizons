#include "d3d11.h"

// TODO(evan): Create my own structures for this
typedef struct vertex
{
  float Position[2];
  float UV[2];
  float Color[3];
} vertex;

// Thanks to mmozeiko for the D3D11 code
// https://gist.github.com/mmozeiko/5e727f845db182d468a34d524508ad5f

internal d3d11_state
InitD3D11(HWND Window, platform_api *Platform, memory_arena *TempArena)
{
  HRESULT Result;
  
  ID3D11Device *Device;
  ID3D11DeviceContext *Context;
  
  // Create device and context
  {
    UINT Flags = 0;
    
#if HORIZONS_DEBUG
    Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    
    D3D_FEATURE_LEVEL Levels[] = {D3D_FEATURE_LEVEL_11_0};
    
    Result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0,
                               Flags, Levels, ArrayCount(Levels),
                               D3D11_SDK_VERSION, &Device, 0, &Context);
    
    AssertHR(Result);
  }
  
#ifdef HORIZONS_DEBUG
  // Set up debug break on errors
  {
    ID3D11InfoQueue *Info;
    ID3D11Device_QueryInterface(Device, &IID_ID3D11InfoQueue, &Info);
    ID3D11InfoQueue_SetBreakOnSeverity(Info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    ID3D11InfoQueue_SetBreakOnSeverity(Info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
    ID3D11InfoQueue_Release(Info);
    
    IDXGIInfoQueue *DXGIInfo;
    Result = DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, &DXGIInfo);
    AssertHR(Result);
    IDXGIInfoQueue_SetBreakOnSeverity(DXGIInfo, DXGI_DEBUG_ALL,
                                      DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,
                                      TRUE);
    IDXGIInfoQueue_SetBreakOnSeverity(DXGIInfo, DXGI_DEBUG_ALL,
                                      DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,
                                      TRUE);
    IDXGIInfoQueue_Release(DXGIInfo);
    
    // NOTE(evan): No need to check results after this because it will automatically
    //       break on errors anyways
  }
#endif
  
  // Create swap chain
  IDXGISwapChain1 *SwapChain;
  {
    IDXGIDevice *DXGIDevice;
    ID3D11Device_QueryInterface(Device, &IID_IDXGIDevice, &DXGIDevice);
    
    IDXGIAdapter *DXGIAdapter;
    IDXGIDevice_GetAdapter(DXGIDevice, &DXGIAdapter);
    
    IDXGIFactory2 *Factory;
    IDXGIAdapter_GetParent(DXGIAdapter, &IID_IDXGIFactory2, &Factory);
    
    DXGI_SWAP_CHAIN_DESC1 Desc =
    {
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc = {1, 0},
      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount = 2,
      .Scaling = DXGI_SCALING_NONE,
      .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
    };
    
    IDXGIFactory2_CreateSwapChainForHwnd(Factory, (IUnknown *)Device,
                                         Window, &Desc, 0, 0, &SwapChain);
    
    // Disable weird Alt+Enter behavior
    IDXGIFactory_MakeWindowAssociation(Factory, Window, DXGI_MWA_NO_ALT_ENTER);
    
    IDXGIFactory2_Release(Factory);
    IDXGIAdapter_Release(DXGIAdapter);
    IDXGIDevice_Release(DXGIDevice);
  }
  
  ID3D11Buffer *VBuffer;
  {
    vertex Data[] =
    {
      // NOTE(evan): Flip the UV because all of our images will be upside down
      { { +0.50f, -0.50f }, { 1, 1 }, { 1, 1, 1 } }, // Bottom right
      { { -0.50f, -0.50f }, { 0, 1 }, { 1, 1, 1 } }, // Bottom left
      { { -0.50f, +0.50f }, { 0, 0 }, { 1, 1, 1 } }, // Top left
      { { +0.50f, +0.50f }, { 1, 0 }, { 1, 1, 1 } }  // Top right
    };
    
    D3D11_BUFFER_DESC Desc =
    {
      .ByteWidth = sizeof(Data),
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_VERTEX_BUFFER
    };
    
    D3D11_SUBRESOURCE_DATA Initial = {.pSysMem = Data};
    ID3D11Device_CreateBuffer(Device, &Desc, &Initial, &VBuffer);
  }
  
  ID3D11Buffer *IBuffer;
  {
    u32 Data[] =
    {
      0, 1, 2,
      3, 0, 2
    };
    
    D3D11_BUFFER_DESC Desc =
    {
      .ByteWidth = sizeof(Data),
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_INDEX_BUFFER
    };
    
    D3D11_SUBRESOURCE_DATA Initial = {.pSysMem = Data};
    ID3D11Device_CreateBuffer(Device, &Desc, &Initial, &IBuffer);
  }
  
  ID3D11Buffer *UBuffer;
  {
    D3D11_BUFFER_DESC Desc =
    {
      // NOTE(evan): Basic shader has 1 4x4 matrix
      .ByteWidth = 4*4*sizeof(f32),
      .Usage = D3D11_USAGE_DYNAMIC,
      .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
      .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
    };
    
    ID3D11Device_CreateBuffer(Device, &Desc, 0, &UBuffer);
  }
  
  ID3D11InputLayout *Layout;
  ID3D11VertexShader *VShader;
  ID3D11PixelShader *PShader;
  {
    D3D11_INPUT_ELEMENT_DESC Desc[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(vertex, UV),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
      { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    string8 VSString = CreateString("shader_vs.fxc", TempArena, Platform);
    platform_file_handle VSHandle = Platform->OpenFile(VSString, false, FILE_OPEN_READ);
    u32 VSSize = Platform->GetFileSize(VSHandle);
    void *VSData = PushSize(TempArena, VSSize);
    Platform->ReadEntireFile(VSHandle, VSSize, VSData);
    Platform->CloseFile(VSHandle);
    
    string8 PSString = CreateString("shader_ps.fxc", TempArena, Platform);
    platform_file_handle PSHandle = Platform->OpenFile(PSString, false, FILE_OPEN_READ);
    u32 PSSize = Platform->GetFileSize(PSHandle);
    void *PSData = PushSize(TempArena, PSSize);
    Platform->ReadEntireFile(PSHandle, PSSize, PSData);
    Platform->CloseFile(PSHandle);
    
    ID3D11Device_CreateVertexShader(Device, VSData, VSSize, 0, &VShader);
    ID3D11Device_CreatePixelShader(Device, PSData, PSSize, 0, &PShader);
    ID3D11Device_CreateInputLayout(Device, Desc, ArrayCount(Desc),
                                   VSData, VSSize, &Layout);
  }
  
  ID3D11SamplerState* Sampler;
  {
    D3D11_SAMPLER_DESC Desc =
    {
      .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
      .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
      .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
      .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
      .MipLODBias = 0,
      .MaxAnisotropy = 1,
      .MinLOD = 0,
      .MaxLOD = D3D11_FLOAT32_MAX,
    };
    
    ID3D11Device_CreateSamplerState(Device, &Desc, &Sampler);
  }
  
  ID3D11BlendState* BlendState;
  {
    // enable alpha blending
    D3D11_BLEND_DESC Desc =
    {
      .RenderTarget[0] =
      {
        .BlendEnable = TRUE,
        .SrcBlend = D3D11_BLEND_SRC_ALPHA,
        .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
        .BlendOp = D3D11_BLEND_OP_ADD,
        .SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
        .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
      },
    };
    
    ID3D11Device_CreateBlendState(Device, &Desc, &BlendState);
  }
  
  ID3D11RasterizerState* RasterizerState;
  {
    // disable culling
    D3D11_RASTERIZER_DESC Desc =
    {
      .FillMode = D3D11_FILL_SOLID,
      .CullMode = D3D11_CULL_NONE,
      .DepthClipEnable = TRUE,
    };
    
    ID3D11Device_CreateRasterizerState(Device, &Desc, &RasterizerState);
  }
  
  ID3D11DepthStencilState* DepthState;
  {
    D3D11_DEPTH_STENCIL_DESC Desc =
    {
      .DepthEnable = FALSE,
      .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
      .DepthFunc = D3D11_COMPARISON_LESS,
      .StencilEnable = FALSE,
      .StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
      .StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
      // .FrontFace = ... 
      // .BackFace = ...
    };
    
    ID3D11Device_CreateDepthStencilState(Device, &Desc, &DepthState);
  }
  
  d3d11_state State =
  {
    Device, Context,
    SwapChain,
    VBuffer, IBuffer,
    Layout, VShader, PShader, UBuffer,
    Sampler, BlendState, RasterizerState, DepthState,
    0, 0
  };
  return(State);
}

internal d3d11_sprite
D3D11CreateSprite(d3d11_state *State,
                  u32 *Texture, u32 TextureWidth, u32 TextureHeight)
{
  d3d11_sprite Result;
  
  // Texture
  {
    D3D11_TEXTURE2D_DESC Desc =
    {
      .Width = TextureWidth,
      .Height = TextureHeight,
      .MipLevels = 1,
      .ArraySize = 1,
      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc = {1, 0},
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_SHADER_RESOURCE
    };
    
    D3D11_SUBRESOURCE_DATA Data =
    {
      .pSysMem = Texture,
      .SysMemPitch = TextureWidth*sizeof(u32)
    };
    
    ID3D11Texture2D *Texture2D;
    ID3D11Device_CreateTexture2D(State->Device, &Desc, &Data, &Texture2D);
    ID3D11Device_CreateShaderResourceView(State->Device, (ID3D11Resource *)Texture2D,
                                          0, &Result.TextureView);
    ID3D11Texture2D_Release(Texture2D);
  }
  
  return(Result);
}

internal void
D3D11Resize(d3d11_state *State, window_dimension New, platform_api *Platform)
{
  if(State->RTView)
  {
    ID3D11DeviceContext_ClearState(State->Context);
    ID3D11RenderTargetView_Release(State->RTView);
    ID3D11DepthStencilView_Release(State->DSView);
    
    State->RTView = 0;
  }
  
  HRESULT Result;
  if(New.Width != 0 && New.Height != 0)
  {
    Result = IDXGISwapChain1_ResizeBuffers(State->SwapChain, 0,
                                           New.Width, New.Height,
                                           DXGI_FORMAT_UNKNOWN, 0);
    if(FAILED(Result))
    {
      Platform->LogMessagePlain("Failed to resize swap chain\n", false,
                                MESSAGE_SEVERITY_ERROR);
      Assert(0);
    }
    
    ID3D11DeviceContext_RSSetState(State->Context, State->RasterizerState);
    ID3D11DeviceContext_OMSetBlendState(State->Context, State->BlendState, 0, ~0U);
    ID3D11DeviceContext_OMSetDepthStencilState(State->Context, State->DepthState, 0);
    ID3D11DeviceContext_PSSetShader(State->Context, State->PShader, 0, 0);
    ID3D11DeviceContext_VSSetShader(State->Context, State->VShader, 0, 0);
    
    ID3D11Texture2D *BackBuffer;
    IDXGISwapChain1_GetBuffer(State->SwapChain, 0, &IID_ID3D11Texture2D, &BackBuffer);
    ID3D11Device_CreateRenderTargetView(State->Device, (ID3D11Resource *)BackBuffer,
                                        0, &State->RTView);
    ID3D11Texture2D_Release(BackBuffer);
    
    D3D11_TEXTURE2D_DESC Desc =
    {
      .Width = New.Width,
      .Height = New.Height,
      .MipLevels = 1,
      .ArraySize = 1,
      .Format = DXGI_FORMAT_D32_FLOAT,
      .SampleDesc = {1, 0},
      .Usage = D3D11_USAGE_DEFAULT,
      .BindFlags = D3D11_BIND_DEPTH_STENCIL
    };
    
    ID3D11Texture2D *Depth;
    ID3D11Device_CreateTexture2D(State->Device, &Desc, 0, &Depth);
    ID3D11Device_CreateDepthStencilView(State->Device, (ID3D11Resource *)Depth,
                                        0, &State->DSView);
    ID3D11Texture2D_Release(Depth);
    
    ID3D11DeviceContext_IASetPrimitiveTopology(State->Context,
                                               D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    D3D11_VIEWPORT Viewport =
    {
      .Width = (f32)New.Width,
      .Height = (f32)New.Height,
      .MaxDepth = 1
    };
    
    ID3D11DeviceContext_RSSetViewports(State->Context, 1, &Viewport);
    
    ID3D11DeviceContext_PSSetSamplers(State->Context, 0, 1, &State->Sampler);
  }
}

internal void
D3D11StartFrame(d3d11_state *State)
{
  if(State->RTView)
  {
    f32 Color[] = {0.392f, 0.584f, 0.929f, 1.f};
    ID3D11DeviceContext_ClearRenderTargetView(State->Context, State->RTView, Color);
    ID3D11DeviceContext_ClearDepthStencilView(State->Context, State->DSView,
                                              D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL,
                                              1.f, 0);
    
    ID3D11DeviceContext_IASetInputLayout(State->Context, State->Layout);
    u32 Stride = sizeof(vertex);
    u32 Offset = 0;
    ID3D11DeviceContext_IASetVertexBuffers(State->Context, 0, 1, &State->VBuffer,
                                           &Stride, &Offset);
    ID3D11DeviceContext_IASetIndexBuffer(State->Context, State->IBuffer,
                                         DXGI_FORMAT_R32_UINT, 0);
    
    ID3D11DeviceContext_OMSetRenderTargets(State->Context, 1,
                                           &State->RTView, State->DSView);
  }
}

internal void
D3D11DrawSprite(d3d11_state *State, d3d11_sprite *Sprite, f32 *Matrix,
                window_dimension Dimension, platform_api *Platform)
{
  D3D11_MAPPED_SUBRESOURCE Mapped;
  ID3D11DeviceContext_Map(State->Context, (ID3D11Resource *)State->UBuffer,
                          0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
  Platform->CopyMemory(Mapped.pData, Matrix, 4*4*sizeof(f32));
  ID3D11DeviceContext_Unmap(State->Context,
                            (ID3D11Resource *)State->UBuffer, 0);
  
  ID3D11DeviceContext_VSSetConstantBuffers(State->Context, 0, 1,
                                           &State->UBuffer);
  
  ID3D11DeviceContext_PSSetShaderResources(State->Context, 0, 1,
                                           &Sprite->TextureView);
  
  ID3D11DeviceContext_DrawIndexed(State->Context, 6, 0, 0);
}

internal void
D3D11EndFrame(d3d11_state *State, platform_api *Platform)
{
  b32 VSync = true;
  HRESULT Result = IDXGISwapChain1_Present(State->SwapChain, VSync, 0);
  if(Result == DXGI_STATUS_OCCLUDED)
  {
    if(VSync)
    {
      Platform->Sleep(10);
    }
  }
  else if(FAILED(Result))
  {
    Platform->LogMessagePlain("Failed to present swap chain\n", false,
                              MESSAGE_SEVERITY_ERROR);
    Assert(0);
  }
}