
//------------------------------------------
// General Lighting Equation
//
// The lighting equation (commonly used in real-time graphics) is combined from three 
// parts - ambient, diffuse and specular in this fashion:
//
// I_total = I_ambient + I_diffuse + I_specular
//
// I_total    is the final color value
// I_ambient  is the ambient color
// I_diffuse  is the diffuse color
// I_specular is the specular color for a given pixel
//
//--------------------------------------------
// Ambient component
//
// The ambient component simulates indirect light contribution (for example a light that
// bounced off a wall and then reached the object) and emission of a light by the surface itself.
// This type of light interaction is not computed in diffuse or specular part. This component
// ensures that every pixel receives at least some minimum amount of color and will not remain
// black during the rendering.
//
// I_ambient = (M_ambient * L_ambient) + M_emissive
//
// M_ambient is the ambient part of material properties (color or a texture value at current pixel)
// L_ambient is the ambient light color of a scene
// M_emissive specifies color of light that surface emits
//
// It can be clearly seen that groups of pixels lit only by ambient lighting component will
// likely loose the effect of 3D (due to absence of shades), because the light contribution for this
// part is constant. A cure for this is to ensure that every place in scene receives at least
// minimum light amount from a light source (i.e. we can use some type of minor headlight
// light). Another method is to pre-compute offline the ambient (and static diffuse) parts of scene
// lighting using for example the radiosity method and then use results as textures.
//
//------------------------------------------
// Diffuse component
//
// The diffuse component is based on a law of physics called Lambert's law and has the
// biggest base in the real world (and the interaction of photons and surfaces). Lambert's law
// says that the reflected light intensity is determined by the cosine of the angle a between the
// surface normal n and the light vector L (heading from a surface point to the light) for totally
// matte (diffuse) surfaces. Both vectors are normalized. This lighting component is independent
// of viewer’s position, because of the fact that for a perfectly diffuse surface the probability of a
// new reflection direction is equal for every direction.
// 
// The diffuse light intensity (I_diffuse) computation has the form:
// I_diffuse = cos(a) = dot(n, L)
// 
// We extend the equation to take into account
// 1) The surface color M_diffuse at the current pixel,
// 2) The light color - L_diffuse 
// 3) That the angle between the light and the normal has to be from 0 to PI/2
//    because for greater angles, the normal is faced away from the light and is not being lit.
//
// I_diffuse = max( dot(n, L), 0) * M_diffuse * L_diffuse
//
//------------------------------------------
// Specular component
//
// We use the specular part of lighting equation to simulate shiny surfaces with the
// highlights. A very popular model is called the Phong reflection model and it computes
// light intensity as the cosine of the angle between the normalized light vector, reflected around
// the point normal, and the normalized vector from the point to the position of the viewer,
// powered by the shininess value that specifies an amount of reflection. This equation has little to
// do with the real world illumination but it looks good under most conditions and is easy to
// compute.
//
// Phong's specular equation then is:
// r = 2 * (dot(n, L) * n - L
// I_specular = cos(a)^shininess = (dot(r, v)^shininess
//
// r is the reflection vector
// n is the face normal
// L is the normalized light vector
// v is the vector from the point to the viewer
// shininess is an amount of reflection
//
// If a dot product of n and v is less than zero (angle is greater than PI/2), then a pixel is
// facing away from the light and lighting should not be computed.
// 
// We extend the equation to take into account
// 1) The material's specular intensity modifier M_specular (in real-time graphics commonly called the gloss map)
// 2) The light's color L_specular
// 
// r = 2 * (dot(n, L) * n - L
// I_specular = (dot(r, v)^shininess * M_specular * L_specular

#include "EffectPool.fxh"

matrix world                 : World;
matrix worldInverseTranspose : WorldInverseTranspose;

//-------------------
// Ambient Variables
//
// Ambient color is the same as diffuse color for this shader. It is rare they would beed to be different.
// Emmisive color can be a single color or sampled from a texture

float4    ambientColor      = float4(1.0f, 1.0f, 1.0f, 1.0f);
Texture2D ambientTexture;
bool      ambientMapped     = false;   // true if texture is mapped to diffuse color

