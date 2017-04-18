

// Project Includes
#include "Buffers.h"

// Common Lib Includes
#include "Exception.h"

//---------------------------------------------------------------------------
const std::string ContentTypeToSemanticName(const BufferContentType & contentType)
{
    if( contentType == BITANGENT )
    {
        return "BITANGENT";
    }
    else if( contentType == BLENDINDEX )
    {
        return "BLENDINDICES";
    }
    else if( contentType == BLENDWEIGHT )
    {
        return "BLENDWEIGHT";
    }
    else if( contentType == COLOR )
    {
        return "COLOR";
    }
    else if( contentType == NORMAL )
    {
        return "NORMAL";
    }
    else if( contentType == POSITION )
    {
        return "POSITION";
    }
    else if( contentType == POSITIONT )
    {
        return "POSITIONT";
    }
    else if( contentType == POINTSIZE )
    {
        return "PSIZE";
    }
    else if( contentType == TANGENT )
    {
        return "TANGENT";
    }
    else if( contentType == TEXCOORD1D ||
            contentType == TEXCOORD2D ||
            contentType == TEXCOORD3D )
    {
        return "TEXCOORD";
    }
    else if( contentType == TRANSFORM )
    {
        return "TRANSFORM";
    }
    else
    {
        const std::string msg("Semantic for buffered content type not found");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
const DXGI_FORMAT GetFormat(const BufferContentType & contentType)
{
    // 1 unsigned int
    if( contentType == BLENDINDEX || 
        contentType == INDEX )
    {
        return DXGI_FORMAT_R32_UINT;
    }
   
    // 1 float
    else if( contentType == BLENDWEIGHT ||
            contentType == POINTSIZE   ||
            contentType == TEXCOORD1D )
    {
        return DXGI_FORMAT_R32_FLOAT;
    }
   
    // 2 floats
    else if( contentType == TEXCOORD2D )
    {
        return DXGI_FORMAT_R32G32_FLOAT;
    }

    // 3 floats
    else if( contentType == BITANGENT ||
            contentType == NORMAL    ||
            contentType == POSITION  ||
            contentType == TANGENT   ||
            contentType == TEXCOORD3D )
    {
        return DXGI_FORMAT_R32G32B32_FLOAT;
    }

    // 4 floats
    else if( contentType == COLOR     || 
            contentType == POSITIONT ||
            contentType == TRANSFORM )       // Transforms are read 4 floats at a time
    {
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    // Unknown
    else
    {
        const std::string msg("Format for buffered cotent type not found");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
const unsigned GetFormatSize(const DXGI_FORMAT & format)
{
    if( format == DXGI_FORMAT_R32G32B32A32_FLOAT ||
        format == DXGI_FORMAT_R32G32B32A32_UINT  ||
        format == DXGI_FORMAT_R32G32B32A32_SINT )
    {
        return 16;
    }

    if( format == DXGI_FORMAT_R32G32B32_FLOAT ||
        format == DXGI_FORMAT_R32G32B32_UINT  ||
        format == DXGI_FORMAT_R32G32B32_SINT )
    {
        return 12;
    }

    if( format == DXGI_FORMAT_R32G32_FLOAT ||
        format == DXGI_FORMAT_R32G32_UINT  ||
        format == DXGI_FORMAT_R32G32_SINT )
    {
        return 8;
    }

    if( format == DXGI_FORMAT_R32_FLOAT ||
        format == DXGI_FORMAT_R32_UINT  ||
        format == DXGI_FORMAT_R32_SINT )
    {
        return 4;
    }

    // Unknown
    const std::string msg("Format size unknown");
    throw Common::Exception(__FILE__, __LINE__, msg);
}

//---------------------------------------------------------------------------
const unsigned GetStride(const BufferContentType & contentType)
{
    // 1 unsigned int
    if( contentType == BLENDINDEX || 
        contentType == INDEX )
    {
        return sizeof(BlendIndex);
    }
   
    // 1 float
    else if( contentType == BLENDWEIGHT ||
            contentType == POINTSIZE   ||
            contentType == TEXCOORD1D )
    {
        return sizeof(BlendWeight);
    }
   
    // 2 floats
    else if( contentType == TEXCOORD2D )
    {
        return sizeof(TexCoord2D);
    }

    // 3 floats
    else if( contentType == BITANGENT ||
            contentType == NORMAL    ||
            contentType == POSITION  ||
            contentType == TANGENT   ||
            contentType == TEXCOORD3D )
    {
        return sizeof(BiTangent);
    }

    // 4 floats
    else if( contentType == COLOR || 
            contentType == POSITIONT )
    {
        return sizeof(Color);
    }
   
    // 4x4 floats
    else if( contentType == TRANSFORM )
    {
        return sizeof(D3DXMATRIX);
    }

    // Unknown
    else
    {
        const std::string msg("Stride for buffered content type not found");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<unsigned int> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
{
    // Check for sillyness
    if( sizeof(unsigned int) != GetStride(contentType) ||
        DXGI_FORMAT_R32_UINT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    if( contentType == INDEX && perInstance )
    {
        const std::string msg("An index buffer cannot be 'per instance'");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    if( contentType == INDEX )
    {
        bufferDesc.BindFlags      = D3D10_BIND_INDEX_BUFFER;   
    }
    else
    {
        bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    }

    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(unsigned int) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];
   
    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<float> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
{
    // Check for sillyness
    if( sizeof(float) != GetStride(contentType) ||
        DXGI_FORMAT_R32_FLOAT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(float) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];
   
    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<D3DXVECTOR2> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
{
    // Check for sillyness
    if( sizeof(D3DXVECTOR2) != GetStride(contentType)      ||
        DXGI_FORMAT_R32G32_FLOAT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(D3DXVECTOR2) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];
   
    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<D3DXVECTOR3> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
{
    // Check for sillyness
    if( sizeof(D3DXVECTOR3) != GetStride(contentType)         ||
        DXGI_FORMAT_R32G32B32_FLOAT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(D3DXVECTOR3) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];
   
    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<D3DXVECTOR4> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
{
    // Check for sillyness
    if( sizeof(D3DXVECTOR4) != GetStride(contentType)            ||
        DXGI_FORMAT_R32G32B32A32_FLOAT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(D3DXVECTOR4) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];

    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<D3DXCOLOR> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
{   
    // Check for sillyness
    if( sizeof(D3DXCOLOR) != GetStride(contentType)              ||
        DXGI_FORMAT_R32G32B32A32_FLOAT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(D3DXCOLOR) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];
   
    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::Buffer(ID3D10Device & device,
               const BufferContentType contentType,
               const std::vector<D3DXMATRIX> & data,
               const bool dynamic,
               const bool perInstance)
    :
    m_device(device),
    m_buffer(NULL),
    m_contentType(contentType),
    m_dynamic(dynamic),
    m_perInstance(perInstance),
    m_numElements(static_cast<unsigned>(data.size()))
    {
    // Check for sillyness
    if( sizeof(D3DXMATRIX) != GetStride(contentType)              ||
        DXGI_FORMAT_R32G32B32A32_FLOAT != GetFormat(contentType) )
    {
        const std::string msg("Illegal content type for this constructor");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Describe the vertex buffer
    D3D10_BUFFER_DESC bufferDesc;

    if( !dynamic )
    {
        bufferDesc.Usage          = D3D10_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = 0;
    }
    else
    {
        bufferDesc.Usage          = D3D10_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
    }

    bufferDesc.BindFlags      = D3D10_BIND_VERTEX_BUFFER;
    bufferDesc.ByteWidth      = static_cast<UINT>(sizeof(D3DXMATRIX) * data.size());
    bufferDesc.MiscFlags      = 0;
   
    // Provide the data for the buffer
    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &data[0];
   
    // Create the buffer with the provided data
    if( FAILED(m_device.CreateBuffer(&bufferDesc, &initData, &m_buffer)) )
    {
        const std::string msg("Failed to create buffer");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}

//---------------------------------------------------------------------------
Buffer::~Buffer()
{
    if( m_buffer )
    {
        m_buffer->Release();
    }
}

//---------------------------------------------------------------------------
const BufferContentType Buffer::GetContentType() const
{
   return m_contentType;
}

//---------------------------------------------------------------------------
const bool Buffer::IsDynamic() const
{
    return m_dynamic;
}

//---------------------------------------------------------------------------
const bool Buffer::IsPerInstance() const
{
    return m_perInstance;
}

//---------------------------------------------------------------------------
const unsigned Buffer::GetNumElements() const
{
    return m_numElements;
}

//---------------------------------------------------------------------------
ID3D10Buffer * Buffer::GetD3DBuffer() const
{
    return m_buffer;
}

