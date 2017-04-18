//--------------------------------------------------------------------------------------
// File: default.fx
//
//--------------------------------------------------------------------------------------

#include "EffectPool.fxh"

matrix world : World;

//-------------------
// Diffuse Variables
//
// Diffuse Color can be a single color or sampled from a texture

float4    diffuseColor;
Texture2D diffuseTexture;

SamplerState samplerLinear
{
   Filter = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

//-------------------
// Specular Variables
//
// Specular Color can be a single color or sampled from a texture

float4    specularColor;
Texture2D specularTexture;

float     specularExponent;

//------------------
// Normal Map
//
// Depending on the technique a normal map may be provided or a height map may be provided and
// normals will be calculated from the provided height data

Texture2D normalMap;              // Expects DXGI_FORMAT_R8G8B8A8_UNORM
Texture2D heightMap;              //

float2 texelSize;                 // 1.0 / TextureWidth, 1.0 / TextureHeight

//------------------
// Lighting Variables
//

float3 cameraPosition;            // Position of the camera in world space
float3 lightDir;                  // Direction vector of the light in world space (See TODO, where this is used)

//////// CONNECTOR DATA STRUCTURES ///////////

/* Data from application vertex buffers */
struct VS_INPUT
{
   float3   position  : POSITION;
   float3   tangent   : TANGENT;
   float3   biTangent : BITANGENT;
   float3   normal	 : NORMAL;
   float2   texCoord  : TEXCOORD;
};

/* Data passed from vertex shader to pixel shader */
struct VS_OUTPUT
{
   float4 position : SV_POSITION;
   float2 texCoord : TEXCOORD0;
   float3 lightDir : TEXCOORD1;
   float3 viewDir  : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Normal Vector Lookup
//--------------------------------------------------------------------------------------
float3 FetchNormalVector(float2 texCoord, uniform bool readFromTexture, uniform bool useSobelFilter)
{
   // Precomputed look-up
   if( readFromTexture )
   {
      float3 normal = normalMap.Sample(samplerLinear, texCoord).rgb;
      
      // Normal texture is stored in DXGI_FORMAT_R8G8B8_UNORM, so each component is in the range 0.0 to 1.0,
      // whereas the vector we want should be in the range -1.0 to 1.0. The multiply by 2 and subtract 1 
      // maps those vectors correctly
      return normalize( normal * 2.0f - 1.0f );
   }
   else
   {
      // Sobel Filter
      if( useSobelFilter )
      {
         float2 o00 = texCoord + float2( -texelSize.x, -texelSize.y);
         float2 o10 = texCoord + float2(          0.0f, -texelSize.y);
         float2 o20 = texCoord + float2(  texelSize.x, -texelSize.y);
         
         float2 o01 = texCoord + float2( -texelSize.x, 0.0f);
         float2 o21 = texCoord + float2(  texelSize.x, 0.0f);
         
         float2 o02 = texCoord + float2( -texelSize.x,  texelSize.y);
         float2 o12 = texCoord + float2(          0.0f,  texelSize.y);
         float2 o22 = texCoord + float2(  texelSize.x,  texelSize.y);
         
         // Get eight samples surrounding the current pixel
         float h00 = heightMap.Sample( samplerLinear, o00).r;
         float h10 = heightMap.Sample( samplerLinear, o10).r;
         float h20 = heightMap.Sample( samplerLinear, o20).r;
         
         float h01 = heightMap.Sample( samplerLinear, o01).r;
         float h21 = heightMap.Sample( samplerLinear, o21).r;
         
         float h02 = heightMap.Sample( samplerLinear, o02).r;
         float h12 = heightMap.Sample( samplerLinear, o12).r;
         float h22 = heightMap.Sample( samplerLinear, o22).r;
         
         // Evaluate the Sobel filters
         float Gx = h00 - h20 + 2.0f * h01 - 2.0f * h21 + h02 - h22;
         float Gy = h00 + 2.0f * h10 + h20 - h02 - 2.0f * h12 - h22;
         
         // Generate the missing Z
         float Gz = 0.5f * sqrt(1.0f - Gx * Gx - Gy * Gy);
         
         return normalize( float3( 2.0f * Gx, 2.0f * Gy, Gz));
      }
      
      // 3 Sample Evaluation
      else
      {
         float2 o1 = float2(texelSize.x, 0.0f);
         float2 o2 = float2(0.0f, texelSize.y);
         
         float h0 = heightMap.Sample( samplerLinear, texCoord).r;
         float h1 = heightMap.Sample( samplerLinear, texCoord + o1).r;
         float h2 = heightMap.Sample( samplerLinear, texCoord + 02).r;
         
         float3 v01 = float3( o1, h1 - h0);
         float3 v02 = float3( o2, h2 - h0);
         
         float3 n = cross(v01, v02);
         
         // Can be useful to scale the Z component to tweak the amount bumps show up.
         // Less than 1.0 will make them more apparent. Greater than 1.0 will smooth them out
         n.z *= 0.5f;
         
         return normalize(n); 
      }
   }
}


//--------------------------------------------------------------------------------------
// Lighting Models
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Blinn Phong
//
float4 Blinn_Phong(in float3 normal, in float3 viewer, in float3 light)
{
   float3 half_vector = normalize(light + viewer);
   float  HdotN       = max(0.0f, dot(half_vector, normal));
   float3 specular    = specularColor * pow(HdotN, specularExponent);
   float3 diffuse     = diffuseColor * max(0.0f, dot(normal, light));
   
   return float4(diffuse + specular, 1.0f);
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_OUTPUT VS( in VS_INPUT input )
{
   VS_OUTPUT output = (VS_OUTPUT)0;
   
   // Transform the incoming model-space position to projection space
   matrix worldViewProjection = mul( mul(world, view), projection);
   output.position            = mul( float4(input.position, 1.0f), worldViewProjection);
   
   // The world space vertex position is useful for later calculations
   float3 positionWorld = mul( float4(input.position, 1.0f), world).xyz;
   
   // The per-vertex attributes are in model space, so transform them to world space
   // before generating a world->tangent space transformation
   float3 vTangentWS   = mul( input.tangent, (float3x3)world);
   float3 vBiTangentWS = mul( input.biTangent, (float3x3) world);
   float3 vNormalWS    = mul( input.normal, (float3x3) world);
   
   // Generate a world space to tangent space transformation
   float3x3 worldToTangentSpace;
   worldToTangentSpace[0] = normalize( vTangentWS );
   worldToTangentSpace[1] = normalize( vBiTangentWS );
   worldToTangentSpace[2] = normalize( vNormalWS );
   
   // Transform inputs to the Pixel Shader into tangent space
   output.lightDir = mul( worldToTangentSpace, -lightDir);        // TODO - Not sure if light dir should remain flipped or not, depends on app
   
   output.viewDir  = cameraPosition - positionWorld;
   output.viewDir  = mul( worldToTangentSpace, output.viewDir);
   
   output.texCoord = input.texCoord;
   
   return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS( in VS_OUTPUT input) : SV_Target
{  
   float3 view  = normalize(input.viewDir);
   float3 light = normalize(input.lightDir);
   
   // Determine the offset for this pixel
   // 
   // TODO - Not sure how to incorperate this. It is for Parralex Mapping?
// float2 offset = ComputeOffset(input.texCoord, view);
   
   // Retrieve the normal
   float3 normal = FetchNormalVector(input.texCoord, true, false);
   
   // Evaluate the Lighting model
   float4 color = Blinn_Phong(normal, view, light);
   
   return color;
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

