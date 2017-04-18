
#include "PolygonSet.h"


// Common Lib Includes
#include "BaseException.h"

// Standard Includes
#include <algorithm>
#include <sstream>

using namespace common_lib_cpisz;


//----------------------------------------------------------------------------------------------------------------------
PolygonSet::PolygonSet(ID3D10Device & device,
                       EffectManager & effectManager,
                       InputLayoutManager & inputLayoutManager,
                       const Renderable::RenderType renderType)
   :
   Renderable(device, effectManager, renderType),   
   m_inputLayoutManager(inputLayoutManager)
{
}

//----------------------------------------------------------------------------------------------------------------------
PolygonSet::PolygonSet(const PolygonSet & rhs)
   :
   Renderable(rhs),
   m_inputLayoutManager(rhs.m_inputLayoutManager)
{
   std::vector<Buffer::SharedPtr> buffers(rhs.m_vertexBuffers);
   buffers.push_back(rhs.m_indexBuffer);

   try
   {
      // Copy the vertex and index buffers
      //
      // NOTE - Probably not the most efficient to recreate the per pass info again, but simpler.
      //        Consider copying directly later.
      SetBuffers(buffers, rhs.m_primitiveTopology);

      // Copy the effect
      SetEffectName(rhs.m_effectName, rhs.m_techniqueName);

      // Copy the material
      SetMaterial(rhs.GetMaterial());
   }
   catch(BaseException & e)
   {
      throw e;
   }
}


//---------------------------------------------------------------------------
PolygonSet::~PolygonSet()
{
}

//---------------------------------------------------------------------------
PolygonSet & PolygonSet::operator = (const PolygonSet & rhs)
{
   BaseException exception("Not Set", 
                           "PolygonSet & PolygonSet::operator = (const PolygonSet & rhs)",
                           "PolygonSet.cpp");

   if( this == &rhs )
   {
      return *this;
   }

   std::vector<Buffer::SharedPtr> buffers;
   std::copy(rhs.m_vertexBuffers.begin(), rhs.m_vertexBuffers.end(), buffers.begin());
   buffers.push_back(rhs.m_indexBuffer);

   try
   {
      this->Renderable::operator = (rhs);

      // Copy the vertex and index buffers
      //
      // NOTE - Probably not the most efficient to recreate the per pass info again, but simpler.
      //        Consider copying directly later.
      SetBuffers(buffers, rhs.m_primitiveTopology);

      // Copy the effect
      SetEffectName(rhs.m_effectName, rhs.m_techniqueName);

      // Copy the material
      SetMaterial(rhs.GetMaterial());
   }
   catch(BaseException & e)
   {
      throw e;
   }

   return *this;
}

//---------------------------------------------------------------------------
void PolygonSet::SetBuffers(const std::vector<Buffer::SharedPtr> & buffers, const D3D10_PRIMITIVE_TOPOLOGY topology)
{
   BaseException exception("Not Set", 
                           "void PolygonSet::SetBuffers(const std::vector<Buffer::SharedPtr> & buffers, const D3D10_PRIMITIVE_TOPOLOGY topology)",
                           "PolygonSet.cpp");

   // Release all the current buffers
   m_vertexBuffers.clear();
   m_indexBuffer.reset();

   // Store the new buffers
   for( std::vector<Buffer::SharedPtr>::const_iterator it = buffers.begin(); it != buffers.end(); ++it)
   {
      // At render time, the PolygonSet can be drawn using indices or with a regular draw call
      // If an index buffer is provided then we know to draw this PolygonSet using indices
      // A check will be made at render time to see if an index buffer has been set or not
      //
      // Seperate the vertex buffers from the index buffer, if an index buffer was provided
      if( (*it)->GetContentType() == INDEX )
      {
         if( m_indexBuffer )
         {
            exception.m_msg = "Only one index buffer is permitted";
            throw exception;
         }

         m_indexBuffer = (*it);
         continue;
      }

      // Currently, per instance data is not supported using this class
      if( (*it)->IsPerInstance() )
      {
         exception.m_msg = "Currently, per instance data is not supported using this class";
         throw exception;
      }

      m_vertexBuffers.push_back(*it);
   }

   // Check that we have at least one vertex buffer
   if( m_vertexBuffers.empty() )
   {
      exception.m_msg = "No vertex buffer was provided"; 
      throw exception;
   }

   // Check that all vertex buffers have the same number of elements
   unsigned numVertices = m_vertexBuffers[0]->GetNumElements();

   for(std::vector<Buffer::SharedPtr>::const_iterator it = m_vertexBuffers.begin(); it != m_vertexBuffers.end(); ++it)
   {
      if( (*it)->GetNumElements() != numVertices )
      {
         exception.m_msg = "Not all vertex buffers, containing per vertex data, contain the same number of elements"; 
         throw exception;
      }
   }

   // Validate the buffers against the technique, if a technique is set
   try
   {
      CreatePerPassInfo();
   }
   catch(BaseException & e)
   {
      throw e;
   }

   // Store the primitive topology
   m_primitiveTopology = topology;
}

