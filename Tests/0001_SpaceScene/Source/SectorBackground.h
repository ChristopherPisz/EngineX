#ifndef SECTORBACKGROUND_H
#define SECTORBACKGROUND_H

// EngineX Includes
#include "Graphics\Textures\TextureManager.h"
#include "Graphics\Effects\EffectManager.h"
#include "Graphics\3D\InputLayoutManager.h"
#include "Graphics\3D\PolygonSet.h"
#include "Graphics\3D\LensFlare.h"
#include "Graphics\Lights\AmbientLight.h"
#include "Graphics\3D\RenderQueue.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>

//----------------------------------------------------------------------------------------------------------------------
class SectorBackground
{
public:

   /**
   *
   **/
   SectorBackground(ID3D10Device & device, 
                    InputLayoutManager & inputLayoutManager, 
                    TextureManager & textureManager,
                    EffectManager & effectManager,
                    RenderQueue & renderQueue,
                    const std::string & nebulaFilename,
                    const std::string & starsFilename,
                    const std::string & flareGlowFilename,
                    const std::string & flareSourceFilename,
                    const std::string & flare1Filename,
                    const std::string & flare2Filename,
                    const std::string & flare3Filename);

   /**
   *
   **/
   ~SectorBackground();

private:

   ID3D10Device &       m_device;
   InputLayoutManager & m_inputLayoutManager;
   TextureManager &     m_textureManager;
   EffectManager &      m_effectManager;
   RenderQueue &        m_renderQueue;

   PolygonSet   *       m_nebula;
   PolygonSet   *       m_stars;
   LensFlare    *       m_lensFlare;         // Lens flare to represent stars that are more near than those in the background
   AmbientLight *       m_ambientLight;      // Ambient Light for the scene
   DirectionalLight *   m_directionalLight;  // Directional Light for the scene
};


#endif SECTORBACKGROUND_H