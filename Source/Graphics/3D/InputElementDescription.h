
#ifndef INPUTELEMENTDESCRIPTION_H
#define INPUTELEMENTDESCRIPTION_H

// EngineX Includes
#include "Graphics\3D\Buffers.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <string>


//------------------------------------------------------------------------------------------
/**
* C++ Wrapper for a D3D Input Element Description
*
* The original structure did not control the lifetime of the semantic name.
* Simply cast to a D3D10_INPUT_ELEMENT_DESCRIPTION when passing to the D3D API.
**/
class InputElementDescription : public D3D10_INPUT_ELEMENT_DESC
{
public:
   
   /**
   *
   **/
   InputElementDescription(const std::string & semanticName,
                           const unsigned semanticIndex,
                           const DXGI_FORMAT format,
                           const unsigned inputSlot,
                           const unsigned alignedByteOffset,
                           const D3D10_INPUT_CLASSIFICATION inputSlotClass,
                           const unsigned instanceDataStepRate);

   /**
   *
   **/
   InputElementDescription(const InputElementDescription & rhs);

   /**
   *
   **/
   ~InputElementDescription();
   
   /**
   *
   **/
   const InputElementDescription & operator =(const InputElementDescription & rhs);


   /**
   * Gets the semantic name as a string
   **/
   const std::string & GetSemanticName() const;

   /**
   *
   **/
   static void GetInputElementDesc(std::vector<InputElementDescription> & inputElementDescs, 
                                   const BufferContentType contentType, 
                                   const unsigned startIndex,
                                   const unsigned inputSlot,
                                   const bool perInstance = false);

private:

   std::string m_semanticName;
};

#endif // INPUTELEMENTDESCRIPTION_H
