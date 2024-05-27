#include "d3d11_horizons.h"

// Thanks to mmozeiko for the D3D11 code
// https://gist.github.com/mmozeiko/5e727f845db182d468a34d524508ad5f

internal d3d11_shader
D3D11CreateShader_(ID3D11Device *Device, s8 *Name,
                   memory_arena *TempArena, platform_api *Platform);
internal d3d11_sprite
D3D11CreateSprite_(ID3D11Device *Device,
                   u32 *Texture, u32 TextureWidth, u32 TextureHeight);
internal d3d11_mesh
D3D11CreateMesh_(ID3D11Device *Device,
                 vertex *VData, u32 VDataCount,
                 u32 *IData, u32 IDataLength);

internal d3d11_state
InitD3D11(HWND Window, platform_api *Platform,
          memory_arena *PermArena, memory_arena *TempArena)
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
  
  d3d11_mesh CubeMesh;
  vertex *CubeVData;
  s32 CubeVDataCount;
  // TODO(evan): UV Data for default cube
  {
    vertex VData_[] =
    {
      { {1, 1, -1}, {0, 0, -1}, {0, 0}, {1, 1, 1} },
      { {1, 1, -1}, {0, 1, 0}, {0, 0}, {1, 1, 1} },
      { {1, 1, -1}, {1, 0, 0}, {0, 0}, {1, 1, 1} },
      
      { {1, -1, -1}, {0, -1, 0}, {0, 0}, {1, 1, 1} },
      { {1, -1, -1}, {0, 0, -1}, {0, 0}, {1, 1, 1} },
      { {1, -1, -1}, {1, 0, 0}, {0, 0}, {1, 1, 1} },
      
      { {1, 1, 1}, {0, 0, 1}, {0, 0}, {1, 1, 1} },
      { {1, 1, 1}, {0, 1, 0}, {0, 0}, {1, 1, 1} },
      { {1, 1, 1}, {1, 0, 0}, {0, 0}, {1, 1, 1} },
      
      { {1, -1, 1}, {0, -1, 0}, {0, 0}, {1, 1, 1} },
      { {1, -1, 1}, {0, 0, 1}, {0, 0}, {1, 1, 1} },
      { {1, -1, 1}, {1, 0, 0}, {0, 0}, {1, 1, 1} },
      
      { {-1, 1, -1}, {-1, 0, 0}, {0, 0}, {1, 1, 1} },
      { {-1, 1, -1}, {0, 0, -1}, {0, 0}, {1, 1, 1} },
      { {-1, 1, -1}, {0, 1, 0}, {0, 0}, {1, 1, 1} },
      
      { {-1, -1, -1}, {-1, 0, 0}, {0, 0}, {1, 1, 1} },
      { {-1, -1, -1}, {0, -1, 0}, {0, 0}, {1, 1, 1} },
      { {-1, -1, -1}, {0, 0, -1}, {0, 0}, {1, 1, 1} },
      
      { {-1, 1, 1}, {-1, 0, 0}, {0, 0}, {1, 1, 1} },
      { {-1, 1, 1}, {0, 0, 1}, {0, 0}, {1, 1, 1} },
      { {-1, 1, 1}, {0, 1, 0}, {0, 0}, {1, 1, 1} },
      
      { {-1, -1, 1}, {-1, 0, 0}, {0, 0}, {1, 1, 1} },
      { {-1, -1, 1}, {0, -1, 0}, {0, 0}, {1, 1, 1} },
      { {-1, -1, 1}, {0, 0, 1}, {0, 0}, {1, 1, 1} },
    };
    CubeVDataCount = ArrayCount(VData_);
    CubeVData = PushArray(PermArena, vertex, CubeVDataCount);
    Platform->CopyMemory(CubeVData, VData_, sizeof(vertex)*CubeVDataCount);
    
    u32 IData[] =
    {
      1, 14, 20, 1, 20, 7,
      10, 6, 19, 10, 19, 23,
      21, 18, 12, 21, 12, 15,
      16, 3, 9, 16, 9, 22,
      5, 2, 8, 5, 8, 11,
      17, 13, 0, 17, 0, 4,
    };
    
    CubeMesh = D3D11CreateMesh_(Device, CubeVData, CubeVDataCount,
                                IData, ArrayCount(IData));
  }
  
  d3d11_mesh QuadMesh;
  vertex *QuadVData;
  s32 QuadVDataCount;
  {
    vertex VData[] =
    {
      { {1, 1, 0}, {0, 0, 0}, {1, 1}, {1, 1, 1} },
      { {1, -1, 0}, {0, 0, 0}, {1, 0}, {1, 1, 1} },
      { {-1, -1, 0}, {0, 0, 0}, {0, 0}, {1, 1, 1} },
      { {-1, 1, 0}, {0, 0, 0}, {0, 1}, {1, 1, 1} },
    };
    QuadVDataCount = ArrayCount(VData);
    QuadVData = PushArray(PermArena, vertex, QuadVDataCount);
    Platform->CopyMemory(QuadVData, VData, sizeof(vertex)*QuadVDataCount);
    
    u32 IData[] =
    {
      0, 1, 2,
      2, 3, 0,
    };
    
    QuadMesh = D3D11CreateMesh_(Device, QuadVData, QuadVDataCount,
                                IData, ArrayCount(IData));
  }
  
  ID3D11Buffer *VSConstantsBuffer;
  ID3D11Buffer *PSConstantsBuffer;
  ID3D11Buffer *VSConstantsBuffer2D;
  {
    D3D11_BUFFER_DESC Desc =
    {
      .Usage = D3D11_USAGE_DYNAMIC,
      .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
      .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
    };
    
    Desc.ByteWidth = (UINT)AlignTo(sizeof(vs_shader_constants), 16);
    ID3D11Device_CreateBuffer(Device, &Desc, 0, &VSConstantsBuffer);
    
    Desc.ByteWidth = (UINT)AlignTo(sizeof(ps_shader_constants), 16);
    ID3D11Device_CreateBuffer(Device, &Desc, 0, &PSConstantsBuffer);
    
    Desc.ByteWidth = (UINT)AlignTo(sizeof(vs_shader_constants_2d), 16);
    ID3D11Device_CreateBuffer(Device, &Desc, 0, &VSConstantsBuffer2D);
  }
  
  d3d11_shader Shader = D3D11CreateShader_(Device, "shader", TempArena, Platform);
  if(Shader.VShader == 0)
  {
    Platform->LogMessagePlain("Invalid shader file\n", false, MESSAGE_SEVERITY_ERROR);
    return((d3d11_state){0});
  }
  
  d3d11_sprite Sprite;
  {
    u32 Data[] =
    {
      0xFFFFFFFF
    };
    
    Sprite = D3D11CreateSprite_(Device, Data, 1, 1);
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
      .MaxLOD = D3D11_FLOAT32_MAX
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
        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
      }
    };
    
    ID3D11Device_CreateBlendState(Device, &Desc, &BlendState);
  }
  
  ID3D11RasterizerState* RSSolid;
  ID3D11RasterizerState* RSWireframe;
  ID3D11RasterizerState *RS2D;
  {
    D3D11_RASTERIZER_DESC Desc =
    {
      .FillMode = D3D11_FILL_SOLID,
      .CullMode = D3D11_CULL_BACK,
      .DepthClipEnable = TRUE,
    };
    
    ID3D11Device_CreateRasterizerState(Device, &Desc, &RSSolid);
    
    Desc.FillMode = D3D11_FILL_WIREFRAME;
    Desc.CullMode = D3D11_CULL_NONE;
    ID3D11Device_CreateRasterizerState(Device, &Desc, &RSWireframe);
    
    Desc.FillMode = D3D11_FILL_SOLID;
    ID3D11Device_CreateRasterizerState(Device, &Desc, &RS2D);
  }
  
  ID3D11DepthStencilState* DepthState;
  {
    D3D11_DEPTH_STENCIL_DESC Desc =
    {
      .DepthEnable = TRUE,
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
    .Device = Device,
    .Context = Context,
    
    .SwapChain = SwapChain,
    
    .VSConstantsBuffer = VSConstantsBuffer,
    .PSConstantsBuffer = PSConstantsBuffer,
    .VSConstantsBuffer2D = VSConstantsBuffer2D,
    
    .DefaultShader = Shader,
    .CurrentShader = Shader,
    .DefaultSprite = Sprite,
    .CurrentSprite = Sprite,
    .DefaultQuadMesh = QuadMesh,
    .DefaultQuadMeshVertices = QuadVData,
    .DefaultQuadMeshVertexCount = QuadVDataCount,
    .DefaultCubeMesh = CubeMesh,
    .DefaultCubeMeshVertices = CubeVData,
    .DefaultCubeMeshVertexCount = CubeVDataCount,
    .CurrentMesh = CubeMesh,
    
    .Sampler = Sampler,
    .BlendState = BlendState,
    .RSSolid = RSSolid,
    .RSWireframe = RSWireframe,
    .CurrentRS = RSSolid,
    .RS2D = RS2D,
    .DepthState = DepthState
  };
  return(State);
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
    
    ID3D11DeviceContext_RSSetState(State->Context, State->CurrentRS);
    ID3D11DeviceContext_OMSetBlendState(State->Context, State->BlendState, 0, ~0U);
    ID3D11DeviceContext_OMSetDepthStencilState(State->Context, State->DepthState, 0);
    ID3D11DeviceContext_IASetInputLayout(State->Context, State->CurrentShader.Layout);
    ID3D11DeviceContext_PSSetShader(State->Context, State->CurrentShader.PShader, 0, 0);
    ID3D11DeviceContext_VSSetShader(State->Context, State->CurrentShader.VShader, 0, 0);
    
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
    
    ID3D11DeviceContext_OMSetRenderTargets(State->Context, 1,
                                           &State->RTView, State->DSView);
  }
}

