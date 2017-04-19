


#include "PolygonSetParser.h"

// EngineX Includes
#include "Graphics\Effects\Effect.h"
#include "Graphics\Effects\Technique.h"

// Common Lib Includes
#include "Exception.h"
#include "StringUtility.h"

// Standard Includes
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctype.h>

//---------------------------------------------------------------------------
PolygonSetParser::PolygonSetParser(ID3D10Device & device, 
                                   InputLayoutManager & inputLayoutManager,
                                   TextureManager & textureManager,
                                   EffectManager & effectManager)
    :
    m_device(device),
    m_inputLayoutManager(inputLayoutManager),
    m_textureManager(textureManager),
    m_effectManager(effectManager)
{
}

//---------------------------------------------------------------------------
PolygonSetParser::~PolygonSetParser()
{
    // Release all the PolygonSet objects that remain
    std::vector<PolygonSet *>::iterator it = m_polygonSets.begin();

    while( !m_polygonSets.empty() )
    {
        delete (*it);
        it = m_polygonSets.erase(it);
    }
}

//---------------------------------------------------------------------------
void PolygonSetParser::ParseFile(const std::string & filepath,
                                 const bool generateTangentData)
{
    // Open the file
    std::ifstream file(filepath.c_str(), std::fstream::in | std::fstream::binary);

    // Check if the file was successfully opened
    if( file.fail() )
    {
        std::string msg("Failed to open file: ");
        msg += filepath;
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Release the current polygon sets
    std::vector<PolygonSet *>::iterator it = m_polygonSets.begin();

    while( !m_polygonSets.empty() )
    {
        delete (*it);
        it = m_polygonSets.erase(it);
    }

    // Read in a data identifier (1 byte unsigned integral value)
    unsigned int dataID = 0;
    file.read(reinterpret_cast<char *>(&dataID), 1);

    while( file )
    {
        switch( dataID )
        {
            // Materials
            case MATERIAL_START:
            {
                ParseMaterial(file);
                break;
            }

            // Vertices
            case VERTICES_START:
            {
                ParseVertices(file);
                break;
            }

            // Signal's all data was parsed for one polygon set
            case POLYGONSET_END:
            {
                CreatePolygonSet(generateTangentData);
                break;
            }
         
            // Unknown
            default:
            {
                std::ostringstream msg;
                msg << "Unknown data identifier encountered in file: " << filepath << " at streampos " << file.tellg();

                throw Common::Exception(__FILE__, __LINE__, msg.str());
            }
        }

        file.read(reinterpret_cast<char *>(&dataID), 1);
    }

    file.close();
    }

    //---------------------------------------------------------------------------
    void PolygonSetParser::ParseMaterial(std::ifstream & file)
    {
    // Release the current material
    m_material.reset();

    // Get the material type (1 byte unsigned integral value)
    unsigned materialType = 0;
    file.read(reinterpret_cast<char *>(&materialType), 1);

    if( !file )
    {
        const std::string msg("Error parsing material type");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
   
    // Check if we know how to parse this material type
    if( materialType >= NUM_MATERIAL_TYPES )
    {
        const std::string msg("Unknown material type");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the shader type (1 byte unsigned integral value)
    unsigned shaderType = 0;
    file.read(reinterpret_cast<char *>(&shaderType), 1);

    if( !file )
    {
        const std::string msg("Error parsing shader type");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if we know how to parse this shader type
    if( shaderType > NUM_SHADERTYPES )
    {
        const std::string msg("Unknown shader type");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get the whther or not the ambient color is mapped to a texture (1 byte unsigned integral value)
    unsigned    ambientMapped = 0;
    D3DXCOLOR   ambientColor(0.0f, 0.0f, 0.0f, 0.0f);
    std::string ambientMapFileName("");

    file.read(reinterpret_cast<char *>(&ambientMapped), 1);

    if( !file )
    {
        const std::string msg("Error parsing whether or not the ambient channel is mapped");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( ambientMapped )
    {
        // Get the texture filename the ambient color is mapped to
        char oneByte = 0x01;

        while( oneByte != 0x00 )
        {
            oneByte = file.get();
            ambientMapFileName += oneByte;
        }

        if( !file )
        {
            const std::string msg("Error parsing filename of texture ambient is mapped to");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }
    else
    {
        // Get the ambient color
        file.read(reinterpret_cast<char *>(&ambientColor.r), 4);
        file.read(reinterpret_cast<char *>(&ambientColor.g), 4);
        file.read(reinterpret_cast<char *>(&ambientColor.b), 4);
        file.read(reinterpret_cast<char *>(&ambientColor.a), 4);

        if( !file )
        {
            const std::string msg("Error parsing ambient color");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }

    // Get the whether or not the emissive color is mapped to a texture (1 byte unsigned integral value)
    unsigned    emissiveMapped = 0;
    D3DXCOLOR   emissiveColor(0.0f, 0.0f, 0.0f, 0.0f);
    std::string emissiveMapFileName("");

    file.read(reinterpret_cast<char *>(&emissiveMapped), 1);

    if( !file )
    {
        const std::string msg("Error parsing whether or not the emissive channel is mapped");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( emissiveMapped )
    {
        // Get the texture file name the emissive color is mapped to
        char oneByte = 0x01;

        while( oneByte != 0x00 )
        {
            oneByte = file.get();
            emissiveMapFileName += oneByte;
        }

        if( !file )
        {
            const std::string msg("Error parsing filename of texture emissive is mapped to");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }
    else
    {
        // Get the emissive color
        file.read(reinterpret_cast<char *>(&emissiveColor.r), 4);
        file.read(reinterpret_cast<char *>(&emissiveColor.g), 4);
        file.read(reinterpret_cast<char *>(&emissiveColor.b), 4);
        file.read(reinterpret_cast<char *>(&emissiveColor.a), 4);

        if( !file )
        {
            const std::string msg("Error parsing emissive color");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }

    // Get the whether or not the diffuse color is mapped to a texture (1 byte unsigned integral value)
    unsigned    diffuseMapped = 0;
    D3DXCOLOR   diffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
    std::string diffuseMapFileName("");

    file.read(reinterpret_cast<char *>(&diffuseMapped), 1);

    if( !file )
    {
        const std::string msg("Error parsing whether or not the diffuse channel is mapped");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( diffuseMapped )
    {
        // Get the texture file name the diffuse color is mapped to
        char oneByte = 0x01;

        while( oneByte != 0x00 )
        {
            oneByte = file.get();
            diffuseMapFileName += oneByte;
        }

        if( !file )
        {
            const std::string msg("Error parsing filename of texture diffuse is mapped to");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }
    else
    {
        // Get the diffuse color
        file.read(reinterpret_cast<char *>(&diffuseColor.r), 4);
        file.read(reinterpret_cast<char *>(&diffuseColor.g), 4);
        file.read(reinterpret_cast<char *>(&diffuseColor.b), 4);
        file.read(reinterpret_cast<char *>(&diffuseColor.a), 4);

        if( !file )
        {
            const std::string msg("Error parsing diffuse color");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }

    // Get the whether or not the specular color is mapped to a texture (1 byte unsigned integral value)
    unsigned    specularMapped = 0;
    D3DXCOLOR   specularColor(0.0f, 0.0f, 0.0f, 0.0f);
    std::string specularMapFileName("");
    float       specularExponent = 0.0f;

    file.read(reinterpret_cast<char *>(&specularMapped), 1);

    if( !file )
    {
        const std::string msg("Error parsing whether or not the specular channel is mapped");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( specularMapped )
    {
        // Get the texture file name the specular color is mapped to
        char oneByte = 0x01;

        while( oneByte != 0x00 )
        {
            oneByte = file.get();
            specularMapFileName += oneByte;
        }

        if( !file )
        {
            const std::string msg("Error parsing filename of texture specular is mapped to");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }
    else
    {
        // Get the specular color
        file.read(reinterpret_cast<char *>(&specularColor.r), 4);
        file.read(reinterpret_cast<char *>(&specularColor.g), 4);
        file.read(reinterpret_cast<char *>(&specularColor.b), 4);
        file.read(reinterpret_cast<char *>(&specularColor.a), 4);

        if( !file )
        {
            const std::string msg("Error parsing specular color");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }
    }

    // Get the specular exponent
    file.read(reinterpret_cast<char *>(&specularExponent), 4);

    if( !file )
    {
        const std::string msg("Error parsing specular exponent");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    //-----
    // Create the effect
    RemoveExtFromFilename("PerPixelPhong.fx", m_effectName);
    m_techniqueName = "RenderDefault";

    Effect * effect       = NULL;
    Technique * technique = NULL;
   
    try
    {  
        effect    = &(m_effectManager.CreateChildEffect(m_effectName, "PerPixelPhong.fx"));
        technique = &(effect->GetTechnique(m_techniqueName));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Get a material
    m_material = effect->CreateMaterial();

    // Set the material attributes
   
    // Ambient
    if( ambientMapped )
    {
        std::string ambientMapName;
        RemoveExtFromFilename(ambientMapFileName, ambientMapName);
        m_textureManager.CreateTextureFromFile(ambientMapName, ambientMapFileName);

        m_material->SetTextureName("ambientTexture", ambientMapName);
        m_material->SetBool("ambientMapped", true);
    }
    else
    {
        m_material->SetFloat4("ambientColor", static_cast<D3DXVECTOR4>(ambientColor));
        m_material->SetBool("ambientMapped", false);
    }

    // Emissive
    if( emissiveMapped )
    {
        std::string emissiveMapName;
        RemoveExtFromFilename(emissiveMapFileName, emissiveMapName);
        m_textureManager.CreateTextureFromFile(emissiveMapName, emissiveMapFileName);

        m_material->SetTextureName("emissiveTexture", emissiveMapName);
        m_material->SetBool("emissiveMapped", true);
    }
    else
    {
        m_material->SetFloat4("emissiveColor", static_cast<D3DXVECTOR4>(emissiveColor));
        m_material->SetBool("emissiveMapped", false);
    }

    // Diffuse
    if( diffuseMapped )
    {
        std::string diffuseMapName;
        RemoveExtFromFilename(diffuseMapFileName, diffuseMapName);
        m_textureManager.CreateTextureFromFile(diffuseMapName, diffuseMapFileName);

        m_material->SetTextureName("diffuseTexture", diffuseMapName);
        m_material->SetBool("diffuseMapped", true);
    }
    else
    {
        m_material->SetFloat4("diffuseColor", static_cast<D3DXVECTOR4>(diffuseColor));
        m_material->SetBool("diffuseMapped", false);
    }
   
    // Specular
    if( specularMapped )
    {
        std::string specularMapName;
        RemoveExtFromFilename(specularMapFileName, specularMapName);
        m_textureManager.CreateTextureFromFile(specularMapName, specularMapFileName);

        m_material->SetTextureName("specularTexture", specularMapName);
        m_material->SetBool("specularMapped", true);
    }
    else
    {
        m_material->SetFloat4("specularColor", static_cast<D3DXVECTOR4>(specularColor));
        m_material->SetBool("specularMapped", false);
    }

    m_material->SetFloat("specularExponent", specularExponent);
}

//---------------------------------------------------------------------------
void PolygonSetParser::ParseVertices(std::ifstream & file)
{
    // Clear the current vertex data
    m_positions.clear();
    m_normals.clear();
    m_uvSets.clear();

    // Get how many vertices to parse (4 byte unsigned integral value)
    unsigned numVertices = 0;
    file.read(reinterpret_cast<char *>(&numVertices), 4);

    if( !file )
    {
        const std::string msg("Error parsing number of vertices to read");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Get how many UVs each vertex has
    unsigned numUVSets = 0;
    file.read(reinterpret_cast<char *>(&numUVSets), 1);

    if( !file )
    {
        const std::string msg("Error parsing number of UVs to read");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    while(m_uvSets.size() < numUVSets)
    {
        std::vector<TexCoord2D> empty;
        m_uvSets.push_back(empty);
    }

    // Get the data
    for(unsigned i = 0; i < numVertices; ++i)
    {
        // Get the position
        Position position(0.0f, 0.0f, 0.0f);
        file.read(reinterpret_cast<char *>(&position.x), 4);
        file.read(reinterpret_cast<char *>(&position.y), 4);
        file.read(reinterpret_cast<char *>(&position.z), 4);

        if( !file )
        {
            const std::string msg("Error parsing vertex position");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        m_positions.push_back(position);

        // Get the normal
        Normal normal(0.0f, 0.0f, 0.0f);
        file.read(reinterpret_cast<char *>(&normal.x), 4);
        file.read(reinterpret_cast<char *>(&normal.y), 4);
        file.read(reinterpret_cast<char *>(&normal.z), 4);

        if( !file )
        {
            const std::string msg("Error parsing vertex normal");
            throw Common::Exception(__FILE__, __LINE__, msg);
        }

        m_normals.push_back(normal);

        // Get the UV
        for(unsigned j = 0; j < numUVSets; ++j)
        {
            TexCoord2D texCoord(0.0f, 0.0f);
            file.read(reinterpret_cast<char *>(&texCoord.x), 4);
            file.read(reinterpret_cast<char *>(&texCoord.y), 4);
      
            if( !file )
            {
                const std::string msg("Error parsing vertex UV");
                throw Common::Exception(__FILE__, __LINE__, msg);
            }

            m_uvSets[j].push_back(texCoord);
        }
    }
}

//---------------------------------------------------------------------------
const unsigned PolygonSetParser::GetNumPolygonSets() const
{
    return static_cast<unsigned>(m_polygonSets.size());
}

//---------------------------------------------------------------------------
std::auto_ptr<PolygonSet> PolygonSetParser::GetPolygonSet(const unsigned index)
{
    if( index >= m_polygonSets.size() )
    {
        return std::auto_ptr<PolygonSet>(NULL);
    }

    std::auto_ptr<PolygonSet> ptr(m_polygonSets[index]);

    m_polygonSets.erase(m_polygonSets.begin() + index);

    return ptr;
}

//---------------------------------------------------------------------------
void PolygonSetParser::CreatePolygonSet(const bool generateTangentData)
{
    // Create buffers
    std::vector<Buffer::SharedPtr> buffers;
   
    Buffer::SharedPtr bufPositions(new Buffer(m_device, POSITION, m_positions));
    buffers.push_back(bufPositions);

    Buffer::SharedPtr bufNormals(new Buffer(m_device, NORMAL, m_normals));
    buffers.push_back(bufNormals);

    for(std::vector<std::vector<TexCoord2D> >::iterator it = m_uvSets.begin(); it < m_uvSets.end(); ++it)
    {
        Buffer::SharedPtr bufTexCoords(new Buffer(m_device, TEXCOORD2D, (*it)));
        buffers.push_back(bufTexCoords);
    }

    // Create the polygon set
    PolygonSet * polygonSet = new PolygonSet(m_device, 
                                            m_effectManager,                                      
                                            m_inputLayoutManager);

    try
    {
        polygonSet->SetBuffers(buffers, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        polygonSet->SetEffectName(m_effectName, m_techniqueName);
        polygonSet->SetMaterial(*m_material);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    m_polygonSets.push_back(polygonSet);
}

