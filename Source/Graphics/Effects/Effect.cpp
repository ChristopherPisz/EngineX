

// Project Includes
#include "Effect.h"

// Common Lib Includes
#include "Exception.h"

// Direct X Includes
#include <DXErr.h>

// Standard Includes
#include <sstream>

//----------------------------------------------------------------------------
Effect::Effect(ID3D10Device & device, 
               ID3D10EffectPool & effectPool,
               TextureManager & textureManager,
               const std::string & effectName, 
               const std::string & filePath)
    :
    m_device                     (device),
    m_textureManager             (textureManager),
    m_effect                     (nullptr),
    m_name                       (effectName),
    m_defaultEffectState         (effectName),
    m_currentEffectState         (effectName),
    m_worldMatrix                (nullptr),
    m_worldInverseTransposeMatrix(nullptr)
{
    //-----
    // Compile the effect

    DWORD effectFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
    
   // Set the D3D10_SHADER_DEBUG flag to embed debug information in the Effects.
   // Setting this flag improves the Effect debugging experience, but still allows 
   // the Effects to be optimized and to run exactly the way they will run in 
   // the release configuration of this program.
   effectFlags |= D3D10_SHADER_DEBUG;

#endif

    ID3D10Blob * compilationErrors = NULL;

    HRESULT hr = D3DX10CreateEffectFromFile(filePath.c_str(),
                                            NULL,
                                            NULL,
                                            "fx_4_0",
                                            effectFlags,
                                            D3D10_EFFECT_COMPILE_CHILD_EFFECT,
                                            &m_device,
                                            &effectPool,
                                            NULL,
                                            &m_effect,
                                            &compilationErrors,
                                            NULL);
    if( FAILED(hr) || !m_effect )
    {
        std::stringstream msg;
        msg << "Could not compile effect file: " << filePath << " ";
      
        if( compilationErrors )
        {
            msg << "\n\nCompilation Error Reported: " << reinterpret_cast<char *>(compilationErrors->GetBufferPointer());
            compilationErrors->Release();
        }

        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    //-----
    // Create the effect variables and this effect's blank material
   
    // Query the effect for information
    D3D10_EFFECT_DESC effectDesc;
    m_effect->GetDesc(&effectDesc);

    UINT numGlobals = effectDesc.GlobalVariables;
   
    // Query the effect for variables
    for(unsigned i = 0; i < numGlobals; ++i)
    {
        ID3D10EffectVariable * effectVariable = m_effect->GetVariableByIndex(i);
      
        // Get the effect variable name
        D3D10_EFFECT_VARIABLE_DESC effectVariableDesc;
        effectVariable->GetDesc(&effectVariableDesc);

        std::string name = effectVariableDesc.Name;

        // Get the effect variable type
        ID3D10EffectType * type = effectVariable->GetType();
      
        D3D10_EFFECT_TYPE_DESC effectTypeDesc;
        type->GetDesc(&effectTypeDesc);

        std::string typeName                      = effectTypeDesc.TypeName;
        D3D10_SHADER_VARIABLE_CLASS variableClass = effectTypeDesc.Class;
        D3D10_SHADER_VARIABLE_TYPE  variableType  = effectTypeDesc.Type;

        // Create the variable as a member of this effect and this effect's material
        //
      
        // Matrix
        if( variableClass == D3D10_SVC_MATRIX_COLUMNS )
        {
            // Handle non-tweakable matrix parameters
            if( name == "world" )
            {
            m_worldMatrix = effectVariable->AsMatrix();
            }
            else if( name == "worldInverseTranspose" )
            {
            m_worldInverseTransposeMatrix = effectVariable->AsMatrix();
            }

            // Handle tweakable matrix parameters
            else
            {
            m_effectMatrixVariables[name] = effectVariable->AsMatrix();
            
            D3DXMATRIX defaultValue;
            m_effectMatrixVariables[name]->GetMatrix((float *) &defaultValue);
            m_defaultEffectState.CreateMatrix(name, defaultValue);
            
            }
        }

        // Bool
        else if( variableClass == D3D10_SVC_SCALAR && variableType == D3D10_SVT_BOOL )
        {
            m_effectBoolVariables[name] = effectVariable->AsScalar();

            BOOL defaultValue;
            m_effectBoolVariables[name]->GetBool(&defaultValue);
            m_defaultEffectState.CreateBool(name, (defaultValue != 0) );
        }

        // Float
        else if( variableClass == D3D10_SVC_SCALAR && variableType == D3D10_SVT_FLOAT )
        {
            m_effectFloatVariables[name] = effectVariable->AsScalar();

            float defaultValue;
            m_effectFloatVariables[name]->GetFloat(&defaultValue);
            m_defaultEffectState.CreateFloat(name, defaultValue);
        }

        // Float4
        else if( variableClass == D3D10_SVC_VECTOR && variableType == D3D10_SVT_FLOAT )
        {
            m_effectFloat4Variables[name] = effectVariable->AsVector();

            D3DXVECTOR4 defaultValue;
            m_effectFloat4Variables[name]->GetFloatVector((float *)&defaultValue);
            m_defaultEffectState.CreateFloat4(name, defaultValue);
        }

        // Texture2D
        else if( variableClass == D3D10_SVC_OBJECT && variableType == D3D10_SVT_TEXTURE2D )
        {
            m_effectTextureVariables[name] = effectVariable->AsShaderResource();

            m_defaultEffectState.CreateTextureName(name);
        }

        // Sampler
        else if( variableClass == D3D10_SVC_OBJECT && variableType == D3D10_SVT_SAMPLER )
        {
            // Samplers are internal to the effect
            continue;
        }

        // States
        else if( variableClass == D3D10_SVC_OBJECT      && 
                (variableType == D3D10_SVT_DEPTHSTENCIL || 
                 variableType == D3D10_SVT_BLEND        ||
                 variableType == D3D10_SVT_RASTERIZER)  )
        {
            // states are internal to the effect
            continue;
        }

        else
        {
            std::string msg;
            msg  = "Could not get effect variables for effect: " + filePath;
            msg += "\n Effect contains unimplimented effect variable type: " + typeName;

            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }

    //-----
    // Get techniques
    unsigned numTechniques = effectDesc.Techniques;
   
    for(unsigned i = 0; i < numTechniques; ++i)
    {
        ID3D10EffectTechnique * d3dTechnique = m_effect->GetTechniqueByIndex(i);
      
        Technique * technique = new Technique(d3dTechnique);
        m_techniques[technique->GetName()] = technique;
    }

    // Initialize the current state of the effect variables
    m_currentEffectState = m_defaultEffectState;
}

//----------------------------------------------------------------------------
Effect::~Effect()
{
   for( Techniques::iterator it = m_techniques.begin(); it != m_techniques.end(); ++it)
   {
      delete it->second;
   }

   if( m_effect )
   {
      m_effect->Release();
   }
}

//----------------------------------------------------------------------------
const std::string & Effect::GetName() const
{
   return m_name;
}

//----------------------------------------------------------------------------
Technique & Effect::GetTechnique(const std::string & techniqueName)
{
    Techniques::iterator it = m_techniques.find(techniqueName);

    if( it == m_techniques.end() )
    {
        std::string msg("Effect does not contain technique: ");
        msg += techniqueName;

        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    return *(it->second);
}

//----------------------------------------------------------------------------
void Effect::SetWorldMatrix(const D3DXMATRIX & worldMatrix)
{
    // Check for a valid world matrix
    if ( !m_worldMatrix ||
        !m_worldMatrix->IsValid() )
    {
        const std::string msg("World matrix effect variable is NULL or invalid. Check that the effect contains a matrix variable named 'world'");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check for a valid world inverse transpose matrix
    if( !m_worldInverseTransposeMatrix ||
        !m_worldInverseTransposeMatrix->IsValid() )
    {
        const std::string msg("World inverse transpose matrix effect variable is NULL or invalid. Check that the effect contains a matrix variable named 'worldInverseTranspose'");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Set the world matrix effect variable
    m_worldMatrix->SetMatrix((float *)&worldMatrix);

    // Set the world inverse transpose matrix effect variable
    D3DXMATRIX inverseTranspose(worldMatrix);
    inverseTranspose._14 = inverseTranspose._24 = inverseTranspose._34 = 0.0f;
    inverseTranspose._41 = inverseTranspose._42 = inverseTranspose._43 = 0.0f;
    inverseTranspose._44 = 1.0f;

    float determinant = D3DXMatrixDeterminant(&inverseTranspose);
    D3DXMatrixInverse(&inverseTranspose, &determinant, &inverseTranspose);
    D3DXMatrixTranspose(&inverseTranspose, &inverseTranspose);

    m_worldInverseTransposeMatrix->SetMatrix((float *)&inverseTranspose);
}

//----------------------------------------------------------------------------
std::auto_ptr<Material> Effect::CreateMaterial() const
{
    return std::auto_ptr<Material>(new Material(m_defaultEffectState));
}

//----------------------------------------------------------------------------
std::auto_ptr<Material> Effect::CreateMaterial(const Material & rhs) const
{
    return std::auto_ptr<Material>(new Material(m_defaultEffectState));

    // TODO - WIP NOT COMPLETED
    Material newEffectState(m_defaultEffectState);

    // Matrices
    for(Material::Matrices::const_iterator itAttribute = m_defaultEffectState.m_matrices.begin();
        itAttribute != m_defaultEffectState.m_matrices.end(); ++itAttribute)
    {
        std::string attributeName                     = itAttribute->first;
        Material::Attribute<D3DXMATRIX> defaultState  = itAttribute->second;
        Material::Attribute<D3DXMATRIX> newState;





    }
}

//----------------------------------------------------------------------------
void Effect::SetMaterial(const Material & material)
{
    // Check that the material is compatible with this effect
    if( material.GetEffectName() != m_name )
    {
        const std::string msg("Material is not compatible with the effect it is being passed to");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Update the current material and shader variables
    UpdateMatrices(material);
    UpdateBools(material);
    UpdateFloats(material);
    UpdateFloat4s(material);
    UpdateTextures(material);  
}

//----------------------------------------------------------------------------
void Effect::UpdateMatrices(const Material & material)
{
    // For all matrix variables in the effect
    for(EffectMatrixVariables::iterator itEffectVariable = m_effectMatrixVariables.begin(); 
        itEffectVariable != m_effectMatrixVariables.end(); ++itEffectVariable)
    {
        const std::string               variableName   = itEffectVariable->first;
        ID3D10EffectMatrixVariable *    effectVariable = itEffectVariable->second;
        Material::Attribute<D3DXMATRIX> defaultState;
        Material::Attribute<D3DXMATRIX> currentState;
        Material::Attribute<D3DXMATRIX> newState;

        // Get the default state of the matrix variable
        Material::Matrices::iterator itDefaultState = m_defaultEffectState.m_matrices.end();

        if( !m_defaultEffectState.m_matrices.empty() )
        {
            itDefaultState = m_defaultEffectState.m_matrices.find(variableName);
        }

        if( itDefaultState == m_defaultEffectState.m_matrices.end() )
        {
            // The default material should always have an entry for the effect variable or it would never have become the default material
            std::string msg("Default material does not contain a variable for effect matrix variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        defaultState = itDefaultState->second;

        // Get the current state of the matrix variable
        Material::Matrices::iterator itCurrentState = m_currentEffectState.m_matrices.end();

        if( !m_currentEffectState.m_matrices.empty() )
        {
            itCurrentState = m_currentEffectState.m_matrices.find(variableName);
        }

        if( itCurrentState == m_currentEffectState.m_matrices.end() )
        {
            // The current material should always have an entry for the effect variable or it would never have become the current material
            std::string msg("Current material does not contain a variable for effect matrix variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        currentState = itCurrentState->second;

        // Get the new state of the matrix variable
        Material::Matrices::const_iterator itNewState = material.m_matrices.end();

        if( !material.m_matrices.empty() )
        {
            itNewState = material.m_matrices.find(variableName);
        }

        if( itNewState == material.m_matrices.end() )
        {
            std::string msg("Effect contains matrix variable not found in given material: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        newState = itNewState->second;

        // 0 0 - If the new state is not initialized and current state is not initialized, then set to the default state
        // 0 1 - If the new state is not initialized and current state is     initialized, then set to the default state
        // 1 0 - If the new state is     initialized and current state is not initialized, then set to new state
        // 1 1 - If the new state is     initialized and current state is     initialized, then set to new state
      
        if( !newState.m_initialized )
        {
            if( currentState.m_value != defaultState.m_value)
            {
                effectVariable->SetMatrix((float*) &(defaultState.m_value));
                m_currentEffectState.m_matrices[variableName] = defaultState;
            }
        }
        else
        {
            if( currentState.m_value != newState.m_value)
            {
                effectVariable->SetMatrix((float*) &(newState.m_value));
            }

            m_currentEffectState.m_matrices[variableName] = newState;
        }
    }
}

//----------------------------------------------------------------------------
void Effect::UpdateBools(const Material & material)
{
    // For all bool variables in the effect
    for(EffectBoolVariables::iterator itEffectVariable = m_effectBoolVariables.begin(); 
        itEffectVariable != m_effectBoolVariables.end(); ++itEffectVariable)
    {
        const std::string            variableName   = itEffectVariable->first;
        ID3D10EffectScalarVariable * effectVariable = itEffectVariable->second;
        Material::Attribute<bool>    defaultState;
        Material::Attribute<bool>    currentState;
        Material::Attribute<bool>    newState;

        // Get the default state of the variable
        Material::Bools::iterator itDefaultState = m_defaultEffectState.m_bools.end();

        if( !m_defaultEffectState.m_bools.empty() )
        {
            itDefaultState = m_defaultEffectState.m_bools.find(variableName);
        }

        if( itDefaultState == m_defaultEffectState.m_bools.end() )
        {
            // The default material should always have an entry for the effect variable or it would never have become the default material
            std::string msg("Default material does not contain a variable for effect bool variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        defaultState = itDefaultState->second;

        // Get the current state of the variable
        Material::Bools::iterator itCurrentState = m_currentEffectState.m_bools.end();

        if( !m_currentEffectState.m_bools.empty() )
        {
            itCurrentState = m_currentEffectState.m_bools.find(variableName);
        }

        if( itCurrentState == m_currentEffectState.m_bools.end() )
        {
            // The current material should always have an entry for the effect variable or it would never have become the current material
            std::string msg("Current material does not contain a variable for effect bool variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        currentState = itCurrentState->second;

        // Get the new state of the variable
        Material::Bools::const_iterator itNewState = material.m_bools.end();

        if( !material.m_bools.empty() )
        {
            itNewState = material.m_bools.find(variableName);
        }

        if( itNewState == material.m_bools.end() )
        {
            std::string msg("Effect contains bool variable not found in given material: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        newState = itNewState->second;

        // 0 0 - If the new state is not initialized and current state is not initialized, then set to the default state
        // 0 1 - If the new state is not initialized and current state is     initialized, then set to the default state
        // 1 0 - If the new state is     initialized and current state is not initialized, then set to new state
        // 1 1 - If the new state is     initialized and current state is     initialized, then set to new state
      
        if( !newState.m_initialized )
        {
            if( currentState.m_value != defaultState.m_value)
            {
                effectVariable->SetBool(defaultState.m_value);
                m_currentEffectState.m_bools[variableName] = defaultState;
            }
        }
        else
        {
            if( currentState.m_value != newState.m_value)
            {
                effectVariable->SetBool(newState.m_value);
            }

            m_currentEffectState.m_bools[variableName] = newState;
        }
    }
}

//----------------------------------------------------------------------------
void Effect::UpdateFloats(const Material & material)
{  
    // For all float variables in the effect
    for(EffectFloatVariables::iterator itEffectVariable = m_effectFloatVariables.begin(); 
        itEffectVariable != m_effectFloatVariables.end(); ++itEffectVariable)
    {
        const std::string            variableName   = itEffectVariable->first;
        ID3D10EffectScalarVariable * effectVariable = itEffectVariable->second;
        Material::Attribute<float>   defaultState;
        Material::Attribute<float>   currentState;
        Material::Attribute<float>   newState;

        // Get the default state of the variable
        Material::Floats::iterator itDefaultState = m_defaultEffectState.m_floats.end();

        if( !m_defaultEffectState.m_floats.empty() )
        {
            itDefaultState = m_defaultEffectState.m_floats.find(variableName);
        }

        if( itDefaultState == m_defaultEffectState.m_floats.end() )
        {
            // The default material should always have an entry for the effect variable or it would never have become the default material
            std::string msg("Default material does not contain a variable for effect float variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        defaultState = itDefaultState->second;

        // Get the current state of the variable
        Material::Floats::iterator itCurrentState = m_currentEffectState.m_floats.end();

        if( !m_currentEffectState.m_floats.empty() )
        {
            itCurrentState = m_currentEffectState.m_floats.find(variableName);
        }

        if( itCurrentState == m_currentEffectState.m_floats.end() )
        {
            // The current material should always have an entry for the effect variable or it would never have become the current material
            std::string msg("Current material does not contain a variable for effect float variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        currentState = itCurrentState->second;

        // Get the new state of the variable
        Material::Floats::const_iterator itNewState = material.m_floats.end();

        if( !material.m_floats.empty() )
        {
            itNewState = material.m_floats.find(variableName);
        }

        if( itNewState == material.m_floats.end() )
        {
            std::string msg("Effect contains float variable not found in given material: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        newState = itNewState->second;

        // 0 0 - If the new state is not initialized and current state is not initialized, then set to the default state
        // 0 1 - If the new state is not initialized and current state is     initialized, then set to the default state
        // 1 0 - If the new state is     initialized and current state is not initialized, then set to new state
        // 1 1 - If the new state is     initialized and current state is     initialized, then set to new state
      
        if( !newState.m_initialized )
        {
            if( currentState.m_value != defaultState.m_value)
            {
                effectVariable->SetFloat(defaultState.m_value);
                m_currentEffectState.m_floats[variableName] = defaultState;
            }
        }
        else
        {
            if( currentState.m_value != newState.m_value)
            {
                effectVariable->SetFloat(newState.m_value);
            }

            m_currentEffectState.m_floats[variableName] = newState;
        }
    }
}

//----------------------------------------------------------------------------
void Effect::UpdateFloat4s(const Material & material)
{  
    // For all float4 variables in the effect
    for(EffectFloat4Variables::iterator itEffectVariable = m_effectFloat4Variables.begin(); 
        itEffectVariable != m_effectFloat4Variables.end(); ++itEffectVariable)
    {
        const std::string                variableName   = itEffectVariable->first;
        ID3D10EffectVectorVariable *     effectVariable = itEffectVariable->second;
        Material::Attribute<D3DXVECTOR4> defaultState;
        Material::Attribute<D3DXVECTOR4> currentState;
        Material::Attribute<D3DXVECTOR4> newState;

        // Get the default state of the variable
        Material::Float4s::iterator itDefaultState = m_defaultEffectState.m_float4s.end();

        if( !m_defaultEffectState.m_float4s.empty() )
        {
            itDefaultState = m_defaultEffectState.m_float4s.find(variableName);
        }

        if( itDefaultState == m_defaultEffectState.m_float4s.end() )
        {
            // The default material should always have an entry for the effect variable or it would never have become the default material
            std::string msg("Default material does not contain a variable for effect float4 variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        defaultState = itDefaultState->second;

        // Get the current state of the variable
        Material::Float4s::iterator itCurrentState = m_currentEffectState.m_float4s.end();

        if( !m_currentEffectState.m_float4s.empty() )
        {
            itCurrentState = m_currentEffectState.m_float4s.find(variableName);
        }

        if( itCurrentState == m_currentEffectState.m_float4s.end() )
        {
            // The current material should always have an entry for the effect variable or it would never have become the current material
            std::string msg("Current material does not contain a variable for effect float4 variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        currentState = itCurrentState->second;

        // Get the new state of the variable
        Material::Float4s::const_iterator itNewState = material.m_float4s.end();

        if( !material.m_float4s.empty() )
        {
            itNewState = material.m_float4s.find(variableName);
        }

        if( itNewState == material.m_float4s.end() )
        {
            std::string msg("Effect contains float4 variable not found in given material: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        newState = itNewState->second;

        // 0 0 - If the new state is not initialized and current state is not initialized, then set to the default state
        // 0 1 - If the new state is not initialized and current state is     initialized, then set to the default state
        // 1 0 - If the new state is     initialized and current state is not initialized, then set to new state
        // 1 1 - If the new state is     initialized and current state is     initialized, then set to new state
      
        if( !newState.m_initialized )
        {
            if( currentState.m_value != defaultState.m_value)
            {
                effectVariable->SetFloatVector((float *)&(defaultState.m_value));
                m_currentEffectState.m_float4s[variableName] = defaultState;
            }
        }
        else
        {
            if( currentState.m_value != newState.m_value)
            {
                effectVariable->SetFloatVector((float *)&(newState.m_value));
            }

            m_currentEffectState.m_float4s[variableName] = newState;
        }
    }
}

//----------------------------------------------------------------------------
void Effect::UpdateTextures(const Material & material)
{
    // For all texture variables in the effect
    for(EffectTextureVariables::iterator itEffectVariable = m_effectTextureVariables.begin(); 
        itEffectVariable != m_effectTextureVariables.end(); ++itEffectVariable)
    {
        const std::string                    variableName   = itEffectVariable->first;
        ID3D10EffectShaderResourceVariable * effectVariable = itEffectVariable->second;
        Material::Attribute<std::string>     defaultState;
        Material::Attribute<std::string>     currentState;
        Material::Attribute<std::string>     newState;

        // Get the default state of the variable
        Material::TextureNames::iterator itDefaultState = m_defaultEffectState.m_textureNames.end();

        if( !m_defaultEffectState.m_textureNames.empty() )
        {
            itDefaultState = m_defaultEffectState.m_textureNames.find(variableName);
        }

        if( itDefaultState == m_defaultEffectState.m_textureNames.end() )
        {
            // The default material should always have an entry for the effect variable or it would never have become the default material
            std::string msg("Default material does not contain a texture name for effect texture variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        defaultState = itDefaultState->second;

        // Get the current state of the variable
        Material::TextureNames::iterator itCurrentState = m_currentEffectState.m_textureNames.end();

        if( !m_currentEffectState.m_textureNames.empty() )
        {
            itCurrentState = m_currentEffectState.m_textureNames.find(variableName);
        }

        if( itCurrentState == m_currentEffectState.m_textureNames.end() )
        {
            // The current material should always have an entry for the effect variable or it would never have become the current material
            std::string msg("Current material does not contain a texture name for effect texture variable: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        currentState = itCurrentState->second;

        // Get the new state of the variable
        Material::TextureNames::const_iterator itNewState = material.m_textureNames.end();

        if( !material.m_textureNames.empty() )
        {
            itNewState = material.m_textureNames.find(variableName);
        }

        if( itNewState == material.m_textureNames.end() )
        {
            std::string msg("Effect contains float4 variable not found in given material: ");
            msg += variableName;
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        newState = itNewState->second;

        // 0 0 - If the new state is not initialized and current state is not initialized, then set to the default state
        // 0 1 - If the new state is not initialized and current state is     initialized, then set to the default state
        // 1 0 - If the new state is     initialized and current state is not initialized, then set to new state
        // 1 1 - If the new state is     initialized and current state is     initialized, then set to new state
      
        if( !newState.m_initialized )
        {
            if( currentState.m_value != defaultState.m_value)
            {
                if( !defaultState.m_initialized )
                {
                    effectVariable->SetResource(NULL);
                }
                else
                {
                    Texture & texture = m_textureManager.GetTexture(defaultState.m_value);
                    texture.SetTextureEffectVariable(effectVariable);
                }

                m_currentEffectState.m_textureNames[variableName] = defaultState;
            }
        }
        else
        {
            if( currentState.m_value != newState.m_value)
            {
                Texture & texture = m_textureManager.GetTexture(newState.m_value);
                texture.SetTextureEffectVariable(effectVariable);
            }

            m_currentEffectState.m_textureNames[variableName] = newState;
        }
    }
}

