

// Project Includes
#include "Font2D.h"

// Common Lib Includes
#include "Exception.h"

//----------------------------------------------------------------------------------
Font2D::Font2D(LPDIRECT3DDEVICE9 device)
    :
    m_name("Uninitialized Font"),
    m_numGlyphs(0),
    m_height(0),
    m_glyphWidths(),
    m_glyphRects()
{
    // Share the device interface
    m_device = device;
    if(m_device)
    {
        m_device->AddRef();
    }

    // No texture set yet
    m_texture = NULL;	
}

//----------------------------------------------------------------------------------
Font2D::Font2D(const Font2D & rhs)
    :
    m_name(rhs.m_name),
    m_numGlyphs(rhs.m_numGlyphs),
    m_height(rhs.m_height),
    m_glyphWidths(rhs.m_glyphWidths),
    m_glyphRects(rhs.m_glyphRects)
{
    // Share the device interface
    m_device = rhs.m_device;
    if(m_device)
    {
        m_device->AddRef();
    }

    // Share the texture
    m_texture = rhs.m_texture;
    if(m_texture)
    {
        m_texture->AddRef();
    }
}

//----------------------------------------------------------------------------------
Font2D::~Font2D()
{
    if(m_device)
    {
        m_device->Release();
    }

    if(m_texture)
    {
        m_texture->Release();
    }
}

//-----------------------------------------------------------------------------------
Font2D & Font2D::operator = (const Font2D & rhs)
{
    m_name          = rhs.m_name;
    m_numGlyphs     = rhs.m_numGlyphs;
    m_height        = rhs.m_height;
    m_glyphWidths   = rhs.m_glyphWidths;
    m_glyphRects    = rhs.m_glyphRects;

    //  Release the old device
    if(m_device)
        m_device->Release();

    // Share the device interface
    m_device = rhs.m_device;
    if(m_device)
        m_device->AddRef();

    // Release the old texture
    if(m_texture)
        m_texture->Release();

    // Share the texture
    m_texture = rhs.m_texture;
    if(m_texture)
        m_texture->AddRef();

    return *this;
}

//-----------------------------------------------------------------------------------
void Font2D::CreateFont(std::string name, std::string path, unsigned numGlyphs, GlyphWidths & widths, unsigned int fontHeight)
{
    D3DXIMAGE_INFO	info;    // Information about the art file

    // Check for a valid Direct3D device
    if(!m_device)
    {
        std::string msg("Direct3D device has not been initialized before attempting to load ");
        msg += path;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Extract the image information
    if(FAILED(D3DXGetImageInfoFromFile(path.c_str(), &info)))
    {
        std::string msg("Failed to get file info from ");
        msg += path;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check that there are widths for each glyph
    if(widths.size() != numGlyphs)
    {
        std::string msg("An equal amount of widths must be supplied for the number of glyphs in font ");
        msg += name;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check that the font image may be evenly divisable into 16 glyphs per row 
    if(info.Width % 16)
    {
        std::string msg(path);
        msg += " is not evenly divisible into 16 glyphs per row";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
    
    // Calculate the number of rows required when there will be 16 glyphs per row
    unsigned int numRows = numGlyphs / 16;
    if(numGlyphs % 16)
        numRows++;

    // Check that the font image contains the needed number of rows
    if(numRows * fontHeight != info.Height)
    {
        std::string msg(path);
        msg += " does not contain the needed number of rows at the requested font hieght";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Assign attributes
    m_name			= name;
    m_numGlyphs		= numGlyphs;
    m_height		= fontHeight;
    m_glyphWidths	= widths;

    // If there is a texture already, then release it
    if(m_texture)
    {
        m_texture->Release();
    }

    // Create a new texture from the image file
    if(FAILED(D3DXCreateTextureFromFileEx(m_device,		// Direct3D device
            path.c_str(),								// Path to file to load
            D3DX_DEFAULT,								// Width is rounded up to next power of 2
            D3DX_DEFAULT,								// Height is rounded up to next power of 2
            0,											// Number of mipmaps
            0,											// Usage flag
            D3DFMT_A8R8G8B8,							// Format 
            D3DPOOL_MANAGED,							// Memory pool
            D3DX_DEFAULT,								// Filter
            D3DX_DEFAULT,								// Mipmap filter
            0,											// Color key
            NULL,										// Source image info out
            NULL,										// Palette out
            &m_texture)))								// Texture out
    {
        std::string msg("Failed to load font image from ");
        msg += path;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if we got A8R8G8B8 format back, if not suggest change differant file format
    D3DSURFACE_DESC desc;
    m_texture->GetLevelDesc(0, &desc);
    if(desc.Format != D3DFMT_A8R8G8B8)
    {
        std::string msg("Font image did not load in A8R8G8B8 format from ");
        msg += path;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // The top left pixel is to serve as a color key for transparency
        
    // Lock the pixel data from the texture
    D3DLOCKED_RECT	data;
    if(FAILED(m_texture->LockRect(0, &data, NULL, 0)))
    {
        std::string msg("Failed to lock texture!");
        msg += path;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Create an array of bytes so we can calculate where in the data to look for pixels
    BYTE * textureBuffer = static_cast<BYTE *>(data.pBits);					// Byte array of pixel data

    // Get the A8R8G8B8 color to compare against to determine which pixels to force transparency on
    // Disgard the alpha channel by forcing it to be opaque for comparison
    DWORD colorkey = 0xFF000000 | *(static_cast<DWORD *>(data.pBits));

    // Iterate through each pixel
    for(unsigned int y = 0; y < info.Height; y++)
    {
        for(unsigned int x = 0; x < info.Width; x++)
        {
            // Get a pointer to the pixel at the current row and column
            DWORD * pixel = reinterpret_cast<DWORD *>(textureBuffer + y * data.Pitch + x * 4);

            // Compare the color without alpha channel
            DWORD color = (*pixel) | 0xFF000000;

            // If the color mathced the top left pixel than make this pixel transparent
            if(color == colorkey)
            {
                *pixel = (*pixel) & 0x00FFFFFF;
            }
        }
    }

    m_texture->UnlockRect(0);
    
    // Calculate the Glyph Rects
    for(unsigned int glyphNum = 1; glyphNum <= m_numGlyphs; glyphNum++)
    {	
        // Calculate the center point, in pixel coordinates, of the rectangle that encompesses the glyph
        float center_x = (static_cast<float>(( glyphNum - 1) % 16) + 0.5f) * static_cast<float>(info.Width / 16);
        float center_y = (static_cast<float>(( glyphNum - 1) / 16) + 0.5f) * static_cast<float>(m_height);

        // Calculate the rectangle that encompasses each glyph in pixel coordinates
        D3DXVECTOR2 topLeft(center_x - static_cast<float>(m_glyphWidths[glyphNum - 1]) / 2.0f, 
                            center_y - static_cast<float>(m_height) / 2.0f);

        D3DXVECTOR2 bottomRight(center_x + static_cast<float>(m_glyphWidths[glyphNum - 1]) / 2.0f, 
                                center_y + static_cast<float>(m_height) / 2.0f);

        // Transform the rectangle coordinates from pixel coordinates into texture coordinates
        topLeft.x		*= 1.0f / static_cast<float>(info.Width);
        bottomRight.x	*= 1.0f / static_cast<float>(info.Width);
        topLeft.y		*= 1.0f / static_cast<float>(info.Height);
        bottomRight.y	*= 1.0f / static_cast<float>(info.Height);

        // Save texture rects
        TextureCoordRect tRect(topLeft, bottomRight);
        m_glyphRects.push_back(tRect);
    }
}
