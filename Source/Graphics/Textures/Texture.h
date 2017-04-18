
#ifndef TEXTURE_H
#define TEXTURE_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>

//------------------------------------------------------------------------------------------
/**
* Wrapper around the Direct3D texture resource
*/
class Texture
{
public:

   /**
   * Constructor
   *
   * @param device   - Direct3D device
   * @param filePath - Path to the texture file to load
   * @param format   - Desired format to store the loaded texture in
   *
   * @throws BaseException - if the texture cannot be loaded
   */
   Texture(ID3D10Device & device, const std::string & filePath, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);

   /**
   * Deconstructor
   */
   ~Texture();

   /**
   * Sets an effect variable to use this texture
   */
   void SetTextureEffectVariable(ID3D10EffectShaderResourceVariable * effectVariable);

   /**
   * Gets the width of the texture image in pixels
   **/
   unsigned GetWidth() const;
   
   /**
   * Gets the height of the texture image in pixels
   **/
   unsigned GetHeight() const;

private:

   /**
   * Copy Constructor
   *
   * You cannot copy this class, there should only be one instance of a particular texture
   * in video memory. There are ways to get an manupulate data and create a new resource
   * from it, which might be added later.
   */
   Texture(const Texture & rhs);


   ID3D10Device &              m_device;
   ID3D10ShaderResourceView *  m_texture;
   bool                        m_hasAlpha;
   unsigned                    m_width;
   unsigned                    m_height;
};

#endif