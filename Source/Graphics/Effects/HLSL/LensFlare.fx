//--------------------------------------------------------------------------------------
// File: LensFlare.fx
//
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

SamplerState samplerLinear
{
   Filter = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

//////// CONNECTOR DATA STRUCTURES ///////////

struct VS_IN_OCCLUSION
{
   float4 position : POSITION;
};

struct PS_IN_OCCLUSION
{
   float4 position : SV_POSITION;
};

struct VS_IN_TINT
{
   float4 position : POSITION;
   float2 texCoord : TEXCOORD;
};

struct PS_IN_TINT
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

BlendState SrcAlphaAdd
{
   BlendEnable[0]           = TRUE;
   SrcBlend                 = SRC_ALPHA;
   DestBlend                = ONE;
   BlendOp                  = ADD;
   SrcBlendAlpha            = ONE;
   DestBlendAlpha           = ONE;
   BlendOpAlpha             = ADD;
   RenderTargetWriteMask[0] = 0x0F;
};

BlendState OcclusionAlphaBlend
{
   BlendEnable[0]           = FALSE;
   RenderTargetWriteMask[0] = 0x00;      // Write no colors
};

DepthStencilState DepthStencilNoWrite
{
   DepthEnable              = TRUE;
   DepthWriteMask           = ZERO;
   DepthFunc                = Less;
   StencilEnable            = FALSE;
};

//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Transforms positions
// 
PS_IN_OCCLUSION VS_Occlusion( VS_IN_OCCLUSION input )
{
   PS_IN_OCCLUSION output = (PS_IN_OCCLUSION)0;
   
   // Transform the incoming model-space position to projection space
   matrix worldViewProjection = mul( mul(world, view), projection);
   output.position            = mul( input.position, worldViewProjection);

   return output;
}

//--------------------------------------------------------------------------------------
// Transforms positions and copies texture coordinates
//
PS_IN_TINT VS_Tint( VS_IN_TINT input )
{
   PS_IN_TINT output = (PS_IN_TINT)0;
   
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
// Colors the object with the diffuse color
//
float4 PS_Occlusion( PS_IN_OCCLUSION input ) : SV_Target
{  
   return diffuseColor;
}

//--------------------------------------------------------------------------------------
// Colors the object with the diffuse color multiplied by the texture color
//
float4 PS_Tint( PS_IN_TINT input ) : SV_Target
{  
   return diffuseColor * diffuseTexture.Sample(samplerLinear, input.texCoord);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
//
technique10 OcclusionQuery
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Occlusion() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Occlusion() ) );
        
        SetBlendState( OcclusionAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState( DepthStencilNoWrite, 0);
    }
}

//--------------------------------------------------------------------------------------
//
//
technique10 RenderGlow
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Tint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Tint() ) );
        
        SetBlendState( SrcAlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState( DepthStencilNoWrite, 0);
    }
}

//--------------------------------------------------------------------------------------
// Renders an object at 100% opacity, with no lights or shadows
//
technique10 RenderFlare
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Tint() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Tint() ) );
        
        SetBlendState( SrcAlphaAdd, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetDepthStencilState( DepthStencilNoWrite, 0);
    }
}
