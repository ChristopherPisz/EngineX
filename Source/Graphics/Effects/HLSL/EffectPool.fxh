
//--------------------------------------------------------------------------------------
// File: EffectPool.fx
//
// Contains shared effect variables and functions that remain the same every frame
// for all effects.
//--------------------------------------------------------------------------------------

shared cbuffer Matrices
{
   matrix view          : View;
   matrix projection    : Projection;
};

shared cbuffer Lights
{
   struct AmbientLight
   {
      float  intensity;
      float4 color;
   };
   
   struct DirectionalLight
   {
      bool   enabled;
      float3 direction;
      float4 color;
   };
   
   AmbientLight     ambientLight;
   DirectionalLight directionalLights[8];
};

//////////// STATES ///////////////

shared BlendState DefaultBlending
{
   AlphaToCoverageEnable    = FALSE;
   BlendEnable[0]           = FALSE;
   BlendEnable[1]           = FALSE;
   BlendEnable[2]           = FALSE;
   BlendEnable[3]           = FALSE;
   BlendEnable[4]           = FALSE;
   BlendEnable[5]           = FALSE;
   BlendEnable[6]           = FALSE;
   BlendEnable[7]           = FALSE;
   SrcBlend                 = One;
   DestBlend                = One;
   BlendOp                  = Add;
   SrcBlendAlpha            = One;
   DestBlendAlpha           = Zero;
   BlendOpAlpha             = Add;
   RenderTargetWriteMask[0] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[1] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[2] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[3] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[4] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[5] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[6] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
   RenderTargetWriteMask[7] = 0x0F;   // D3D10_COLOR_WRITE_ENABLE_ALL
};

shared DepthStencilState DefaultDepthStencil
{
   DepthEnable               = TRUE;
   DepthWriteMask            = All;
   DepthFunc                 = Less;
   StencilEnable             = FALSE;
   StencilReadMask           = 0xFF;  // DEFAULT_STENCIL_READ_MASK
   StencilWriteMask          = 0xFF;  // DEFAULT_STENCIL_WRITE_MASK
   FrontFaceStencilFail      = Keep;
   FrontFaceStencilDepthFail = Keep;
   FrontFaceStencilPass      = Keep;
   FrontFaceStencilFunc      = Always;
   BackFaceStencilFail       = Keep;
   BackFaceStencilDepthFail  = Keep;
   BackFaceStencilPass       = Keep;
   BackFaceStencilFunc       = Always;
};
