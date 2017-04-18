

#ifndef POLYGONSET_H
#define POLYGONSET_H

// EngineX Includes
#include "Graphics\3D\Renderable.h"
#include "Graphics\3D\Buffers.h"
#include "Graphics\3D\InputLayoutManager.h"
#include "Graphics\Effects\EffectManager.h"
#include "Graphics\Effects\Effect.h"
#include "Graphics\Effects\Material.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Boost Includes
#include <boost/shared_ptr.hpp>

// Standard Includes
#include <vector>
#include <list>

//----------------------------------------------------------------------------------------------------------------------
/**
* A set of polygons that are typically rendered as one group of triangles sharing the
* same transform, material, and shading technique.
*
* If any additional processing is needed between render passes, the class will need to be derived 
* from and the Render method modified to suit custom needs.
*/
class PolygonSet : public Renderable
{
public:
   
   /**
   * Constructor
   *
   * @param device - 
   * @param effectManager - 
   * @param inputLayoutManager -
   * @param renderType -
   **/
   PolygonSet(ID3D10Device & device,
              EffectManager & effectManager,
              InputLayoutManager & inputLayoutManager,
              const Renderable::RenderType renderType = Renderable::RENDERTYPE_OPAQUE);

   /**
   * Copy Constructor
   *
   * @throws BaseException - if setting of buffers, effect, or material fails
   **/
   PolygonSet(const PolygonSet & rhs);

   /**
   * Deconstructor
   **/
   virtual ~PolygonSet();

   /**
   * Assignement Operator
   *
   * @throws BaseException - if setting of buffers, effect, or material fails
   **/
   PolygonSet & operator = (const PolygonSet & rhs);


   /**
   * Sets the buffers that contain the geometry data
   *
   * @param buffers - 
   * @param topology - 
   *
   * @throws BaseException - if the call failed. The exception will contain a more detailed error message.
   **/
   virtual void SetBuffers(const std::vector<Buffer::SharedPtr> & buffers, const D3D10_PRIMITIVE_TOPOLOGY topology);

   /**
   * Sets the effect to render
   *
   * @param effectName - 
   * @param techniqueName -
   *
   * @throws BaseException - if the effect was not previously created by the effect manager
   **/
   virtual void SetEffectName(const std::string & effectName, const std::string techniqueName);


   /**
   * Renders
   *
   * @throws BaseException - If the buffers, effect, technique, or material were not previously set
   **/
   virtual void Render();

protected:

   /**
   * Recreates per pass info when buffers or the effect change.
   * Also, performs the validation of the buffers against the technique that DirectX requires
   **/
   void CreatePerPassInfo();

   /**
   * Comparator used internally for sorting descriptions of vertex data a pass requires
   **/
   struct CompareSigParamDescs
   {
      bool operator() (const D3D10_SIGNATURE_PARAMETER_DESC & lhs, const D3D10_SIGNATURE_PARAMETER_DESC & rhs);
   };

   std::vector<Buffer::SharedPtr>  m_vertexBuffers;       // All provided vertex buffers
   Buffer::SharedPtr               m_indexBuffer;         // Index buffer, NULL if not indexed
   InputLayoutManager &            m_inputLayoutManager;  // Contains and creates input layouts for shaders
   D3D10_PRIMITIVE_TOPOLOGY        m_primitiveTopology;   // What kind of primitives the vertex buffer hold

   /**
   * Container for data needed to render a single pass
   **/
   struct PassInfo
   {
      PassInfo(Pass * pass,
               const std::vector<unsigned> & vertexBufferIndices,
               const std::vector<unsigned> & offsets,
               const std::vector<unsigned> & strides,
               ID3D10InputLayout * inputLayout,
               const unsigned numVertices);

      PassInfo(const PassInfo & rhs);
      PassInfo & operator = (const PassInfo & rhs);

      // TODO - Pass and InputLayout are candidates for a shared pointer

      Pass *                    m_pass;                   // A single pass from a technique     
      std::vector<unsigned>     m_vertexBufferIndices;    // Which buffers provide the data required for the pass
      std::vector<unsigned>     m_offsets;                // For each vertex buffer, how many elements in to start from
      std::vector<unsigned>     m_strides;                // For each vertex buffer, the size in bytes of one element
      ID3D10InputLayout *       m_inputLayout;            // Input layout used for a specific pass
      unsigned                  m_numVertices;            // Number of vertices to draw this pass
   };

   std::vector<PassInfo>        m_perPassInfo;            // Information needed to render each pass
};

#endif // POLYGONSET_H
