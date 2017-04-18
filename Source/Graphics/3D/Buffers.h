
#ifndef BUFFERS_H
#define BUFFERS_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>
#include <map>
#include <memory>
#include <vector>

//------------------------------------------------------------------------------------------
/**
* C++ types for representation of D3D buffered data types                 HLSL type
**/
typedef D3DXVECTOR3  BiTangent;        // BiTangent                        float3 
typedef unsigned int BlendIndex;       // Blend indices                    uint
typedef float        BlendWeight;      // Blend weights                    float 
typedef D3DXCOLOR    Color;            // Diffuse and specular color       float4 
typedef D3DXVECTOR3  Normal;           // Normal vector                    float3 
typedef D3DXVECTOR3  Position;         // Vertex position in object space  float3
typedef D3DXVECTOR4  PositionT;        // Transformed vertex position      float4
typedef float        PointSize;        // Point size                       float
typedef D3DXVECTOR3  Tangent;          // Tangent                          float3 
typedef float        TexCoord1D;       // Texture coordinates              float
typedef D3DXVECTOR2  TexCoord2D;       // Texture coordinates              float2
typedef D3DXVECTOR3  TexCoord3D;       // Texture coordinates              float3

typedef unsigned int Index;            // Indices into vertex buffers      uint

typedef D3DXMATRIX   Matrix;           // Transformation matrix            float4x4

//------------------------------------------------------------------------------------------
/**
* Identifies a type of D3D buffered data
*
* Because some D3D buffered data types map to the same C++ data types, this identifier is 
* necessary to tell them apart.
**/
enum BufferContentType
{
   // Correspond to built in semantics
   BITANGENT       = 0,
   BLENDINDEX,
   BLENDWEIGHT,
   COLOR,
   NORMAL,
   POSITION,
   POSITIONT,
   POINTSIZE,
   TANGENT,
   TEXCOORD1D,
   TEXCOORD2D,
   TEXCOORD3D,
   INDEX,

   // Correspond to custom semantics
   TRANSFORM,

   NUM_BUFFER_CONTENT_TYPES
};

//------------------------------------------------------------------------------------------
/**
* Gets the semantic name used for the different buffer content types
**/
const std::string ContentTypeToSemanticName(const BufferContentType & contentType);

/**
* Gets the data format for the different buffered content types 
**/
const DXGI_FORMAT GetFormat(const BufferContentType & contentType);

/**
* Gets the number of bytes a piece of data, represented in the given DXGI format, takes up
**/
const unsigned GetFormatSize(const DXGI_FORMAT & format);

/**
* Gets the stride for the different buffered content types
* The stride is simply the sizeof the C++ data type representing the content type
**/
const unsigned GetStride(const BufferContentType & contentType);

//------------------------------------------------------------------------------------------
/**
* Wrapper for a D3D buffer
**/
class Buffer
{
public:

   typedef std::shared_ptr<Buffer> SharedPtr;
   typedef std::weak_ptr<Buffer>   WeakPtr;

   /**
   * Creates a buffer for the following types:
   *    BlendIndex
   *    Index
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<unsigned int> & data,
          const bool dynamic = false,
          const bool perInstance = false);

   /**
   * Creates a buffer for the following types:
   *    BlendWeight
   *    PointSize
   *    TexCoord1D
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<float> & data,
          const bool dynamic   = false,
          const bool perInstance  = false);

   /**
   * Creates a buffer for the following types:
   *    TexCoord2D
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<D3DXVECTOR2> & data,
          const bool dynamic   = false,
          const bool perInstance  = false);

   /**
   * Creates a buffer for the following types:
   *    BiTangent
   *    Normal
   *    Position
   *    Tangent
   *    TexCoord3D
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<D3DXVECTOR3> & data,
          const bool dynamic = false,
          const bool perInstance = false);

   /**
   * Creates a buffer for the following types:
   *    PositionT
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<D3DXVECTOR4> & data,
          const bool dynamic = false,
          const bool perInstance = false);

   /**
   * Creates a buffer for the following types:
   *    Color
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<D3DXCOLOR> & data,
          const bool dynamic = false,
          const bool perInstance = false);

   /**
   * Creates a buffer for the following types:
   *    Transform
   **/
   Buffer(ID3D10Device & device,
          const BufferContentType contentType,
          const std::vector<D3DXMATRIX> & data,
          const bool dynamic = false,
          const bool perInstance = false);

   /**
   * Destructor
   **/
   ~Buffer();


   /**
   * Get the content type this buffer contains
   **/
   const BufferContentType GetContentType() const;

   /**
   * Query whether the buffer contains dynamic data or static data
   **/
   const bool IsDynamic() const;

   /**
   * Query whether the buffer contains per instance data or per vertex data
   **/
   const bool IsPerInstance() const;

   /**
   * Get the number of elements in the buffer
   **/
   const unsigned GetNumElements() const;

   /**
   * Get the D3D buffer
   **/
   ID3D10Buffer * GetD3DBuffer() const;

private:
   
   /** No Copy allowed */
   Buffer(const Buffer & rhs);

   /** No assignment allowed */
   Buffer & operator = (const Buffer & rhs);



   ID3D10Device &          m_device;        // D3D device used to create the buffer
   ID3D10Buffer *          m_buffer;        // D3D buffer interface

   const BufferContentType m_contentType;   // Identifies the type of data the buffer contains 
   const bool              m_dynamic;       // Whether or not writing to the buffer after creation is permitted
   const bool              m_perInstance;   // Whether or not this buffer contains per instance data or per vertex data

   unsigned                m_numElements;   // Number of elements the buffer contains
};

#endif // BUFFERS_H
