

#pragma once

// EngineX Includes
#include "Graphics\3D\Buffers.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>
#include <map>

class Effect;

//----------------------------------------------------------------------------
/**
* Material
*
* Dynamic container of variables used by an Effect.
*
* A Material first has to be created by an Effect in order to allocate the differant
* types of attributes the Effect uses. It can then be obtained from and given back to 
* the Effect, which will use the attributes of the Material to set the variables of 
* the Effect.
*/
class Material
{
public:

    /**
    * Material creation, lifetime, and state management is tightly coupled with the effect class
    * Be VERY CAREFUL to reflect changes in material implementation in the effect implementation
    **/
    friend class Effect;

    /**
    * Copy Constructor
    */
    Material(const Material & rhs);

    /**
    * Deconstructor
    */
    ~Material();
   
    /**
    * Assignment Operator
    */
    Material & operator = (const Material & rhs);


    /**
    * Get the name of the effect that created this material and to which this material provides attributes to 
    */
    const std::string & GetEffectName() const;


    /**
    * Gets an existing matrix attribute
    *
    * @param variableName - Name of the matrix effect variable as it appears in the effect that created this material
    *
    * @return D3DXMATRIX - The current value that will be used to set the matrix effect variable
    **/
    const D3DXMATRIX & GetMatrix(const std::string & variableName) const;

    /**
    * Sets an existing matrix attribute
    *
    * @param variableName - Name of the matrix effect variable as it appears in the effect that created this material
    * @param value        - The current value that will be used to set the matrix effect variable
    */
    void SetMatrix(const std::string & variableName, const D3DXMATRIX & value);


    /**
    * Gets an existing bool attribute
    *
    * @param variableName - Name of the bool effect variable as it appears in the effect that created this material
    *
    * @return bool - The current value that will be used to set the bool effect variable
    **/
    const bool GetBool(const std::string & variableName) const;

    /**
    * Sets an existing bool attribute
    *
    * @param variableName - Name of the bool effect variable as it appears in the effect that created this material
    * @param value        - The current value that will be used to set the bool effect variable
    */
    void SetBool(const std::string & variableName, const bool value);



    /**
    * Gets an existing float attribute
    *
    * @param variableName - Name of the float effect variable as it appears in the effect that created this material
    *
    * @return float - The current value that will be used to set the float effect variable
    **/
    const float GetFloat(const std::string & variableName) const;

    /**
    * Sets an existing float attribute
    *
    * @param variableName - Name of the float effect variable as it appears in the effect that created this material
    * @param value        - The current value that will be used to set the float effect variable
    */
    void SetFloat(const std::string & variableName, const float value);



    /**
    * Gets an existing float4 attribute
    *
    * @param variableName - Name of the float4 effect variable as it appears in the effect that created this material
    *
    * @return D3DXVECTOR4 - The current value that will be used to set the float4 effect variable
    **/
    const D3DXVECTOR4 GetFloat4(const std::string & variableName) const;

    /**
    * Sets an existing float4 attribute
    *
    * @param variableName - Name of the float4 effect variable as it appears in the effect that created this material
    * @param value        - The current value that will be used to set the float4 effect variable
    */
    void SetFloat4(const std::string & variableName, const D3DXVECTOR4 & value);

   

    /**
    * Gets an existing texture name attribute
    *
    * @param variableName - Name of the texture effect variable as it appears in the effect that created this material
    *
    * @return std::strring - The current name of the texture, as it appears in the TextureManager, that will be used to set the
    *                        texture effect variable
    **/
    const std::string GetTextureName(const std::string & variableName) const;

    /**
    * Sets an existing texture name attribute
    *
    * @param variableName - Name of the texture effect variable as it appears in the effect that created this material
    * @param textureName  - The name of the texture, as it appears in the TextureManager, that will be used to set the
    *                       texture effect variable
    */
    void SetTextureName(const std::string & variableName, const std::string & textureName);

private:

    /**
    * Constructor
    */
    Material(const std::string & effectName);

    /**
    * Creates an unitialized matrix attribute
    *
    * @param variableName - Name of the matrix effect variable as it appears in the effect that created this material
    * @param defaultValue - The value the effect returned for this variable at creation
    */
    void CreateMatrix(const std::string & variableName, const D3DXMATRIX & defaultValue);

    /**
    * Creates an unitialized bool attribute
    *
    * @param variableName - Name of the bool effect variable as it appears in the effect that created this material
    * @param defaultValue - The value the effect returned for this variable at creation
    */
    void CreateBool(const std::string & variableName, bool defaultValue);

    /**
    * Creates an unitialized float attribute
    *
    * @param variableName - Name of the float effect variable as it appears in the effect that created this material
    * @param defaultValue - The value the effect returned for this variable at creation
    */
    void CreateFloat(const std::string & variableName, float defaultValue);

    /**
    * Creates an unitialized float4 attribute
    *
    * @param variableName - Name of the float4 effect variable as it appears in the effect that created this material
    * @param defaultValue - The value the effect returned for this variable at creation
    */
    void CreateFloat4(const std::string & variableName, const D3DXVECTOR4 & defaultValue);

    /**
    * Creates an unitialized texture name attribute
    *
    * @param variableName - Name of the texture effect variable as it appears in the effect that created this material
    */
    void CreateTextureName(const std::string & variableName);



    /** Name of the effect that created this material and to whom it provides attributes to */
    std::string m_effectName;

    /**
    * Material attribute value data structure
    *
    * Each attribute is first created by the effect that creates the material.
    * However, the attributes are not initialized by the effect, but are initialized
    * after the material has been created. This data structure adds a flag to the 
    * attribute value to determine if it has been initialized. 
    */
    template <class T>
    struct Attribute
    {
        bool m_initialized;
        T    m_value;
    };

    /**
    * Map of matrix attributes
    * 
    * key   - matrix variable name as it appears in the DirectX effect
    * value - Attribute structure containing the matrix 
    */
    typedef std::map<std::string, Attribute<D3DXMATRIX> > Matrices;
    Matrices m_matrices;

    /** 
    * Map of bool attributes
    * 
    * key   - bool variable name as it appears in the DirectX effect
    * value - Attribute structure containing the bool
    */
    typedef std::map<std::string, Attribute<bool> > Bools;
    Bools m_bools;

    /** 
    * Map of float attributes
    * 
    * key - float variable name as it appears in the DirectX effect
    * value - Attribute structure containing the float
    */
    typedef std::map<std::string, Attribute<float> > Floats;
    Floats m_floats;

    /** 
    * Map of float attributes
    * 
    * key - float variable name as it appears in the DirectX effect
    * value - Attribute structure containing the float
    */
    typedef std::map<std::string, Attribute<D3DXVECTOR4> > Float4s;
    Float4s m_float4s;

    /** 
    * Map of the texture name attributes
    * 
    * key - texture variable name as it appears in the DirectX effect
    * value - Attribute structure containing the texture name as it appears in the texture manager 
    */
    typedef std::map<std::string, Attribute<std::string> > TextureNames;
    TextureNames m_textureNames;
};

