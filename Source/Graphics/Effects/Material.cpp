

// Project Includes
#include "Material.h"

// Common Library Includes
#include "Exception.h"


//----------------------------------------------------------------------------
Material::Material(const std::string & effectName)
    :
    m_effectName(effectName)
{
}

//----------------------------------------------------------------------------
Material::Material(const Material & rhs)
    :
    m_effectName(rhs.m_effectName),
    m_matrices(rhs.m_matrices),
    m_bools(rhs.m_bools),
    m_floats(rhs.m_floats),
    m_float4s(rhs.m_float4s),
    m_textureNames(rhs.m_textureNames)
{
}

//----------------------------------------------------------------------------
Material::~Material()
{
}

//----------------------------------------------------------------------------
Material & Material::operator = (const Material & rhs)
{
    m_effectName    = rhs.m_effectName;

    m_matrices      = rhs.m_matrices;
    m_bools         = rhs.m_bools;
    m_floats        = rhs.m_floats;
    m_float4s       = rhs.m_float4s;
    m_textureNames  = rhs.m_textureNames;

    return *this;
}

//----------------------------------------------------------------------------
const std::string & Material::GetEffectName() const
{
    return m_effectName;
}

//----------------------------------------------------------------------------
void Material::CreateMatrix(const std::string & variableName, const D3DXMATRIX & defaultValue)
{
    Attribute<D3DXMATRIX> attribute;
    attribute.m_initialized = false;
    attribute.m_value       = defaultValue;

    m_matrices[variableName] = attribute;
}

