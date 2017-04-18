

// Project Includes
#include "Texture.h"

// Common Lib Includes
#include "Exception.h"

//------------------------------------------------------------------------------------------
Texture::Texture(ID3D10Device & device, const std::string & filePath, DXGI_FORMAT format)
    :
    m_device(device),
    m_texture(0),
    m_hasAlpha(false),
    m_width(0),
    m_height(0)
{
    // Attempt to load the image file as a resource in R32G32B32A32 format
    D3DX10_IMAGE_LOAD_INFO loadInfo;
    ZeroMemory(&loadInfo, sizeof(D3DX10_IMAGE_LOAD_INFO));
    loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    loadInfo.Format    = format;

    ID3D10Resource * resource = NULL;
    if( FAILED(D3DX10CreateTextureFromFile(&m_device, filePath.c_str() , &loadInfo, NULL, &resource, NULL)) )
    {
        std::string msg("Failed to load texture from file: ");
        msg += filePath;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Cast the resource into a 2D texture
    D3D10_RESOURCE_DIMENSION type;
    resource->GetType(&type);

    if( type != D3D10_RESOURCE_DIMENSION_TEXTURE2D )
    {
        resource->Release();

        std::string msg("Only 2D textures are supported : ");
        msg += filePath;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    ID3D10Texture2D * texture = static_cast<ID3D10Texture2D *>(resource);
   
    // Get a description of the texture
    D3D10_TEXTURE2D_DESC texDesc;
    texture->GetDesc(&texDesc);

    // Check if that the texture is in R32G32B32A32 format
    if( texDesc.Format != format )
    {
        texture->Release();

        std::string msg("Failed to load texture : ");
        msg += filePath + " in desired format";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Create a shader resource view from the texture for use with directx effects
    D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format                    = texDesc.Format;
    srvDesc.ViewDimension             = D3D10_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels       = texDesc.MipLevels;

    if( FAILED(m_device.CreateShaderResourceView(texture, &srvDesc, &m_texture)) )
    {
        texture->Release();

        std::string msg("Failed to create shader resource view : ");
        msg += filePath;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the width and height
    m_width  = texDesc.Width;
    m_height = texDesc.Height;
   
    // Release the texture interface now that we have a shader resource view
    texture->Release();
}

//------------------------------------------------------------------------------------------
Texture::~Texture()
{
    if( m_texture )
    {
        m_texture->Release();
        m_texture = NULL;
    }
}

//------------------------------------------------------------------------------------------
void Texture::SetTextureEffectVariable(ID3D10EffectShaderResourceVariable * effectVariable)
{
    if( !effectVariable )
    {
        const std::string msg("Effect variable was NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( FAILED(effectVariable->SetResource(m_texture)) )
    {
        const std::string msg("Failed to set effect variable to use texture");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//------------------------------------------------------------------------------------------
unsigned Texture::GetWidth() const
{
    return m_width;
}
   
//------------------------------------------------------------------------------------------
unsigned Texture::GetHeight() const
{
    return m_height;
}