//---------------------------------------------------------------------------
void PolygonSet::SetEffectName(const std::string & effectName, const std::string techniqueName)
{
   try
   {
      Renderable::SetEffectName(effectName, techniqueName);

      // Validate the technique against the buffers, if the buffers are set
      CreatePerPassInfo();
   }
   catch(BaseException & e)
   {
      throw e;
   }
}

//---------------------------------------------------------------------------
void PolygonSet::Render()
{
   BaseException exception("Not Set", 
                           "void PolygonSet::Render()",
                           "PolygonSet.cpp");

   // Check if the vertex buffers were validated against the technique
   if( m_perPassInfo.empty() )
   {
      // Error, someone set the effect without a matching technique, or vica versa,
      // or one of them did not get set.
      exception.m_msg  = "The vertex buffers have not been validated against a technique.\n";
      exception.m_msg += "Make sure both the buffers and the technique have been set and\n";
      exception.m_msg += "that the buffers provide all the data required by the technique.";
      throw exception;
   }

   // Check that a material is set
   if( !m_material.get() )
   {
      exception.m_msg = "No material has been set for this object";
      throw exception;
   }

   // TODO - Nothing needs to be bound if we are rendering the same geometry with the same effect
   //        This would be a possible optimization if tracking isn't as costly

   // Set the effect variables
   try
   {
      Effect & effect = m_effectManager.GetChildEffect(m_effectName);
      effect.SetWorldMatrix(GetTransform());
      effect.SetMaterial(*m_material);
   }
   catch(BaseException & e)
   {
      throw e;
   }

   // Tell the input assembler how to assemble the vertices into primitives
   m_device.IASetPrimitiveTopology(m_primitiveTopology);

   // Render each pass
   for(std::vector<PassInfo>::iterator itPassInfo = m_perPassInfo.begin(); itPassInfo != m_perPassInfo.end(); ++itPassInfo)
   {
      // Bind the input layout
      m_device.IASetInputLayout(itPassInfo->m_inputLayout);

      // Bind the vertex buffers the technique requires
      std::vector<ID3D10Buffer *> vertexBuffers;
      
      for(std::vector<unsigned>::iterator itIndex = itPassInfo->m_vertexBufferIndices.begin();
          itIndex != itPassInfo->m_vertexBufferIndices.end(); ++ itIndex)
      {
         vertexBuffers.push_back( m_vertexBuffers[*itIndex]->GetD3DBuffer() );
      }

      m_device.IASetVertexBuffers(0, 
                                  static_cast<UINT>(itPassInfo->m_vertexBufferIndices.size()),
                                  &vertexBuffers[0], 
                                  &(itPassInfo->m_strides[0]), 
                                  &(itPassInfo->m_offsets[0]));

      // Check if we are drawing using an index buffer
      if( m_indexBuffer )
      {
         // Bind the index buffer
         m_device.IASetIndexBuffer(m_indexBuffer->GetD3DBuffer(), GetFormat(INDEX), 0);

         // Apply the pass
         itPassInfo->m_pass->Apply();

         // Draw
         unsigned numIndices = m_indexBuffer->GetNumElements();
         m_device.DrawIndexed(numIndices, 0, 0);
      }
      else
      {
         // Apply the pass
         itPassInfo->m_pass->Apply();

         // Draw
         m_device.Draw(itPassInfo->m_numVertices, 0);
      }
   }
}

