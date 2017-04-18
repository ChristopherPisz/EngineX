

#include "TextureManager.h"

// Common Lib Includes
#include "Exception.h"

//----------------------------------------------------------------------------
TextureManager::TextureManager(ID3D10Device & device,  const std::string & textureDirectory)
    :
    m_device(device),
    m_textureDirectory(textureDirectory)
{
}

//----------------------------------------------------------------------------
TextureManager::~TextureManager()
{
    // Release all textures
    for(TextureMap::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
    {
        delete it->second;
    }
}

//----------------------------------------------------------------------------
Texture & TextureManager::CreateTextureFromFile(const std::string & textureName, 
                                                const std::string & textureFileName, 
                                                DXGI_FORMAT format)
{
    // Check if the texture already exists
    TextureMap::iterator it = m_textures.end();

    if( !m_textures.empty() )
    {
        it = m_textures.find(textureName);
    }

    if( it != m_textures.end() )
    {
        return *(it->second);
    }

    // Create the texture
    Texture * texture = NULL;
    std::string textureFilePath = m_textureDirectory + "\\" + textureFileName;

    try
    {
        texture = new Texture(m_device, textureFilePath, format);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    m_textures[textureName] = texture;

    return *texture;
}

//----------------------------------------------------------------------------
Texture & TextureManager::GetTexture(const std::string & textureName)
{
    // Check if an texture by that name exists
    TextureMap::iterator it = m_textures.end();
   
    if( !m_textures.empty() )
    {
        it = m_textures.find(textureName);
    }
   
    if( it == m_textures.end() )
    {
        std::string msg("No texture loaded by the name: ");
        msg += textureName;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Return the texture
    return *(it->second);
}

