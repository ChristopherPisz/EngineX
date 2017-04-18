#ifndef LENSFLARE_H
#define LENSFLARE_H

// EngineX Includes
#include "Graphics/3D/Buffers.h"
#include "Graphics/3D/InputLayoutManager.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Effects/EffectManager.h"
#include "Graphics/Effects/Effect.h"
#include "Graphics/Effects/Material.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <vector>

//-----------------------------------------------------------------------
class LensFlare
{
public:

   /**
   * Constructor
   **/
   LensFlare(ID3D10Device & device, 
             InputLayoutManager & inputLayoutManager,
             TextureManager & textureManager, 
             EffectManager & effectManager,
             Position lightPosition,
             const std::string & glowTextureFilePath,
             const std::string & flare1TextureFilePath,
             const std::string & flare2TextureFilePath,
             const std::string & flare3TextureFilePath);

   /**
   * Deconstructor
   **/
   ~LensFlare();

   /**
   *
   **/
   void Render();

private:

   /**
   *
   **/
   void UpdateOcclusion(const D3DXVECTOR3 & lightPosition);

   /**
   *
   **/
   void RenderGlow(const D3DXVECTOR2 & lightPosition);

   /**
   *
   **/
   void RenderFlares(const D3DXVECTOR2 & lightPosition, const D3DXVECTOR2 & screenCenter);


   const float                    m_glowSize;
   const float                    m_querySize;
   D3DXVECTOR3                    m_lightPosition;        // Position of the light source in the 3D world
   std::string                    m_glowTextureName;

   ID3D10Device &                 m_device;               // DirectX device
   InputLayoutManager &           m_inputLayoutManager;   // Contains and creates input layouts for shaders
   TextureManager &               m_textureManager;       // Contains all loaded textures 
   EffectManager &                m_effectManager;        // Effect pool that contains all effects
   Effect *                       m_effect;               // Effect used to render the lens flare

   Buffer::SharedPtr              m_positionBuffer;       // Vertex buffer containing quad positions
   Buffer::SharedPtr              m_texCoordBuffer;       // Vertex buffer containing quad texture coordinates

   ID3D10InputLayout *            m_occlusionInputLayout; // DirectX description of the vertex buffer used for the occlusion query 
   std::auto_ptr<Material>        m_occlusionMaterial;    // Effect variable states to use when rendering the occlusion query
   Pass *                         m_occlusionPass;        // Pass used to render the occlusion query
   ID3D10Query *                  m_occlusionQuery;       // Query to the device if the light source is occluded
   bool                           m_occlusionQueryActive; // Whether the query has been sent to the device
   float                          m_occlusionAlpha;       // Dynamic alpha value calculated from result of the occlusion query

   ID3D10InputLayout *            m_glowInputLayout;      // DirectX description of the vertex buffers used for the glow
   std::auto_ptr<Material>        m_glowMaterial;         // Effect variable states to use when rendering the flares
   Pass *                         m_glowPass;             // Pass used to render the glow

   ID3D10InputLayout *            m_flareInputLayout;     // DirectX description of the vertex buffers used for the flares
   std::auto_ptr<Material>        m_flareMaterial;        // Effect variable states to use when rendering the flares
   Pass *                         m_flarePass;            // Pass used to render the flares

   struct Flare
   {
      Flare(float position, float scale, const D3DXCOLOR & color, const std::string & textureName);
     
      float       m_position;     // Zero is centered on light source. One is center of screen
      float       m_scale;
      D3DXCOLOR   m_color;
      std::string m_textureName;
   };

   std::vector<Flare> m_flares;
};

#endif // LENSFLARE_H