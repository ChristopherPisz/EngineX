

#include "InputElementDescription.h"



//------------------------------------------------------------------------------------------
InputElementDescription::InputElementDescription(const std::string & semanticName,
                                                 const unsigned semanticIndex,
                                                 const DXGI_FORMAT format,
                                                 const unsigned inputSlot,
                                                 const unsigned alignedByteOffset,
                                                 const D3D10_INPUT_CLASSIFICATION inputSlotClass,
                                                 const unsigned instanceDataStepRate)
   :
   D3D10_INPUT_ELEMENT_DESC(),
   m_semanticName(semanticName)
{
   SemanticName         = m_semanticName.c_str();
   SemanticIndex        = semanticIndex;
   Format               = format;
   InputSlot            = inputSlot;
   AlignedByteOffset    = alignedByteOffset;
   InputSlotClass       = inputSlotClass;
   InstanceDataStepRate = instanceDataStepRate;
}

//------------------------------------------------------------------------------------------
InputElementDescription::InputElementDescription(const InputElementDescription & rhs)
   :
   D3D10_INPUT_ELEMENT_DESC(),
   m_semanticName(rhs.m_semanticName)
{
   SemanticName         = m_semanticName.c_str();
   SemanticIndex        = rhs.SemanticIndex;
   Format               = rhs.Format;
   InputSlot            = rhs.InputSlot;
   AlignedByteOffset    = rhs.AlignedByteOffset;
   InputSlotClass       = rhs.InputSlotClass;
   InstanceDataStepRate = rhs.InstanceDataStepRate;
}

//------------------------------------------------------------------------------------------
InputElementDescription::~InputElementDescription()
{
}

//------------------------------------------------------------------------------------------
const InputElementDescription & InputElementDescription::operator =(const InputElementDescription & rhs)
{
   if( &rhs == this )
   {
      return rhs;
   }

   m_semanticName       = rhs.m_semanticName;

   SemanticName         = m_semanticName.c_str();
   SemanticIndex        = rhs.SemanticIndex;
   Format               = rhs.Format;
   InputSlot            = rhs.InputSlot;
   AlignedByteOffset    = rhs.AlignedByteOffset;
   InputSlotClass       = rhs.InputSlotClass;
   InstanceDataStepRate = rhs.InstanceDataStepRate;

   return *this;
}

//------------------------------------------------------------------------------------------
const std::string & InputElementDescription::GetSemanticName() const
{
   return m_semanticName;
}

//------------------------------------------------------------------------------------------
void InputElementDescription::GetInputElementDesc(std::vector<InputElementDescription> & inputElementDescs, 
                                                  const BufferContentType contentType, 
                                                  const unsigned startIndex,
                                                  const unsigned inputSlot,
                                                  const bool perInstance)
{
   // Start with an empty vector
   inputElementDescs.clear();

   // All vertex buffers will be bound to one input slot each.
   //
   // A vertex buffer containing a component that fits into 16 bytes (4 floats or 4 ints) or less will have one 
   // input element description.
   // 
   // For those buffers with content types that are larger than 16 bytes: 
   //
   // For every additional 16 bytes a content type takes up, an additional input element description will have to be generated.
   // D3D treats the next 16 bytes as an additional semantic index, as if it was a seperate piece of data then the first 16 bytes.
   // However, the entire component element will be assigned to the desired vertex shader input.
   //
   // See the D3D10 Instancing Sample for reference on how a vertex buffer contains a 4x4 float matrix and its corresponding
   // input element description.

   D3D10_INPUT_CLASSIFICATION inputSlotClass        = D3D10_INPUT_PER_VERTEX_DATA;
   unsigned                   instanceDataStepRate  = 0;

   if( perInstance )
   {
      inputSlotClass       = D3D10_INPUT_PER_INSTANCE_DATA;
      instanceDataStepRate = 1;
   }

   DXGI_FORMAT format       = GetFormat(contentType);
   unsigned bytesPerElement = GetFormatSize(format);
   unsigned dataSize        = GetStride(contentType);

   for(unsigned i = 0; i * bytesPerElement < dataSize; ++i)
   {
      InputElementDescription inputElementDesc(ContentTypeToSemanticName(contentType),
                                               startIndex + i,
                                               GetFormat(contentType),
                                               inputSlot,
                                               i * bytesPerElement,
                                               inputSlotClass,
                                               instanceDataStepRate);

      inputElementDescs.push_back(inputElementDesc);
   }
}