float4    emissiveColor     = float4(0.0f, 0.0f, 0.0f, 1.0f);    
Texture2D emissiveTexture;
bool      emissiveMapped    = false;   // true if texture is mapped to emmisive color
float     emissiveIntensity = 1.0f;

//-------------------
// Diffuse Variables
//
// Diffuse Color can be a single color or sampled from a texture

float4    diffuseColor    = float4(1.0f, 1.0f, 1.0f, 1.0f);
Texture2D diffuseTexture;
bool      diffuseMapped   = false;   // true if texture is mapped to diffuse color

//-------------------
// Specular Variables
//
// Specular Color can be a single color or sampled from a texture

float4    specularColor    = float4(1.0f, 1.0f, 1.0f, 1.0f);
Texture2D specularTexture;
bool      specularMapped   = false;   // true if texture is mapped to specular color

float     specularExponent = 20.0f;

//-------------------
// Texture Samplers

SamplerState smplLinear
{
   Filter   = MIN_MAG_MIP_LINEAR;
   AddressU = Wrap;
   AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
   float4 position  : POSITION;
   float2 texCoord  : TEXCOORD;
   float3 normal    : NORMAL;
};

struct PS_INPUT
{
   float4   position  : SV_POSITION;
   float2   texCoord  : TEXCOORD0;
   float3   normal    : NORMAL;
   float3   positionW : PositionWorld;
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
// Lighting Models
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Calculates Ambient Color Component
//
float3 Ambient(float2 texCoord : TEXCOORD) : COLOR 
{
   float3 colorA        = diffuseColor.rgb;
   float3 colorE        = emissiveColor.rgb;

   if(ambientMapped)
   {
       colorA = ambientTexture.Sample(smplLinear, texCoord).rgb;
   }
     
   if(emissiveMapped)
   {
       colorE = mul(emissiveIntensity, emissiveTexture.Sample(smplLinear, texCoord).rgb);
   }

   return colorA * (ambientLight.intensity * ambientLight.color.rgb) + colorE;
}

//--------------------------------------------------------------------------------------
// Blinn Phong
//
float3 Blinn_Phong(in float3 normal, in float3 viewer, in float3 light, in float2 texCoord)
{
   float3 colorD = diffuseColor.rgb;
   float3 colorS = specularColor.rgb;
   
   if( diffuseMapped )
   {
       colorD = diffuseTexture.Sample(smplLinear, texCoord).rgb;
   }
   
   if( specularMapped )
   {
       colorS = specularTexture.Sample(smplLinear, texCoord).rgb;
   }
   
   float3 half_vector = normalize(light + viewer);
   float3 HdotN       = max(0.0f, dot(half_vector, normal));
   colorS             = colorS * pow(HdotN, specularExponent);
   colorD             = colorD * max(0.0f, dot(normal, light));
   
   return colorD + colorS;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
   PS_INPUT output = (PS_INPUT)0;
   
   // Transform the incoming model-space position to projection space
   matrix worldViewProjection = mul( mul(world, view), projection);
   output.position            = mul( input.position, worldViewProjection);
   
   // Copy the texture coordinate
   output.texCoord = input.texCoord;
   
   // Transform the incoming normal to world space without scaling
   output.normal = mul(float4(input.normal, 0.0f), worldInverseTranspose).xyz;

   // Pass the position in world space
   output.positionW = mul(input.position, world).xyz;
   
   return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input ) : SV_Target
{
   float3 cameraPosition = mul( -1, float3(view._41, view._42, view._43));
   float3 viewDir = normalize(cameraPosition - input.positionW);
   
   // Calculate Ambient contribution
   float3 colorA = Ambient(input.texCoord);
   
   // Calculate directional light contribution
   float3 colorDS = {0,0,0};
      
   for(uint i = 0; i < 8; ++i)
   {
      float3 lightDir = normalize(-directionalLights[i].direction.xyz);
        
      // Add this light's contribution
      colorDS += Blinn_Phong(input.normal, 
                             viewDir, 
                             lightDir, 
                             input.texCoord);
   }
   
   return float4(colorA.rgb + colorDS.rgb, 1.0);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Renders
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
