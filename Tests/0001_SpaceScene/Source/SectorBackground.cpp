

#include "SectorBackground.h"

// EngineX Includes
#include "Graphics\3D\Shapes.h"

// Common Lib Includes
#include "BaseException.h"
#include "StringUtil.h"

using namespace common_lib_cpisz;

//----------------------------------------------------------------------------------------------------------------------
SectorBackground::SectorBackground(ID3D10Device & device,
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
                                   const std::string & flare3Filename)
   :
   m_device(device),
   m_inputLayoutManager(inputLayoutManager),
   m_textureManager(textureManager),
   m_effectManager(effectManager),
   m_renderQueue(renderQueue),
   m_nebula(NULL),
   m_stars(NULL),
   m_lensFlare(NULL),
   m_ambientLight(NULL),
   m_directionalLight(NULL)
{
   //-----
   // Create graphic objects
   //

   // Create the nebula
   std::vector<Position>   positions;
   std::vector<TexCoord2D> texCoords;
   std::vector<Normal>     normals;
   std::vector<Index>      indices;

   GenerateSphere(positions, texCoords, normals, indices, 1.0f, 128, true);

   std::vector<Buffer::SharedPtr> buffers;
   
   Buffer::SharedPtr bufPositions(new Buffer(m_device, POSITION, positions));
   buffers.push_back(bufPositions);

   Buffer::SharedPtr bufTexCoords(new Buffer(m_device, TEXCOORD2D, texCoords));
   buffers.push_back(bufTexCoords);

   Buffer::SharedPtr bufNormals(new Buffer(m_device, NORMAL, normals));
   buffers.push_back(bufNormals);

   Buffer::SharedPtr bufIndices(new Buffer(m_device, INDEX, indices));
   buffers.push_back(bufIndices);

   try
   {     
      m_nebula = new PolygonSet(m_device, m_effectManager, m_inputLayoutManager);
      m_nebula->SetBuffers(buffers, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      
      Effect & effect = m_effectManager.CreateChildEffect("Background", "background.fx");
      m_nebula->SetEffectName(effect.GetName(), "RenderDefault");
      
      std::string nebulaTextureName;
      RemoveExtFromFilename(nebulaFilename, nebulaTextureName);
      m_textureManager.CreateTextureFromFile(nebulaTextureName, nebulaFilename);

      Material material = m_nebula->GetMaterial();
      material.SetBool("diffuseMapped", true);
      material.SetTextureName("diffuseTexture", nebulaTextureName);
      m_nebula->SetMaterial(material);
      m_nebula->SetRenderType(Renderable::RENDERTYPE_OPAQUE);
      
      m_nebula->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
      m_nebula->SetScale(D3DXVECTOR3(999.0f, 999.0f, 999.0f));
   }
   catch(BaseException & e)
   {
      throw e;
   }

   m_renderQueue.Insert(m_nebula);

   // Create the star sphere
   try
   {
      m_stars = new PolygonSet(*m_nebula);
      
      Effect & effect = m_effectManager.GetChildEffect("Background");
      m_stars->SetEffectName(effect.GetName(), "RenderWithAlpha");
      
      std::string starsTextureName;
      RemoveExtFromFilename(starsFilename, starsTextureName);
      m_textureManager.CreateTextureFromFile(starsTextureName, starsFilename);

      Material material = m_stars->GetMaterial();
      material.SetBool("diffuseMapped", true);
      material.SetFloat("diffuseTile", 3.0f);
      material.SetTextureName("diffuseTexture", starsTextureName);
      m_stars->SetMaterial(material);
      m_nebula->SetRenderType(Renderable::RENDERTYPE_TRANSPARENT);
      
      m_stars->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
      m_stars->SetScale(D3DXVECTOR3(998.0f, 998.0f, 998.0f));
   }
   catch(BaseException e)
   {
      throw e;
   }

   m_renderQueue.Insert(m_stars);

   // Create an ambient light
   m_ambientLight = new AmbientLight(0.0f);
   m_effectManager.SetAmbientLight(m_ambientLight);

   // Create the lens flare
   try
   {
      m_lensFlare = new LensFlare(m_device,
                                  m_inputLayoutManager,
                                  m_textureManager,
                                  m_effectManager,
                                  Position(-440.7692884f, -179.0783106f, -856.7693806f), // Magnitude of 980
                                  flareGlowFilename,
                                  flare1Filename,
                                  flare2Filename,
                                  flare3Filename);
   }
   catch(BaseException & e)
   {
      throw e;
   }

   m_renderQueue.Insert(m_lensFlare);
   
   // Create a directional light to simulate light coming from the lens flare             
   m_directionalLight = new DirectionalLight(D3DXVECTOR3( 0.44976458f, 0.18273297f, 0.87425447f),
                                             D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
                                             true);

   m_effectManager.SetDirectionalLight(m_directionalLight, 0);

   
}

//----------------------------------------------------------------------------------------------------------------------
SectorBackground::~SectorBackground()
{
   // Release the directional Light
   if( m_directionalLight )
   {
      delete m_directionalLight;
      m_directionalLight = NULL;
   }

   // Release the lens flare
   if( m_lensFlare )
   {
      m_renderQueue.Remove(m_lensFlare);

      delete m_lensFlare;
      m_lensFlare = NULL;
   }

   // Release the Ambient Light
   if( m_ambientLight )
   {
      delete m_ambientLight;
      m_ambientLight = NULL;
   }

   // Release the star graphic
   if( m_stars )
   {
      m_renderQueue.Remove(m_stars);

      delete m_stars;
      m_stars = NULL;
   }

   // Release the nebula graphic
   if( m_nebula )
   {
      m_renderQueue.Remove(m_nebula);

      delete m_nebula;
      m_nebula = NULL;
   }
}
