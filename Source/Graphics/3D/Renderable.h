

#ifndef RENDERABLE_H
#define RENDERABLE_H

// EngineX Includes
#include "Graphics\3D\Transform.h"
#include "Graphics\Effects\EffectManager.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>

//----------------------------------------------------------------------------------------------------------------------
class Renderable : public Transform
{
public:

   /**
   * Determines what render queue to insert a renderable into
   **/
   enum RenderType
   {
      RENDERTYPE_OPAQUE            = 0,
      RENDERTYPE_TRANSPARENT,
      RENDERTYPE_SCREEN,
      RENDERTYPE_UI,
      NUM_RENDER_TYPES
   };


   /**
   * Constructor
   *
   * @param device - 
   * @param effectManager - 
   * @param renderType - 
   **/
   Renderable(ID3D10Device & device,
              EffectManager & effectManager,
              const RenderType renderType = RENDERTYPE_OPAQUE);

   /**
   * Deconstructor
   **/
   virtual ~Renderable();

   /**
   * Copy Constructor
   **/
   Renderable(const Renderable & rhs);

   /**
   * Assignment Operator
   *
   * @throws BaseException - if the operands have differing member references from construction time
   **/
   Renderable & operator = (const Renderable & rhs);


   /**
   * Renders the object
   **/
   virtual void Render() = 0;

   /**
   * Gets the type of renderable
   *
   * @returns const RenderType - 
   **/
   const RenderType GetRenderType();

   /**
   * Set the type of renderable
   *
   * @param renderType - 
   **/
   void SetRenderType(const RenderType renderType);


   /**
   * Gets the name of the effect this renderable will use when rendering
   *
   * @param effectName OUT - 
   * @param techniqueName OUT -
   *
   * @throws BaseException - If the effect name or technique name have not been previously set
   **/
   virtual void GetEffectName(std::string & effectName, std::string techniqueName) const;

   /**
   * Sets the name of the effect to use when rendering
   *
   * @param effectName - 
   * @param techniqueName -
   *
   * @throws BaseException - If the effect has not been previously created by the effect manager
   **/
   virtual void SetEffectName(const std::string & effectName, const std::string techniqueName);


   /**
   * Gets the material used to render
   *
   * @returns - Material & -
   *
   * @throws BaseException - A material has not been explicitly set, nor has an effect been set from which a default
   *                         material would be created
   **/
   virtual const Material & GetMaterial() const;

   /**
   * Sets the material used to render
   *
   * @param material -
   **/
   virtual void SetMaterial(const Material & material);
   

protected:

   RenderType                      m_renderType;         // Determines how to queue renderables (see Renderqueue.h)

   ID3D10Device &                  m_device;             // D3D device
   EffectManager &                 m_effectManager;      // Contains and creates effects and techniques 

   std::string                     m_effectName;         // Name of the effect to use when rendering
   std::string                     m_techniqueName;      // Name of the technique to use when rendering
   std::auto_ptr<Material>         m_material;           // Material hold all effect variable values to use when rendering



private:

};

#endif  RENDERABLE_H