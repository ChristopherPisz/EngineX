

// Project Includes
#include "EffectManager.h"

// Common Lib Includes
#include "Exception.h"


//----------------------------------------------------------------------------
EffectManager::EffectManager(ID3D10Device & device,
                             TextureManager & textureManager,
                             const std::string & effectDirectory,
                             const std::string & effectFileName)
    :
    m_device(device),
    m_textureManager(textureManager),
    m_effectDirectory(effectDirectory),
    m_effectVariable_view(NULL),
    m_effectVariable_projection(NULL)
{
    // Create the effect pool
    std::string effectPoolPath = effectDirectory + "\\" + effectFileName;

    if( FAILED(D3DX10CreateEffectPoolFromFile(effectPoolPath.c_str(),
                                                NULL,
                                                NULL,
                                                "fx_4_0",
                                                D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY,
                                                0,
                                                &m_device,
                                                NULL,
                                                &m_effectPool,
                                                NULL,
                                                NULL)) )
    {
        std::string msg("Could not create effect pool from file: ");
        msg += effectPoolPath;

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the shared matrix variables
    ID3D10Effect * pool = m_effectPool->AsEffect();

    m_effectVariable_view = pool->GetVariableByName("view")->AsMatrix();
    m_effectVariable_projection = pool->GetVariableByName("projection")->AsMatrix();

    if( !m_effectVariable_view->IsValid() || 
        !m_effectVariable_projection->IsValid() )
    {
        const std::string msg("Could not retreive matrix variables from the effect pool");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the shared matrix variables
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity); 

    if( FAILED(m_effectVariable_view->SetMatrix((float *)&identity)) )
    {
        const std::string msg("Could not set view matrix default value");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( FAILED(m_effectVariable_projection->SetMatrix((float *)&identity)) )
    {
        const std::string msg("Could not set projection matrix default value");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Get the ambient light variables
    ID3D10EffectVariable * ambientLight = pool->GetVariableByName("ambientLight");
   
    if( !ambientLight->IsValid() )
    {
        const std::string msg("Could not retreive ambient light structure from the effect pool");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    m_effectVariable_ambientLight.m_effectVariable_intensity = ambientLight->GetMemberByName("intensity")->AsScalar();
    m_effectVariable_ambientLight.m_effectVariable_color     = ambientLight->GetMemberByName("color")->AsVector();

    if( !m_effectVariable_ambientLight.m_effectVariable_intensity->IsValid() || 
        !m_effectVariable_ambientLight.m_effectVariable_color->IsValid() )
    {
        const std::string msg("Could not retreive ambient light variables from the effect pool");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the ambient light variables
    D3DXCOLOR color(1.0f, 1.0f, 1.0f, 1.0f );
   
    if( FAILED(m_effectVariable_ambientLight.m_effectVariable_intensity->SetFloat(1.0f)) )
    {
        const std::string msg("Could not set ambient light intensity default value");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( FAILED(m_effectVariable_ambientLight.m_effectVariable_color->SetFloatVector((float *)&color)) )
    {
        const std::string msg("Could not set ambient light color default value");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the directional light variables
    ID3D10EffectVariable * directionalLights = pool->GetVariableByName("directionalLights");

    if( !directionalLights->IsValid() )
    {
        const std::string msg("Could not retreive directional lights array from the effect pool");

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    ID3D10EffectType * type = directionalLights->GetType();
   
    D3D10_EFFECT_TYPE_DESC typeDesc;
    type->GetDesc(&typeDesc);

    for(unsigned i = 0; i < typeDesc.Elements; ++i)
    {
        ID3D10EffectVariable * directionalLight = directionalLights->GetElement(i);

        m_effectVariable_directionalLights[i].m_effectVariable_enabled   = directionalLight->GetMemberByName("enabled")->AsScalar();
        m_effectVariable_directionalLights[i].m_effectVariable_direction = directionalLight->GetMemberByName("direction")->AsVector();
        m_effectVariable_directionalLights[i].m_effectVariable_color     = directionalLight->GetMemberByName("color")->AsVector();

        if( !m_effectVariable_directionalLights[i].m_effectVariable_enabled->IsValid()   || 
            !m_effectVariable_directionalLights[i].m_effectVariable_direction->IsValid() ||
            !m_effectVariable_directionalLights[i].m_effectVariable_color->IsValid() )
        {
            const std::string msg("Could not retreive directional light variables from the effect pool");

            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        // Set the directional light variables
        if( FAILED(m_effectVariable_directionalLights[i].m_effectVariable_enabled->SetBool(false)) )
        {
            const std::string msg("Could not set directional light enabled default value");

            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }
}

//----------------------------------------------------------------------------
EffectManager::~EffectManager()
{
    // Release all the effects;
    for(EffectMap::iterator it = m_effects.begin(); it != m_effects.end(); ++it)
    {
        delete it->second;
    }

    // Release the effect pool
    if( m_effectPool )
    {
        m_effectPool->Release();
    }
}

//----------------------------------------------------------------------------
Effect & EffectManager::CreateChildEffect(const std::string & effectName, const std::string & effectFileName)
{
    // Check if an effect by that name already exists
    EffectMap::iterator it = m_effects.end();
   
    if( !m_effects.empty() )
    {
        it = m_effects.find(effectName);
    }

    if( it != m_effects.end() )
    {
        return *(it->second);
    }

    // Create a new effect
    Effect *    effect         = NULL;
    std::string effectFilePath = m_effectDirectory + "\\" + effectFileName;

    try
    {
        effect = new Effect(m_device, *m_effectPool, m_textureManager, effectName, effectFilePath);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }
   
    m_effects[effectName] = effect;

    return *effect;
}

//----------------------------------------------------------------------------
Effect & EffectManager::GetChildEffect(const std::string & effectName)
{
   // Check if an effect by that name exists
   EffectMap::const_iterator it = m_effects.end();
   
   if( !m_effects.empty() )
   {
      it = m_effects.find(effectName);
   }
   
   if( it == m_effects.end() )
   {
       std::string msg("No effect loaded by the name: ");
       msg += effectName;

       throw Common::Exception(__FILE__, __LINE__, msg);
   }

   // Return the effect
   return *(it->second);
}

//----------------------------------------------------------------------------
void EffectManager::GetViewMatrix(D3DXMATRIX & viewMatrix)
{
    m_effectVariable_view->GetMatrix((float *)&(viewMatrix));
}

//----------------------------------------------------------------------------
void EffectManager::SetViewMatrix(const D3DXMATRIX & viewMatrix)
{
    m_effectVariable_view->SetMatrix((float *)&(viewMatrix));
}

//----------------------------------------------------------------------------
void EffectManager::GetProjectionMatrix(D3DXMATRIX & projectionMatrix)
{
    m_effectVariable_projection->GetMatrix((float *)&(projectionMatrix));
}

//----------------------------------------------------------------------------
void EffectManager::SetProjectionMatrix(const D3DXMATRIX & projectionMatrix)
{
    m_effectVariable_projection->SetMatrix((float *)&(projectionMatrix));
}

//----------------------------------------------------------------------------
void EffectManager::BindCamera(BaseCamera * camera)
{
    // Check for a valid camera
    if( !camera )
    {
        const std::string msg("Camera is NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the view and projection matrices
    m_effectVariable_view->SetMatrix((float *)&(camera->GetViewMatrix()));
    m_effectVariable_projection->SetMatrix((float *)&(camera->GetProjectionMatrix()));
}

//----------------------------------------------------------------------------
void EffectManager::SetAmbientLight(AmbientLight * ambientLight)
{
    // Check for a valid light
    if( !ambientLight )
    {
        const std::string msg("Ambient Light is NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the ambient light   
    m_effectVariable_ambientLight.m_effectVariable_intensity->SetFloat(ambientLight->m_intensity);
    m_effectVariable_ambientLight.m_effectVariable_color->SetFloatVector((float *)&(ambientLight->m_color));
}

//----------------------------------------------------------------------------
void EffectManager::SetDirectionalLight(DirectionalLight * directionalLight, unsigned index)
{
    // Check for a valid light
    if( !directionalLight )
    {
        const std::string msg("Directional Light is NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check for a valid index
    if( index >= sizeof(m_effectVariable_directionalLights) / sizeof(EffectVariable_DirectionalLight) )
    {
        const std::string msg("Invalid directional light index");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the directional light   
    m_effectVariable_directionalLights[index].m_effectVariable_enabled->SetBool(directionalLight->m_enabled);
    m_effectVariable_directionalLights[index].m_effectVariable_direction->SetFloatVector((float *)&(directionalLight->m_direction));
    m_effectVariable_directionalLights[index].m_effectVariable_color->SetFloatVector((float *)&(directionalLight->m_color));
}

