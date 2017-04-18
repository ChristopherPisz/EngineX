#ifndef EFFECT_H
#define EFFECT_H

// EngineX Includes
#include "Graphics/Textures/TextureManager.h"
#include "Graphics/Effects/Technique.h"
#include "Graphics/3D/Buffers.h"
#include "Graphics/Effects/Material.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>
#include <map>
#include <vector>

class EffectManager;

//----------------------------------------------------------------------------
/**
* Interface to an HLSL effect
*
* All .fx files loaded as an effect wrapped by this interface are expected to follow the following guidelines:
*
* Must include a .fxh file that is also wrapped by the EffectManager Class
* Must contain a matrix variable named "word"
* Must contain a matrix variable named "worldInverseTranspose"
* May obtain variables from the effect pool by the names outlined in the EffectManager class header file
*/
class Effect
{
public:

   /** Only the EffectManager should construct an Effect */
   friend class EffectManager;

   /**
   * Deconstructor
   **/
   ~Effect();


   /**
   * Gets the name of this effect
   **/
   const std::string & GetName() const;

   /**
   * Gets a technique from the effect
   **/
   Technique & GetTechnique(const std::string & techniqueName);

   /**
   * Sets the world matrix effect variable
   **/
   void SetWorldMatrix(const D3DXMATRIX & worldMatrix);

   /**
   * Creates an unitialized material containing attributes that reflect all of the
   * effect variables belonging to the effect in thier default state
   *
   * @return const Material & - Copy of the created material
   **/
   std::auto_ptr<Material> CreateMaterial() const;

   /**
   * Creates a material compatible with this effect with the values of common attributes
   * from another material that was created by a different effect.
   *
   * @return const Material & - Copy of the created compatible material
   **/
   std::auto_ptr<Material> CreateMaterial(const Material & rhs) const;

   /**
   * Sets the current material
   * Sets tweakable effect parameters to those values contained within the material.
   *
   * @throws BaseException - If the supplied material is not compatible with this effect.
   *    You may create a compatible material from a material that was created by another
   *    effect by calling: CreateMaterial(const Material & rhs)
   **/
   void SetMaterial(const Material & material);



private:

   /**
   * Constructor
   *
   * Only the EffectManager should construct an Effect
   */
   Effect(ID3D10Device & device, 
          ID3D10EffectPool & effectPool,
          TextureManager & textureManager,
          const std::string & effectName, 
          const std::string & filePath);
   
   /** No copy allowed */
   Effect(const Effect & rhs);

   /** No assignment allowed */
   Effect & operator = (const Effect & rhs);


   /**
   * Updates tweakable parameters by obtaining the values from a supplied material
   */
   void UpdateMatrices(const Material & material);
   void UpdateBools(const Material & material);
   void UpdateFloats(const Material & material);
   void UpdateFloat4s(const Material & material);
   void UpdateTextures(const Material & material);


   /** Reference to the D3D graphics device */
   ID3D10Device & m_device;

   /** Reference to the texture manager from which texturesd will be obtained */
   TextureManager & m_textureManager;

   /** Each Effect creates and maintains its own copy of a DirectX effect interface */
   ID3D10Effect * m_effect;

   /** Name given to this effect at construction */
   std::string m_name;

   /**
   * Material containing all the effect variables belonging to the effect.
   *
   * This material will not have any of its attributes set and all attributes
   * will be flagged as unitialized. This material will be copied any time
   * a material is requested from the effect.
   */
   Material m_defaultEffectState;
   
   /**
   * This material reflects the current state of all effect varibales belonging to the effect.
   * It is updated as effect variables are set, to optimize in such a way that every single
   * effect variable does not have to be set if the current value and the desired value are the
   * same already. Making a call to set an effect variable is more costly then doing a 
   * comparison in the application.
   **/
   Material m_currentEffectState;

   /** Techniques */
   typedef std::map<std::string, Technique *> Techniques;
   Techniques m_techniques;



   // Non- Tweakable effect parameters
   //
   // These values are communicated directly through the public interface to this class
   
   ID3D10EffectMatrixVariable * m_worldMatrix;
   ID3D10EffectMatrixVariable * m_worldInverseTransposeMatrix;


   // Tweakable effect parameters
   //
   // These values are encapsulated by a material and communicated to the effect through the material

   typedef std::map<std::string, ID3D10EffectMatrixVariable *> EffectMatrixVariables;
   EffectMatrixVariables m_effectMatrixVariables;

   typedef std::map<std::string, ID3D10EffectScalarVariable *> EffectBoolVariables;
   EffectBoolVariables m_effectBoolVariables;

   typedef std::map<std::string, ID3D10EffectScalarVariable *> EffectFloatVariables;
   EffectFloatVariables m_effectFloatVariables;

   typedef std::map<std::string, ID3D10EffectVectorVariable *> EffectFloat4Variables;
   EffectFloat4Variables m_effectFloat4Variables;

   typedef std::map<std::string, ID3D10EffectShaderResourceVariable *> EffectTextureVariables;
   EffectTextureVariables m_effectTextureVariables;
};

#endif // EFFECT_H
