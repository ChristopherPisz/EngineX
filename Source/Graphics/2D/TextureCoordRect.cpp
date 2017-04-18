

// Project Includes
#include "TextureCoordRect.h"

// Common Lib Includes
#include "Exception.h"

//---------------------------------------------------------------------------------
TextureCoordRect::TextureCoordRect()
    :
    m_topLeft(D3DXVECTOR2(0.0f, 0.0f)),
    m_bottomRight(D3DXVECTOR2(0.0f, 0.0f))
{
}

//---------------------------------------------------------------------------------
TextureCoordRect::TextureCoordRect(D3DXVECTOR2 topLeft, D3DXVECTOR2 bottomRight)
    :
    m_topLeft(topLeft),
    m_bottomRight(bottomRight)
{
    if( m_topLeft.x	  < 0.0f || m_topLeft.x	  > 1.0f || m_topLeft.y	  < 0.0f || m_topLeft.y	  > 1.0f ||
        m_bottomRight.x < 0.0f || m_bottomRight.x > 1.0f || m_bottomRight.y < 0.0f || m_bottomRight.y > 1.0f)
    {
        const std::string msg("Created texture coordinate rect with invalid coordinates");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------------
TextureCoordRect::TextureCoordRect(float topLeft_u, float topLeft_v, float bottomRight_u, float bottomRight_v)
    :
    m_topLeft(topLeft_u, topLeft_v),
    m_bottomRight(bottomRight_u, bottomRight_v)
{
    if( m_topLeft.x	  < 0.0f || m_topLeft.x	  > 1.0f || m_topLeft.y	  < 0.0f || m_topLeft.y	  > 1.0f ||
        m_bottomRight.x < 0.0f || m_bottomRight.x > 1.0f || m_bottomRight.y < 0.0f || m_bottomRight.y > 1.0f)
    {
        const std::string msg("Created texture coordinate rect with invalid coordinates");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------------
TextureCoordRect::TextureCoordRect(const TextureCoordRect & rhs)
    :
    m_topLeft(rhs.m_topLeft),
    m_bottomRight(rhs.m_bottomRight)
{
}

//---------------------------------------------------------------------------------
TextureCoordRect::~TextureCoordRect()
{
}

//----------------------------------------------------------------------------------	
TextureCoordRect & TextureCoordRect::operator = (const TextureCoordRect & rhs)
{
    m_topLeft       = rhs.m_topLeft;
    m_bottomRight   = rhs.m_bottomRight;

    return *this;
}

//----------------------------------------------------------------------------------
bool TextureCoordRect::operator == (const TextureCoordRect & rhs)
{
    return( m_topLeft == rhs.m_topLeft && m_bottomRight == rhs.m_bottomRight);
}