internal void
D3D11DrawSprite(d3d11_state *State,
                vs_shader_constants_2d *VSConstants,
                platform_api *Platform)
{
  D3D11_MAPPED_SUBRESOURCE Mapped;
  ID3D11DeviceContext_Map(State->Context, (ID3D11Resource *)State->VSConstantsBuffer2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
  Platform->CopyMemory(Mapped.pData, VSConstants, sizeof(vs_shader_constants_2d));
  ID3D11DeviceContext_Unmap(State->Context, (ID3D11Resource *)State->VSConstantsBuffer2D, 0);
  ID3D11DeviceContext_VSSetConstantBuffers(State->Context, 0, 1, &State->VSConstantsBuffer2D);
  ID3D11DeviceContext_PSSetShaderResources(State->Context, 0, 1, &State->CurrentSprite.TextureView);
  
  u32 Stride = sizeof(vertex);
  u32 Offset = 0;
  ID3D11DeviceContext_IASetVertexBuffers(State->Context, 0, 1, &State->DefaultQuadMesh.VBuffer, &Stride, &Offset);
  ID3D11DeviceContext_IASetIndexBuffer(State->Context, State->DefaultQuadMesh.IBuffer, DXGI_FORMAT_R32_UINT, 0);
  
  ID3D11DeviceContext_DrawIndexed(State->Context, State->DefaultQuadMesh.IndexCount, 0, 0);
}

internal void
D3D11DrawMesh(d3d11_state *State, vs_shader_constants *VSConstants, ps_shader_constants *PSConstants, platform_api *Platform)
{
  D3D11_MAPPED_SUBRESOURCE Mapped;
  ID3D11DeviceContext_Map(State->Context, (ID3D11Resource *)State->VSConstantsBuffer,
                          0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
  Platform->CopyMemory(Mapped.pData, VSConstants, sizeof(vs_shader_constants));
  ID3D11DeviceContext_Unmap(State->Context, (ID3D11Resource *)State->VSConstantsBuffer, 0);
  ID3D11DeviceContext_VSSetConstantBuffers(State->Context, 0, 1,
                                           &State->VSConstantsBuffer);
  
  ID3D11DeviceContext_Map(State->Context, (ID3D11Resource *)State->PSConstantsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
  Platform->CopyMemory(Mapped.pData, PSConstants, sizeof(ps_shader_constants));
  ID3D11DeviceContext_Unmap(State->Context, (ID3D11Resource *)State->PSConstantsBuffer, 0);
  ID3D11DeviceContext_PSSetConstantBuffers(State->Context, 1, 1, &State->PSConstantsBuffer);
  ID3D11DeviceContext_PSSetShaderResources(State->Context, 0, 1,
                                           &State->CurrentSprite.TextureView);
  
  u32 Stride = sizeof(vertex);
  u32 Offset = 0;
  ID3D11DeviceContext_IASetVertexBuffers(State->Context, 0, 1, &State->CurrentMesh.VBuffer,
                                         &Stride, &Offset);
  ID3D11DeviceContext_IASetIndexBuffer(State->Context, State->CurrentMesh.IBuffer,
                                       DXGI_FORMAT_R32_UINT, 0);
  
  ID3D11DeviceContext_DrawIndexed(State->Context, State->CurrentMesh.IndexCount, 0, 0);
}

internal void
D3D11EndFrame(d3d11_state *State, platform_api *Platform, window_dimension Dimension)
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

internal d3d11_shader
D3D11CreateShader_(ID3D11Device *Device, s8 *Name,
                   memory_arena *TempArena, platform_api *Platform)
{
  string8 NameString = CreateString(Name, TempArena, Platform);
  string8 VSString = CatStringsPlain(NameString, "_vs.fxc", TempArena, Platform);
  string8 PSString = CatStringsPlain(NameString, "_ps.fxc", TempArena, Platform);
  
  platform_file_handle VSHandle = Platform->OpenFile(VSString, false, FILE_OPEN_READ);
  if(VSHandle.Handle == 0)
  {
    Platform->LogMessagePlain("Invalid vertex file name\n", false, MESSAGE_SEVERITY_ERROR);
    return((d3d11_shader){0});
  }
  u32 VSSize = Platform->GetFileSize(VSHandle);
  void *VSData = PushSize(TempArena, VSSize);
  Platform->ReadEntireFile(VSHandle, VSSize, VSData);
  Platform->CloseFile(VSHandle);
  
  platform_file_handle PSHandle = Platform->OpenFile(PSString, false, FILE_OPEN_READ);
  if(PSHandle.Handle == 0)
  {
    Platform->LogMessagePlain("Invalid pixel file name\n", false, MESSAGE_SEVERITY_ERROR);
    return((d3d11_shader){0});
  }
  u32 PSSize = Platform->GetFileSize(PSHandle);
  void *PSData = PushSize(TempArena, PSSize);
  Platform->ReadEntireFile(PSHandle, PSSize, PSData);
  Platform->CloseFile(PSHandle);
  
  ID3D11VertexShader *VShader;
  ID3D11PixelShader *PShader;
  ID3D11Device_CreateVertexShader(Device, VSData, VSSize, 0, &VShader);
  ID3D11Device_CreatePixelShader(Device, PSData, PSSize, 0, &PShader);
  
  D3D11_INPUT_ELEMENT_DESC Desc[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex, Normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(vertex, UV),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex, Color),    D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };
  
  ID3D11InputLayout *Layout;
  ID3D11Device_CreateInputLayout(Device, Desc, ArrayCount(Desc),
                                 VSData, VSSize, &Layout);
  
  d3d11_shader Result;
  Result.Layout = Layout;
  Result.VShader = VShader;
  Result.PShader = PShader;
  return(Result);
}

internal d3d11_shader
D3D11CreateShader(d3d11_state *State, s8 *Name,
                  memory_arena *TempArena, platform_api *Platform)
{
  d3d11_shader Shader = D3D11CreateShader_(State->Device, Name, TempArena, Platform);
  return(Shader);
}

internal void
D3D11SetShader(d3d11_state *State, d3d11_shader Shader)
{
  State->CurrentShader = Shader;
  ID3D11DeviceContext_IASetInputLayout(State->Context, Shader.Layout);
  ID3D11DeviceContext_PSSetShader(State->Context, Shader.PShader, 0, 0);
  ID3D11DeviceContext_VSSetShader(State->Context, Shader.VShader, 0, 0);
}

internal d3d11_sprite
D3D11CreateSprite_(ID3D11Device *Device,
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
    ID3D11Device_CreateTexture2D(Device, &Desc, &Data, &Texture2D);
    ID3D11Device_CreateShaderResourceView(Device, (ID3D11Resource *)Texture2D,
                                          0, &Result.TextureView);
    ID3D11Texture2D_Release(Texture2D);
  }
  
  return(Result);
}

internal d3d11_sprite
D3D11CreateSprite(d3d11_state *State,
                  u32 *Texture, u32 TextureWidth, u32 TextureHeight)
{
  d3d11_sprite Sprite = D3D11CreateSprite_(State->Device, Texture, TextureWidth, TextureHeight);
  return(Sprite);
}

internal d3d11_mesh
D3D11CreateMesh_(ID3D11Device *Device,
                 vertex *VData, u32 VDataCount,
                 u32 *IData, u32 IDataLength)
{
  ID3D11Buffer *VBuffer;
  {
    D3D11_BUFFER_DESC Desc =
    {
      .ByteWidth = VDataCount*sizeof(vertex),
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_VERTEX_BUFFER
    };
    
    D3D11_SUBRESOURCE_DATA Initial = {.pSysMem = VData};
    ID3D11Device_CreateBuffer(Device, &Desc, &Initial, &VBuffer);
  }
  
  ID3D11Buffer *IBuffer;
  {
    D3D11_BUFFER_DESC Desc =
    {
      .ByteWidth = sizeof(u32)*IDataLength,
      .Usage = D3D11_USAGE_IMMUTABLE,
      .BindFlags = D3D11_BIND_INDEX_BUFFER
    };
    
    D3D11_SUBRESOURCE_DATA Initial = {.pSysMem = IData};
    ID3D11Device_CreateBuffer(Device, &Desc, &Initial, &IBuffer);
  }
  
  d3d11_mesh Mesh;
  Mesh.VBuffer = VBuffer;
  Mesh.IBuffer = IBuffer;
  Mesh.IndexCount = IDataLength;
  return(Mesh);
}

internal d3d11_mesh
D3D11CreateMesh(d3d11_state *State,
                void *VData, u32 VDataSize,
                u32 *IData, u32 IDataLength)
{
  d3d11_mesh Mesh = D3D11CreateMesh_(State->Device,
                                     VData, VDataSize,
                                     IData, IDataLength);
  return(Mesh);
}

internal void
D3D11SetRenderMode(d3d11_state *State, platform_render_mode Mode)
{
  ID3D11RasterizerState *RS = 0;
  if(Mode == PLATFORM_RENDER_SOLID)
  {
    RS = State->RSSolid;
  }
  else if(Mode == PLATFORM_RENDER_WIREFRAME)
  {
    RS = State->RSWireframe;
  }
  else if(Mode == PLATFORM_RENDER_2D)
  {
    RS = State->RS2D;
  }
  
  State->CurrentRS = RS;
  ID3D11DeviceContext_RSSetState(State->Context, RS);
}