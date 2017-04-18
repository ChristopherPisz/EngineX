//--------------------------------------------------------------------------------------
// File: image.fx
//
// Contains techniques to render 2D images
//--------------------------------------------------------------------------------------

#include "EffectPool.fxh"

matrix world                 : World;
matrix worldInverseTranspose : WorldInverseTranspose;

//-------------------
// Diffuse Variables
//
// Diffuse Color can be a single color or sampled from a texture

float4    diffuseColor    = float4(1.0f, 1.0f, 1.0f, 1.0f);
Texture2D diffuseTexture;
bool      diffuseMapped   = false;   // true if texture is mapped to diffuse color

SamplerState samplerLinear
{
   Filter = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

//////// CONNECTOR DATA STRUCTURES ///////////

/* Data from application vertex buffer */
struct VS_INPUT
{
   float4   position  : POSITION;
   float2   texCoord  : TEXCOORD;
};

/* Data passed from vertex shader to pixel shader */
struct PS_INPUT
{
   float4 position : SV_POSITION;
   float2 texCoord : TEXCOORD;
};

//////////// STATES ///////////////

BlendState SrcAlphaBlend
{
   BlendEnable[0]           = TRUE;
   SrcBlend                 = SRC_ALPHA;
   DestBlend                = INV_SRC_ALPHA;
   BlendOp                  = ADD;
   SrcBlendAlpha            = ONE;
   DestBlendAlpha           = ONE;
   BlendOpAlpha             = ADD;
   RenderTargetWriteMask[0] = 0x0F;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Simply transforms positions
// 
PS_INPUT VS( VS_INPUT input )
{
   PS_INPUT output = (PS_INPUT)0;
   
   // Transform the incoming model-space position to projection space
   matrix worldViewProjection = mul( mul(world, view), projection);
   output.position            = mul( input.position, worldViewProjection);
   
   // Copy the texture coordinate
   output.texCoord = input.texCoord;
   
   return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Simply shades an object at 100% opacity, with no lights or shadows
//
float4 PS( PS_INPUT input ) : SV_Target
{  
   float4 colorD = diffuseColor;
   
   if( diffuseMapped )
   {
      colorD = diffuseTexture.Sample(samplerLinear, input.texCoord);
   }
   
   return colorD;
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
        SetDepthStencilState( DefaultDepthStencil, 0);
    }
}

//--------------------------------------------------------------------------------------
// Renders with alpha channel
//
technique10 RenderWithAlpha
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        
        SetBlendState( SrcAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState( DefaultDepthStencil, 0);
    }
}