//---------------------------------------------------------------------------
void PolygonSet::CreatePerPassInfo()
{
   BaseException exception("Not Set", 
                           "void PolygonSet::CreatePerPassInfo()",
                           "PolygonSet.cpp");


   // Clear the per pass info
   //
   // This signals that the buffers have not been validated against the technique.
   // Allowing the client to make a call to change the buffers with the intention 
   // of setting the effect afterward, when an old effect is still set. The reverse
   // scenario is possible too. The Render method will need to check if this is empty.
   m_perPassInfo.clear();

   // If there are not vertex buffers or there is not technique set, we cannot validate
   // Validation will happen when all of the above are set
   if( m_vertexBuffers.empty() || 
       m_effectName.empty()    ||
       m_techniqueName.empty() )
   {
      return;
   }
   
   // Validate the buffers against the technique
   Technique * technique = NULL;

   try
   {
      Effect & effect = m_effectManager.GetChildEffect(m_effectName);
      technique       = &(effect.GetTechnique(m_techniqueName));
   }
   catch(BaseException & e)
   {
      throw e;
   }

   // Go through each pass of the technique
   for(unsigned passIndex = 0; passIndex < technique->GetNumPasses(); ++passIndex)
   {
      Pass * pass = NULL;                         // The pass we are verifying
      std::vector<unsigned> vertexBufferIndices;  // Indices to vertex buffers that fulfill requirements of this pass
      
      try
      {
         pass = &(technique->GetPass(passIndex)); 
      }
      catch(BaseException & e)
      {
         throw e;
      }

      // Make a copy of the requirements, describing the vertex data a technique expects. 
      // A requirement will be removed when it is matched with a vertex buffer
      std::vector<D3D10_SIGNATURE_PARAMETER_DESC> requirements;
      requirements = pass->GetSignatureParamDescs();

      // Make a copy of the available buffer indices
      // An index will be removed when it is matched with a requirement
      std::vector<unsigned> availableIndices;

      for(unsigned i = 0; i < m_vertexBuffers.size(); ++i)
      {
         availableIndices.push_back(i);
      }

      // Keeps track of which slot the provided buffer will take
      // I simply increment this for every buffer I provide
      unsigned inputSlot = 0;
      
      // This map will keep track of the last semantic index of each provided buffer
      // This way I can keep track of what semantic index is being provided and if it matches
      // the requested semantic index.
      //
      // key- semantic name
      // value - number of times that semantic has already been provided
      std::map<std::string, unsigned> providedSemantics;

      // This vector is used to build the input layout
      // It holds descriptions of what each buffer contains, as we provide each buffer
      std::vector<InputElementDescription> inputElementDescs;

      // Go though the requirements and look for a match from the provided buffers
      while( !requirements.empty() )
      {
         // Get the first requirement
         std::string semanticName  = requirements.front().SemanticName;
         unsigned    semanticIndex = requirements.front().SemanticIndex; 

         // Check that the requested semantic index matches what we have provided so far
         if( semanticIndex )
         {
            std::map<std::string, unsigned>::iterator it = providedSemantics.end();
            
            if( !providedSemantics.empty() )
            {
               it = providedSemantics.find(semanticName);
            }

            if( it == providedSemantics.end() || 
                it->second != semanticIndex - 1 )
            {
               exception.m_msg  = "A semantic has been requested by the technique with an index that does not ";
               exception.m_msg += "match how many times the semantic has been provided already.";
               exception.m_msg += "Check vertex buffer inputs to effect: " + m_effectName + " ";
               exception.m_msg += "technique: " + m_techniqueName + ", and that vertex buffers were provided by ";
               exception.m_msg += "the application in the correct order.";  // phew what an error message!
            }
         }

         // Search for a provided buffer with a matching semantic name
         int  index;
         bool found = false;

         for(std::vector<unsigned>::iterator itAvailableIndex = availableIndices.begin();
             itAvailableIndex != availableIndices.end(); ++itAvailableIndex)
         {
            index = (*itAvailableIndex);

            // Check if the content type the buffer offers matches the semantic name we are looking for
            if( ContentTypeToSemanticName(m_vertexBuffers[index]->GetContentType()) == semanticName )
            {
               // Found a match
               found = true;

               // Remove the index from available indices for future searched
               availableIndices.erase(itAvailableIndex);

               break;
            }
         }
 
         if( !found )
         {
            // Cannot provide the vertex buffer type required by the technique
            std::stringstream msg;
            msg << "The provided vertexs buffers do not contain a vertex buffer type required "
                << "by the chosen technique."
                << "\nSemanticName: " << semanticName;

            exception.m_msg = msg.str(); 
            throw exception;
         }

         // We have a match
         
         // Store the index to the vertex buffer fulfilling the requirement         
         vertexBufferIndices.push_back(index);

         // Create an input element description
         std::vector<InputElementDescription> thisElementDesc;

         InputElementDescription::GetInputElementDesc(thisElementDesc, 
                                                      m_vertexBuffers[index]->GetContentType(), 
                                                      semanticIndex,
                                                      inputSlot,
                                                      m_vertexBuffers[index]->IsPerInstance());

         // Store the input element description
         inputElementDescs.insert(inputElementDescs.end(), thisElementDesc.begin(), thisElementDesc.end());

         // Remove requirement
         unsigned numRequirementsToRemove = static_cast<unsigned>(thisElementDesc.size());

         requirements.erase(requirements.begin(), requirements.begin() + numRequirementsToRemove);

         // Increment the input slot for the next vertex buffer
         ++inputSlot;

         // Increment the semantic index for vertex buffers with the same semantic
         providedSemantics[semanticName] = semanticIndex;
      }

      // Get or create an input layout that matches this input element description
      ID3D10InputLayout * inputLayout = m_inputLayoutManager.GetInputLayout(inputElementDescs, *pass);

      // Calculate the offsets
      std::vector<unsigned> offsets;
      for(unsigned i = 0; i < vertexBufferIndices.size(); ++i)
      {
         offsets.push_back(0);
      }

      // Calculate the strides
      std::vector<unsigned> strides;
      for(std::vector<unsigned>::iterator it = vertexBufferIndices.begin(); it != vertexBufferIndices.end(); ++it)
      {
         unsigned index = *it;
         unsigned stride = GetStride(m_vertexBuffers[index]->GetContentType());
         strides.push_back(stride);
      }      

      // Calculate how many vertices will be drawn this pass
      unsigned numVertices = m_vertexBuffers[vertexBufferIndices[0]]->GetNumElements(); 

      // Store the filled out pass info
      PassInfo passInfo(pass, vertexBufferIndices, offsets, strides, inputLayout, numVertices);
      m_perPassInfo.push_back(passInfo);
   }
}