//----------------------------------------------------------------------------
const D3DXMATRIX & Material::GetMatrix(const std::string & variableName) const
{
    // Check if the matrix exists
    Matrices::const_iterator it = m_matrices.end();

    if( !m_matrices.empty() )
    {
        it = m_matrices.find(variableName);
    }

    if( it == m_matrices.end() )
    {
        std::string msg("Could not find matrix type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Check if the value has been initialized
    if( !it->second.m_initialized )
    {
        std::string msg("Matrix type material attribute: ");
        msg += variableName + " has not yet been initialized";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Return the matrix
    return it->second.m_value;
}

//----------------------------------------------------------------------------
void Material::SetMatrix(const std::string & variableName, const D3DXMATRIX & value)
{
    // Check if the matrix exists
    Matrices::const_iterator it = m_matrices.end();

    if( !m_matrices.empty() )
    {
        it = m_matrices.find(variableName);
    }
   
    if( it == m_matrices.end() )
    {
        std::string msg("Could not find matrix type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the attribute
    Attribute<D3DXMATRIX> attribute;
    attribute.m_initialized = true;
    attribute.m_value       = value;

    m_matrices[variableName] = attribute;
}

//----------------------------------------------------------------------------
void Material::CreateBool(const std::string & variableName, bool defaultValue)
{
    Attribute<bool> attribute;
    attribute.m_initialized = false;
    attribute.m_value       = defaultValue;

    m_bools[variableName] = attribute;
}

//----------------------------------------------------------------------------
const bool Material::GetBool(const std::string & variableName) const
{
    // Check if the bool exists
    Bools::const_iterator it = m_bools.end();

    if( !m_bools.empty() )
    {
        it = m_bools.find(variableName);
    }

    if( it == m_bools.end() )
    {
        std::string msg("Could not find bool type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Check if the value has been initialized
    if( !it->second.m_initialized )
    {
        std::string msg("Bool type material attribute: ");
        msg += variableName + " has not yet been initialized";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Return the bool
    return it->second.m_value;
}

//----------------------------------------------------------------------------
void Material::SetBool(const std::string & variableName, const bool value)
{
    // Check if the bool exists
    Bools::const_iterator it = m_bools.end();

    if( !m_bools.empty() )
    {
        it = m_bools.find(variableName);
    }

    if( it == m_bools.end() )
    {
        std::string msg("Could not find bool type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the attribute
    Attribute<bool> attribute;
    attribute.m_initialized = true;
    attribute.m_value       = value;

    m_bools[variableName] = attribute;
}

//----------------------------------------------------------------------------
void Material::CreateFloat(const std::string & variableName, float defaultValue)
{
    Attribute<float> attribute;
    attribute.m_initialized = false;
    attribute.m_value       = defaultValue;

    m_floats[variableName] = attribute;
}

//----------------------------------------------------------------------------
const float Material::GetFloat(const std::string & variableName) const
{
    // Check if the float exists
    Floats::const_iterator it = m_floats.end();

    if( !m_floats.empty() )
    {
        it = m_floats.find(variableName);
    }

    if( it == m_floats.end() )
    {
        std::string msg("Could not find float type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Check if the value has been initialized
    if( !it->second.m_initialized )
    {
        std::string msg("Float type material attribute: ");
        msg += variableName + " has not yet been initialized";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Return the float
    return it->second.m_value;
}

//----------------------------------------------------------------------------
void Material::SetFloat(const std::string & variableName, const float value)
{
    // Check if the float exists
    Floats::const_iterator it = m_floats.end();

    if( !m_floats.empty() )
    {
        it = m_floats.find(variableName);
    }

    if( it == m_floats.end() )
    {
        std::string msg("Could not find float type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the attribute
    Attribute<float> attribute;
    attribute.m_initialized = true;
    attribute.m_value       = value;

    m_floats[variableName] = attribute;
}

//----------------------------------------------------------------------------
void Material::CreateFloat4(const std::string & variableName, const D3DXVECTOR4 & defaultValue)
{
    Attribute<D3DXVECTOR4> attribute;
    attribute.m_initialized = false;
    attribute.m_value       = defaultValue;

    m_float4s[variableName] = attribute;
}

//----------------------------------------------------------------------------
const D3DXVECTOR4 Material::GetFloat4(const std::string & variableName) const
{
    // Check if the float4 exists
    Float4s::const_iterator it = m_float4s.end();

    if( !m_float4s.empty() )
    {
        it = m_float4s.find(variableName);
    }

    if( it == m_float4s.end() )
    {
        std::string msg("Could not find float4 type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Check if the value has been initialized
    if( !it->second.m_initialized )
    {
        std::string msg("Float4 type material attribute: ");
        msg += variableName + " has not yet been initialized";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Return the float4
    return it->second.m_value;
}

//----------------------------------------------------------------------------
void Material::SetFloat4(const std::string & variableName, const D3DXVECTOR4 & value)
{
    // Check if the float4 exists
    Float4s::const_iterator it = m_float4s.end();

    if( !m_float4s.empty() )
    {
        it = m_float4s.find(variableName);
    }

    if( it == m_float4s.end() )
    {
        std::string msg("Could not find float4 type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the attribute
    Attribute<D3DXVECTOR4> attribute;
    attribute.m_initialized = true;
    attribute.m_value       = value;

    m_float4s[variableName] = attribute;
}

//----------------------------------------------------------------------------
void Material::CreateTextureName(const std::string & variableName)
{
    Attribute<std::string> attribute;
    attribute.m_initialized = false;
    attribute.m_value       = std::string();

    m_textureNames[variableName] = attribute;
}

//----------------------------------------------------------------------------
void Material::SetTextureName(const std::string & variableName, const std::string & textureName)
{
    // Check if the texture name exists
    TextureNames::const_iterator it = m_textureNames.end();
   
    if( !m_textureNames.empty() )
    {
        it = m_textureNames.find(variableName);
    }
   
    if( it == m_textureNames.end() )
    {
        std::string msg("Could not find texture name type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the attribute
    Attribute<std::string> attribute;
    attribute.m_initialized = true;
    attribute.m_value       = textureName;

    m_textureNames[variableName] = attribute;
}

//----------------------------------------------------------------------------
const std::string Material::GetTextureName(const std::string & variableName) const
{
    // Check if the texture name exists
    TextureNames::const_iterator it = m_textureNames.end();

    if( !m_textureNames.empty() )
    {
        it = m_textureNames.find(variableName);
    }

    if( it == m_textureNames.end() )
    {
        std::string msg("Could not find texture name type material attribute: ");
        msg += variableName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Check if the value has been initialized
    if( !it->second.m_initialized )
    {
        std::string msg("Texture name type material attribute: ");
        msg += variableName + " has not yet been initialized";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Return the texture name
    return it->second.m_value;
}

