#ifndef PASS_H
#define PASS_H

// EngineX Includes
#include "Graphics\3D\Buffers.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <vector>

class Technique;

//----------------------------------------------------------------------------
class Pass
{
public:

   /** Only a Technique should construct a Pass */
   friend class Technique;
   
   ~Pass();
 

   /**
   * Gets the D3D pass description
   **/
   const D3D10_PASS_DESC GetPassDesc() const;

   /** 
   * Gets descriptions of what kinds of data this pass expects to be input to its vertex shader
   **/
   const std::vector<D3D10_SIGNATURE_PARAMETER_DESC> & GetSignatureParamDescs() const;

   /**
   * Runs the shader code for this pass
   **/
   void Apply();

private:

   /**
   * Constructor
   *
   * Only a Technique should construct a Pass
   **/
   Pass(ID3D10EffectPass * pass);

   /* No copy not allowed */
   Pass(const Pass & rhs);

   /** No assignment allowed */
   Pass & operator = (const Pass & rhs);



   /** D3D interface to a pass */
   ID3D10EffectPass * m_pass;

   /** Description of what kinds of data this pass expects to be input to its vertex shader */
   std::vector<D3D10_SIGNATURE_PARAMETER_DESC> m_sigParamDescs;
};

#endif // PASS_H