//------------------------------------------------------------------------------------------
bool PolygonSet::CompareSigParamDescs::operator () (const D3D10_SIGNATURE_PARAMETER_DESC & lhs,
                                                    const D3D10_SIGNATURE_PARAMETER_DESC & rhs)
{
   std::string lSemanticName(lhs.SemanticName);
   std::string rSemanticName(rhs.SemanticName);

   if( lSemanticName < rSemanticName )
   {
      return true;
   }
   
   if( lSemanticName == rSemanticName && lhs.SemanticIndex < rhs.SemanticIndex )
   {
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------------------
PolygonSet::PassInfo::PassInfo(Pass * pass,
                               const std::vector<unsigned> & vertexBufferIndices,
                               const std::vector<unsigned> & offsets,
                               const std::vector<unsigned> & strides,
                               ID3D10InputLayout * inputLayout,
                               unsigned numVertices)
   :
   m_pass(pass),
   m_vertexBufferIndices(vertexBufferIndices),
   m_offsets(offsets),
   m_strides(strides),
   m_inputLayout(inputLayout),
   m_numVertices(numVertices)
{
}

//------------------------------------------------------------------------------------------
PolygonSet::PassInfo::PassInfo(const PassInfo & rhs)
   :
   m_pass(rhs.m_pass),
   m_vertexBufferIndices(rhs.m_vertexBufferIndices),
   m_offsets(rhs.m_offsets),
   m_strides(rhs.m_strides),
   m_inputLayout(rhs.m_inputLayout),
   m_numVertices(rhs.m_numVertices)
{
}

//------------------------------------------------------------------------------------------
PolygonSet::PassInfo & PolygonSet::PassInfo::operator = (const PassInfo & rhs)
{
   m_pass                = rhs.m_pass;
   m_vertexBufferIndices = rhs.m_vertexBufferIndices;
   m_offsets             = rhs.m_offsets;
   m_strides             = rhs.m_strides;
   m_inputLayout         = rhs.m_inputLayout;
   m_numVertices         = rhs.m_numVertices;

   return *this;
}
