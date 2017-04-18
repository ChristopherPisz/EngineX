#ifndef SHAPES_H
#define SHAPES_H

// EngineX Includes
#include "Graphics\3D\Buffers.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <vector>

//------------------------------------------------------------------------------

/**
* Creates a sphere as an indexed triangle list
**/
void GenerateSphere(std::vector<Position> & positions,
                    std::vector<TexCoord2D> & texCoords,
                    std::vector<Normal> & normals,
                    std::vector<Index> & indices,
                    const float radius, 
                    const unsigned segments,
                    const bool flipNormals = false);

/**
* Creates a quad as a triangle strip
**/
void GenerateQuad(std::vector<Position> & positions,
                  std::vector<TexCoord2D> & texCoords,
                  std::vector<Normal> & normals,
                  std::vector<Index> & indices,
                  const D3DXVECTOR2 & dimensions);



#endif // SHAPES_H

