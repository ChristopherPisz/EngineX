
#ifndef POLYGONSETPARSER_H
#define POLYGONSETPARSER_H

// EngineX Includes
#include "Graphics\3D\PolygonSet.h"
#include "Graphics\3D\InputLayoutManager.h"
#include "Graphics\Effects\EffectManager.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>
#include <vector>

//---------------------------------------------------------------------------
// Parses binary files that were exported from 3DS Max using the EngineX export maxscript
//
class PolygonSetParser
{
public:

   /**
   * Constructor
   **/
   PolygonSetParser(ID3D10Device & device, 
                    InputLayoutManager & inputLayoutManager, 
                    TextureManager & textureManager,
                    EffectManager & effectManager);
   
   /**
   * Deconstructor
   **/
   virtual ~PolygonSetParser();


   /**
   * Parses and creates PolygonSet objects from an binary file
   *
   * @param filepath                 - Path to the binary file to parse
   * @param generateTangentData      - If true, will calculate per triangle tangent, bitangent, and normal, in object space 
   *                                      and assign it to each vertex of a triangle
   *
   * @throws - BaseException if there was a parsing error
   **/
   virtual void ParseFile(const std::string & filepath, 
                          const bool generateTangentData = false);

   /**
   * Get the number of PolygonSet objects currently stored
   **/
   virtual const unsigned GetNumPolygonSets() const;

   /**
   * Get a PolygonSet object that was a result of the last parse
   *
   * NOTE - This method results in the obtained PolygonSet object being removed from storage in the parser,
   *        as the caller is now responsible for its lifetime management.
   **/
   virtual std::auto_ptr<PolygonSet> GetPolygonSet(const unsigned index);


protected:

   enum DataID
   {
      MATERIAL_START = 0,
      VERTICES_START,
      POLYGONSET_END
   };

   enum MaterialType
   {
      MAX_STANDARD_MATERIAL  = 0,    // 3DS Max standard material 
      NUM_MATERIAL_TYPES
   };

   enum ShaderType
   {
      MAX_ANISOTROPIC        = 0,    // 3DS Max anisotropic shader
      MAX_BLINN,                     // 3DS Max blinn shader
      MAX_METAL,                     // 3DS Max metal shader
      MAX_MULTILAYER,                // 3DS Max multilayer shader
      MAX_OREN_NAYAR_BLINN,          // 3DS Max oren nayar blinn shader
      MAX_PHONG,                     // 3DS Max phong shader
      MAX_STRAUSS,                   // 3DS Max straus shader
      MAX_TRANSLUCENT,               // 3DS Max translucent shader
      NUM_SHADERTYPES
   };

   /**
   * Parse a material, after a MATERIAL_START data identifier was found
   **/
   virtual void ParseMaterial(std::ifstream & file);

   /**
   * Parse vertices, after a VERTICES_START data identifier was found
   **/
   virtual void ParseVertices(std::ifstream & file);


   /**
   * Create and allocate a PolygonSet object from the parsed data so far
   **/
   virtual void CreatePolygonSet(const bool generateTangentData);


   ID3D10Device &       m_device;
   InputLayoutManager & m_inputLayoutManager;
   TextureManager &     m_textureManager;
   EffectManager &      m_effectManager;


   /**
   *
   **/
   struct MaxStandardMaterialData
   {
	   ShaderType  m_shaderType;
	
	   bool        m_ambientMapped;
	   D3DXCOLOR   m_ambientColor;
      std::string m_ambientMappedFile;
	
	   bool        m_diffuseMapped;
      D3DXCOLOR   m_diffuseColor;
      std::string m_diffuseMappedFile;
   };

   std::vector<MaxStandardMaterialData> m_maxStandardMaterialDatas;

   std::string                           m_effectName;
   std::string                           m_techniqueName;
   std::auto_ptr<Material>               m_material;
   std::vector<Position>                 m_positions;
   std::vector<Normal>                   m_normals;
   std::vector<std::vector<TexCoord2D> > m_uvSets;

   /**
   * PolygonSets that were a result of the file
   **/
   std::vector<PolygonSet *> m_polygonSets;
};




#endif // POLYGONSETPARSER_H