//--------------------------------------------------------------------------------------
// File: skybox.fx
//
//--------------------------------------------------------------------------------------

#include "EffectPool.fxh"

matrix world : World;              // Not currently used, only here to comply with application requirements

Texture2D textureDiffuse;

SamplerState samplerLinear
{
   Filter   = MIN_MAG_MIP_LINEAR;
   AddressU = Clamp;
   AddressV = Clamp;
};

//////// CONNECTOR DATA STRUCTURES ///////////

/* Data from application vertex buffer */
struct VS_INPUT
{
   float4   position  : POSITION;
   float2   uv        : TEXCOORD;
};

/* Data passed from vertex shader to pixel shader */
struct VS_OUTPUT
{
   float4 position : SV_POSITION;
   float2 uv       : TEXCOORD;
};

//////////// STATES ///////////////

DepthStencilState SkyBoxDepthStencil
{
   DepthEnable               = TRUE;
   DepthWriteMask            = Zero;
   DepthFunc                 = Less_Equal;
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

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( VS_INPUT input )
{
   VS_OUTPUT output = (VS_OUTPUT)0;
   
   // Transform the position
   matrix viewProjection = mul(view, projection);
   output.position = mul(input.position.xyz, viewProjection).xyww;
   
   // Copy the UV
   output.uv = input.uv;
   
   return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Simply shades an object at 100% opacity, with no lights or shadows
//
float4 PS( VS_OUTPUT input ) : SV_Target
{  
   // Sample the color
   float4 finalColor = textureDiffuse.Sample(samplerLinear, input.uv);
   finalColor.a = 1;
   
   return finalColor;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Renders an object at 100% opacity, with no lights or shadows
//
technique10 RenderDefault
{
   pass P0
   {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS() ) );
   
      SetBlendState( DefaultBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
      SetDepthStencilState( SkyBoxDepthStencil, 0);
   }
}

