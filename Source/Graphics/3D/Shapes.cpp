

// Project Includes
#include "Shapes.h"

// Common Lib Includes
#include "Exception.h"

// Standard Includes
#include <cmath>
#include <sstream>

//------------------------------------------------------------------------------
void GenerateSphere(std::vector<Position> & positions,
                    std::vector<TexCoord2D> & texCoords,
                    std::vector<Normal> & normals,
                    std::vector<Index> & indices,
                    const float radius, 
                    const unsigned segments,
                    const bool flipNormals)
{
    // Example Sphere, with 6 segments:
    //
    // 0  1  2  3  4  5
    // |\ |\ |\ |\ |\ |\ 
    // | \| \| \| \| \| \
    // 6  7  8  9  10 11 12
    // | /| /| /| /| /| /|
    // |/ |/ |/ |/ |/ |/ |       as would be seen in an UV editing tool
    // 12 14 15 16 17 18 19
    // | /| /| /| /| /| /
    // |/ |/ |/ |/ |/ |/
    // 34 35 36 37 38 39
    //
    // 0
    //  \
    //   1
    //   |    Verticle Divisions
    //   2
    //  /
    // 3
    //
    //   5 - 4
    //  /     \
    // 0,6     3  Horizontal Divisions
    //  \     /   Middle rings contain an extra vertex at the same position as the start point to allow texture to be wrapped.
    //   1 _ 2    Top and Botton rings do not contain the extra vertex

    // Start with an empty vectors of data
    positions.clear();
    texCoords.clear();
    normals.clear();

    // Check min divisions = 3
    if( segments < 3 )
    {
        std::ostringstream msg("Sphere requires a minimum of 3 segments to be classified as a sphere at all.");
        msg << " segments param: " << segments;

        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    // Check radius > 0
    if( radius <= 0.0f )
    {
        std::ostringstream msg("Sphere requires a radius greater than zero.");
        msg << " radius param: " << radius;

        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    // Generate geometry
    Position   position;
    Normal     normal;
    TexCoord2D texCoord;

    unsigned divisionsH = segments;
    unsigned divisionsV = segments / 2 + 1;

    float incrementU = 1.0f / static_cast<float>(divisionsH);
    float incrementV = 1.0f / static_cast<float>(divisionsV - 1);

    // Top Ring
    for(unsigned i = 0; i < divisionsH; ++i)
    {
        normal.x =  0.0f;
        normal.y =  1.0f;
        normal.z =  0.0f;
      
        position = normal * radius;

        texCoord.x = incrementU * static_cast<float>(i);
        texCoord.y = 0.0f;

        if( flipNormals )
        {
            normal *= -1.0f;
        }

        positions.push_back(position);
        texCoords.push_back(texCoord);
        normals.push_back(normal);
    }

    // Middle Rings
    float angleV = static_cast<float>(D3DX_PI / static_cast<double>(divisionsV - 1));
    float angleH = static_cast<float>(2.0 * D3DX_PI / static_cast<double>(divisionsH));

    for(unsigned j = 1; j < divisionsV - 1; ++j)
    {
        float y          = cos(static_cast<float>(j) * angleV);
        float ringRadius = sin(static_cast<float>(j) * angleV);

        for(unsigned i = 0; i < divisionsH; ++i)
        {
            normal.x = ringRadius * cos(static_cast<float>(D3DX_PI) + static_cast<float>(i) * angleH);
            normal.y = y;
            normal.z = ringRadius * sin(static_cast<float>(D3DX_PI) + static_cast<float>(i) * angleH);

            position = normal * radius;

            texCoord.x = static_cast<float>(i) * incrementU;
            texCoord.y = static_cast<float>(j) * incrementV;

            if( flipNormals )
            {
            normal *= -1.0f;
            }

            positions.push_back(position);
            texCoords.push_back(texCoord);
            normals.push_back(normal);
        }

        // Need an extra vertex at same position as the first to wrap the texture around the sphere
        normal.x = ringRadius * -1.0f;
        normal.y = y;
        normal.z = 0.0f;

        position = normal * radius;

        texCoord.x = 1.0f;
        texCoord.y = static_cast<float>(j) * incrementV;

        if( flipNormals )
        {
            normal *= -1.0f;
        }

        positions.push_back(position);
        texCoords.push_back(texCoord);
        normals.push_back(normal);
    }

    // Bottom Ring
    for(unsigned i = 0; i < divisionsH; ++i)
    {
        normal.x =  0.0f;
        normal.y = -1.0f;
        normal.z =  0.0f;

        position = normal * radius;

        texCoord.x = incrementU * static_cast<float>(i);
        texCoord.y = 1.0f;

        if( flipNormals )
        {
            normal *= -1.0f;
        }

        positions.push_back(position);
        texCoords.push_back(texCoord);
        normals.push_back(normal);
    }

    // At this point we have:
    //
    // A number of rings on the XZ plane equal to the number of segments / 2 + 1
    // The first ring sits at the highest Y value. The last ring sits at the lowest Y value
    // Top and bottom rings have a number of points equal to the number of segments
    // Inner rings have a number of points equal to the number of segments and an extra point positioned the same as the first
    // Each ring starts at the left and circles counter clockwise

    // Generate indices
   
    // Positive Y cap
    for(unsigned i = 0; i < divisionsH; ++i)
    {
        if( !flipNormals )
        {
            indices.push_back(divisionsH + i);
            indices.push_back(i);
            indices.push_back(divisionsH + i + 1);
        }
        else
        {
            indices.push_back(divisionsH + i);
            indices.push_back(divisionsH + i + 1);
            indices.push_back(i);
        }
    }

    // Inner Rings
    unsigned numPoints = 2 * divisionsH + (divisionsV - 2) * (divisionsH + 1); 

    for(unsigned ringA = divisionsH; ringA < numPoints - 2 * divisionsH - 1; ringA+= divisionsH + 1)
    {
        unsigned ringB = ringA + (divisionsH + 1);

        for(unsigned i = 0; i < divisionsH; ++i)
        {
            if( !flipNormals )
            {
            indices.push_back(ringA + i);
            indices.push_back(ringA + i + 1);
            indices.push_back(ringB + i);

            indices.push_back(ringA + i + 1);
            indices.push_back(ringB + i + 1);
            indices.push_back(ringB + i);
            }
            else
            {
            indices.push_back(ringA + i);
            indices.push_back(ringB + i);
            indices.push_back(ringA + i + 1);

            indices.push_back(ringA + i + 1);
            indices.push_back(ringB + i);
            indices.push_back(ringB + i + 1);
            }
        }
    }

    // Negative Y cap
    for(unsigned i = 0; i < segments; ++i)
    {
        if( !flipNormals )
        {
            indices.push_back(numPoints - segments * 2 - 1 + i);
            indices.push_back(numPoints - segments * 2 + i);
            indices.push_back(numPoints - segments + i);
        }
        else
        {
            indices.push_back(numPoints - segments * 2 - 1 + i);
            indices.push_back(numPoints - segments + i);
            indices.push_back(numPoints - segments * 2 + i);
        }
    }
}

//------------------------------------------------------------------------------
void GenerateQuad(std::vector<Position> & positions,
                  std::vector<TexCoord2D> & texCoords,
                  std::vector<Normal> & normals,
                  std::vector<Index> & indices,
                  const D3DXVECTOR2 & dimensions)
{
    // Start with an empty vectors of data
    positions.clear();
    texCoords.clear();
    normals.clear();

    // Create the quad
    float x = 0.5f * dimensions.x;
    float y = 0.5f * dimensions.y;

    positions.push_back(Position( -x, -y, 0.0f));
    positions.push_back(Position(  x, -y, 0.0f));
    positions.push_back(Position( -x,  y, 0.0f));
    positions.push_back(Position(  x,  y, 0.0f));

    normals.push_back(Normal(0.0f, 0.0f, 1.0f));
    normals.push_back(Normal(0.0f, 0.0f, 1.0f));
    normals.push_back(Normal(0.0f, 0.0f, 1.0f));
    normals.push_back(Normal(0.0f, 0.0f, 1.0f));

    texCoords.push_back(TexCoord2D( 0.0f,  0.0f));
    texCoords.push_back(TexCoord2D( 1.0f,  0.0f));
    texCoords.push_back(TexCoord2D( 0.0f,  1.0f));
    texCoords.push_back(TexCoord2D( 1.0f,  1.0f));
}