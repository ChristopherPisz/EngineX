

#include "Renderable.h"

// Common Lib Includes
#include "Exception.h"

//----------------------------------------------------------------------------------------------------------------------
Renderable::Renderable(ID3D10Device & device,
                       EffectManager & effectManager,
                       const RenderType renderType)
    :
    Transform      (),
    m_device       (device),
    m_effectManager(effectManager),
    m_renderType   (renderType)
{
}

//----------------------------------------------------------------------------------------------------------------------
Renderable::~Renderable()
{
}

//----------------------------------------------------------------------------------------------------------------------
Renderable::Renderable(const Renderable & rhs)
    :
    Transform      (rhs),
    m_device       (rhs.m_device),
    m_effectManager(rhs.m_effectManager),
    m_renderType   (rhs.m_renderType)
{
}

//----------------------------------------------------------------------------------------------------------------------
Renderable & Renderable::operator = (const Renderable & rhs)
{
    if( this == &rhs )
    {
        return *this;
    }
   
    this->Transform::operator = (rhs);

    if( &m_device != &(rhs.m_device) ||
        &m_effectManager != & rhs.m_effectManager )
    {
        const std::string msg("Operands have differing m_device and m_effectManager references");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_renderType    = rhs.m_renderType;

    return *this;
}

//----------------------------------------------------------------------------------------------------------------------
const Renderable::RenderType Renderable::GetRenderType()
{
    return m_renderType;
}

//----------------------------------------------------------------------------------------------------------------------
void Renderable::SetRenderType(const RenderType renderType)
{
    m_renderType = renderType;
}

//----------------------------------------------------------------------------------------------------------------------
void Renderable::GetEffectName(std::string & effectName, std::string techniqueName) const
{
   if( m_effectName.empty() || m_techniqueName.empty() )
   {
       const std::string msg("Effect name or technique name not previously set");
       throw Common::Exception(__FILE__, __LINE__, msg);
   }

    effectName    = m_effectName;
    techniqueName = m_techniqueName;
}

//----------------------------------------------------------------------------------------------------------------------
void Renderable::SetEffectName(const std::string & effectName, const std::string techniqueName)
{
    // Check to see if the material is compatible with the current effect is skipped here
    // If they do not match, the effect will throw an exception, when attempting to set the
    // effect's state with the material, saying so.
    //
    // This allows freedom in the order of setting effect and material.

    m_effectName    = effectName;
    m_techniqueName = techniqueName;

    // If no material is currently set, get an empty copy from the effect
    if( !m_material.get() )
    {
        try
        {
            Effect & effect = m_effectManager.GetChildEffect(m_effectName);
            m_material = effect.CreateMaterial();
        }
        catch(Common::Exception & e)
        {
            throw e;
        }
    }
}

//---------------------------------------------------------------------------
const Material & Renderable::GetMaterial() const
{
   // A material is not created until
   // 1) An effect has been set, in which case the default material is created for the effect
   // 2) Or a material has been explicitly set
   if( !m_material.get() )
   {
       const std::string msg("No material currently set.");
       throw Common::Exception(__FILE__, __LINE__, msg);
   }

    return *m_material;
}

//---------------------------------------------------------------------------
void Renderable::SetMaterial(const Material & material)
{
    // Check to see if the material is compatible with the current effect is skipped here
    // If they do not match, the effect will throw an exception, when attempting to set the
    // effect's state with the material, saying so.
    //
    // This allows freedom in the order of setting effect and material.

    m_material.reset(new Material(material));
}

