

#include "InputLayoutManager.h"


//----------------------------------------------------------------------------
InputLayoutManager::InputLayoutManager(ID3D10Device & device)
   :
   m_device(device)
{
}

//----------------------------------------------------------------------------
InputLayoutManager::~InputLayoutManager()
{
   for(InputLayoutMap::iterator it = m_inputLayouts.begin(); it != m_inputLayouts.end(); ++it)
   {
      it->second->Release();
   }
}

//----------------------------------------------------------------------------
ID3D10InputLayout * InputLayoutManager::GetInputLayout(const std::vector<InputElementDescription> inputElementDescs,
                                                       const Pass & pass)
{
   // Check if the input layout already exists
   if( !m_inputLayouts.empty() )
   {
      InputLayoutMap::iterator it = m_inputLayouts.find(inputElementDescs);

      if( it != m_inputLayouts.end() )
      {
         return it->second;
      }
   }

   // Create a new input layout
   D3D10_PASS_DESC passDesc        = pass.GetPassDesc();
   ID3D10InputLayout * inputLayout = NULL;
   
   std::vector<D3D10_INPUT_ELEMENT_DESC> d3dInputElementDescs;
   for(std::vector<InputElementDescription>::const_iterator it = inputElementDescs.begin(); it != inputElementDescs.end(); ++it)
   {
      d3dInputElementDescs.push_back(static_cast<D3D10_INPUT_ELEMENT_DESC>(*it));
   }

   m_device.CreateInputLayout(&d3dInputElementDescs[0], 
                              static_cast<UINT>(inputElementDescs.size()), 
                              passDesc.pIAInputSignature,
                              passDesc.IAInputSignatureSize, 
                              &inputLayout);

   // Store the input layout
   m_inputLayouts[inputElementDescs] = inputLayout;

   return inputLayout;
}

//----------------------------------------------------------------------------
bool InputLayoutManager::CompareKeys::operator() (const std::vector<InputElementDescription> & lhs,
                                                  const std::vector<InputElementDescription> & rhs) const
{
   // This is not really a "less than" operation. 
   // We are simply defining a way to order keys that meet the criteria of a std::map

   if( lhs.size() < rhs.size() )
   {
      return true;
   }
   else if( lhs.size() > rhs.size() )
   {
      return false;
   }

   for(unsigned i = 0; i < lhs.size(); ++i)
   {
      if( lhs[i].GetSemanticName() < rhs[i].GetSemanticName() )
      {
         return true;
      }
      else if( lhs[i].GetSemanticName() > rhs[i].GetSemanticName() )
      {
         return false;
      }

      if( lhs[i].SemanticIndex < rhs[i].SemanticIndex )
      {
         return true;
      }
      else if( lhs[i].SemanticIndex > rhs[i].SemanticIndex )
      {
         return false;
      }

      if( lhs[i].Format < rhs[i].Format )
      {
         return true;
      }
      else if( lhs[i].Format > rhs[i].Format )
      {
         return false;
      }

      if( lhs[i].InputSlot < rhs[i].InputSlot )
      {
         return true;
      }
      else if( lhs[i].InputSlot > rhs[i].InputSlot )
      {
         return false;
      }

      if( lhs[i].AlignedByteOffset < rhs[i].AlignedByteOffset )
      {
         return true;
      }
      else if( lhs[i].AlignedByteOffset > rhs[i].AlignedByteOffset )
      {
         return false;
      }

      if( lhs[i].InputSlotClass < rhs[i].InputSlotClass )
      {
         return true;
      }
      else if( lhs[i].InputSlotClass > rhs[i].InputSlotClass )
      {
         return false;
      }

      if( lhs[i].InstanceDataStepRate < rhs[i].InstanceDataStepRate )
      {
         return true;
      }
      else if( lhs[i].InstanceDataStepRate > rhs[i].InstanceDataStepRate )
      {
         return false;
      }
   }

   return false;
}