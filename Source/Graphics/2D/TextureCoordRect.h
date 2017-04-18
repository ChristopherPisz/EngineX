#ifndef TEXTURECOORDRECT_H
#define TEXTURECOORDRECT_H

#include <d3d9.h>
#include <d3dx9.h>

//-------------------------------------------------------------------------------
// Simple structure to hold a rectangle of texture coordinates
class TextureCoordRect
{
public:
	TextureCoordRect();
	TextureCoordRect(D3DXVECTOR2 topLeft, D3DXVECTOR2 bottomRight);
	TextureCoordRect(float topLeft_u, float topLeft_v, float bottomRight_u, float bottomRight_v);
	TextureCoordRect(const TextureCoordRect & rhs);
	~TextureCoordRect();
	
	TextureCoordRect & operator = (const TextureCoordRect & rhs);
	bool operator == (const TextureCoordRect & rhs);

	D3DXVECTOR2 m_topLeft;
	D3DXVECTOR2 m_bottomRight;
};

#endif