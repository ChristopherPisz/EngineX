
#ifndef EFFECTMANAGER_H
#define EFFECTMANAGER_H

// EngineX Includes
#include "Graphics/Effects/Effect.h"
#include "Graphics/Effects/Material.h"
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Cameras/BaseCamera.h"
#include "Graphics/Lights/AmbientLight.h"
#include "Graphics/Lights/DirectionalLight.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Included
#include <string>
#include <map>

//---------------------------------------------------------------------------
class EffectManager
{
public:

   /**
   * Constructor
   *
   * @param device          - D3D device to use
   * @param textureManager  - Texture Manager that will contain the textures for the materials of child effects
   * @param effectDirectory - Directory that contains all effect files
   * @param effectFileName  - Filename of the .fxh file that contains the D3D effect pool
   *
   * @throws BaseException - If creation of the effect pool fails
   */
   EffectManager(ID3D10Device & device,
                 TextureManager & textureManager,
                 const std::string & effectDirectory,
                 const std::string & effectFileName);

   /**
   * Deconstructor
   */
   ~EffectManager();


   /**
   * Creates a child effect from a .fx file
   *
   * The effect will be a member of the effect pool that this object was constructed with.
   * The child effect will have all effect variables available to it, which are members of the effect pool.
   * In that fashion, variables, such as the projection matrix, will only need to be set once per frame.
   *
   * If an effect by the same name already exists in the pool, a new effect will not be created and a reference
   * to the old effect returned
   *
   * @param  - effectName     - Name of the effect for the application to refer to
   * @param  - effectFileName - Filename of the .fxo file that is the D3D effect
   * @return Effect &         - reference to the effect
   *
   * @throws BaseException - If creation of the child effect fails
   */
   virtual Effect & CreateChildEffect(const std::string & effectName, const std::string & effectFileName);

   /**
   * Get a loaded child effect
   *
   * @param effectName - Name that was given to the requested effect when it was created
   *
   * @throws BaseException - If the requested effect does not exist
   */
   virtual Effect & GetChildEffect(const std::string & effectName);


   /**
   * Gets the view matrix currently bound to this effect pool
   **/
   virtual void GetViewMatrix(D3DXMATRIX & viewMatrix);
   
   /**
   * Sets the view matrix currently bound to the effect pool
   **/
   virtual void SetViewMatrix(const D3DXMATRIX & viewMatrix);

   /**
   * Gets the projection matrix currently bound to this effect pool
   **/
   virtual void GetProjectionMatrix(D3DXMATRIX & projectionMatrix);
   
   /**
   * Sets the projection matrix currently bound to the effect pool
   **/
   virtual void SetProjectionMatrix(const D3DXMATRIX & projectionMatrix);

   /**
   * Binds the camera's view and projection matrices to the effect pool
   *
   * @param camera - Active camera for the next frame to be rendered
   *
   * @throws BaseException - If the provided camera is invalid
   */
   virtual void BindCamera(BaseCamera * camera);

   /**
   * Binds an ambient light to the effect pool
   *
   * @param ambientLight - Ambient Light to be bound
   *
   * @throws BaseException - If the provided ambient light is invalid
   */
   virtual void SetAmbientLight(AmbientLight * ambientLight);

   /**
   * Binds an ambient light to the effect pool
   *
   * @param directionalLight - Directional Light to be bound
   * @param directionalLight - Which of the 8 available directional lights to bind to
   *
   * @throws BaseException - If the provided directional light is invalid
   */
   virtual void SetDirectionalLight(DirectionalLight * directionalLight, unsigned index);

private:

   ID3D10Device &     m_device;             // D3D Device
   TextureManager &   m_textureManager;     // Texture Manager that will contain loaded textures for the materials of child effects   
   std::string        m_effectDirectory;    // Directory path where all effect files may be found
   ID3D10EffectPool * m_effectPool;         // D3D Effect Pool

   /**
   * Child effects
   *
   * Key - string containing the name assigned to the effect at creation
   * Value - pointer to the effect
   **/
   typedef std::map<std::string, Effect *> EffectMap;
   EffectMap m_effects;


   // Shared effect variables
   ID3D10EffectMatrixVariable * m_effectVariable_view;
   ID3D10EffectMatrixVariable * m_effectVariable_projection;

   /**
   *
   **/
   struct EffectVariable_AmbientLight
   {
      EffectVariable_AmbientLight()
         :
         m_effectVariable_intensity(0),
         m_effectVariable_color(0)
      {}

      ID3D10EffectScalarVariable * m_effectVariable_intensity;
      ID3D10EffectVectorVariable * m_effectVariable_color;
   };              
   
   EffectVariable_AmbientLight m_effectVariable_ambientLight;

   /**
   *
   **/
   struct EffectVariable_DirectionalLight
   {
      EffectVariable_DirectionalLight()
         :
         m_effectVariable_enabled(0),
         m_effectVariable_direction(0),
         m_effectVariable_color(0)
      {}

      ID3D10EffectScalarVariable * m_effectVariable_enabled;
      ID3D10EffectVectorVariable * m_effectVariable_direction;
      ID3D10EffectVectorVariable * m_effectVariable_color;
   };
   
   EffectVariable_DirectionalLight m_effectVariable_directionalLights[8];

};

#endif // EFFECTMANAGER_H