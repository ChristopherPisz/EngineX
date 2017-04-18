#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

// EngineX Includes
#include "Texture.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>
#include <map>


class TextureManager
{
public:

   /**
   * Constructor
   *
   * @param device           - Intialized Direct3D device
   * @param textureDirectory - Directory that contains all texture files
   */
   TextureManager(ID3D10Device & device, const std::string & textureDirectory);

   /**
   * Deconstructor
   */
   ~TextureManager();


   /**
   * Creates a texture from a file
   *
   * If an effect by the same name already exists, a new texture will not be created and a reference
   * to the old effect returned
   *
   * @param textureName     - Name of the texture for the application to refer to
   * @param textureFileName - Filename of the image file that is the texture
   * @param format          - Desired format to store the loaded texture in
   * @return Texture &      - reference to the created texture
   *
   * @throws BaseException - If texture creation fails
   */
   Texture & CreateTextureFromFile(const std::string & textureName, 
                                   const std::string & textureFileName, 
                                   DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);

   /**
   * Gets a loaded texture
   *
   * @param textureName - Name that was given to the requested texture when it was created
   *
   * @throws BaseException - If the requested texture does not exist
   */
   Texture & GetTexture(const std::string & textureName);

protected:

private:

   ID3D10Device & m_device;
   std::string    m_textureDirectory;

   /**
   * Textures
   *
   * Key   - string containing the name assigned to the texture at creation
   * Value - pointer to the texture object
   **/
   typedef std::map<std::string, Texture *> TextureMap;
   TextureMap m_textures;
};


#endif // TEXTUREMANAGER_H
