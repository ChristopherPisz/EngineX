#ifndef SKYBOX_H
#define SKYBOX_H

// EngineX Includes
#include "Graphics/3D/Buffers.h"
#include "Graphics/3D/InputLayoutManager.h"
#include "Graphics/3D/InputElementDescription.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Effects/EffectManager.h"
#include "Graphics/Effects/Pass.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>

//-----------------------------------------------------------------------
class SkyBox
{
public:
   
   /**
   * Constructor
   * 
   * This constructor uses one texture and expects all 6 faces lines up horizontally in the order posX, negX, posY, negY, posZ, negZ.
   * The texture should therefor be, 6 times as wide as it is high.
   *
   * Will load textures into the texture manager with names that match the filenames in the paths given, without the extension.
   *
   * @throws BaseException - If construction failed
   **/
   SkyBox(ID3D10Device & device,
          TextureManager & textureManager, 
          EffectManager & effectManager, 
          InputLayoutManager & inputLayoutManager,
          const std::string & textureName_cubeFaces);

   /**
   * Constructor
   *
   * This constructor uses six seperate textures, one for each face.
   *
   * Will load textures into the texture manager with names that match the filenames in the paths given, without the extension.
   *
   * @throws BaseException - If construction failed
   **/
   SkyBox(ID3D10Device & device, 
          TextureManager & textureManager, 
          EffectManager & effectManager, 
          InputLayoutManager & inputLayoutManager,
          const std::string & textureFileName_posX,
          const std::string & textureFileName_negX,
          const std::string & textureFileName_posY,
          const std::string & textureFileName_negY,
          const std::string & textureFileName_posZ,
          const std::string & textureFileName_negZ);
   
   /**
   * Deconstructor
   **/
   ~SkyBox();

   /**
   * Renders the SkyBox
   **/
   void Render();


private:

   ID3D10Device &                 m_device;                  // D3D Device
   EffectManager &                m_effectManager;           // Contains and manages the effect pool
   
   std::vector<Buffer::SharedPtr> m_buffers;                 // Vertex buffers containing the geometry data to passed to the shader 
   ID3D10InputLayout *            m_inputLayout;             // Input layout of the buffered data being passed to the shader
   std::vector<unsigned>          m_offsets;                 // Number of bytes between the first element of the corresponding (by index into the vector) vertex buffer and the first element to be used
   std::vector<unsigned>          m_strides;                 // Size in bytes of an element in the corresponding (by index into the vector) vertex buffer
   
   std::string                    m_effectName;              // Effect to render with
   std::string                    m_techniqueName;           // Technique to render with
   std::auto_ptr<Material>        m_material;                // Material containing the effect state to render with

   std::string                    m_cubeFacesTextureName;    // Name of a single texture that contains all sides
   std::string                    m_perSideTextureNames[6];  // Names of the textures to use for each side
};

#endif // SKYBOX_H
