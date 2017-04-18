#ifndef INPUTLAYOUTMANAGER_H
#define INPUTLAYOUTMANAGER_H

// EngineX Includes
#include "Graphics\3D\InputElementDescription.h"
#include "Graphics\Effects\Pass.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <vector>
#include <map>


//----------------------------------------------------------------------------
class InputLayoutManager
{
public:

   /**
   * Constructor
   **/
   InputLayoutManager(ID3D10Device & device);

   /**
   * Deconstructor
   */
   ~InputLayoutManager();


   /**
   * Gets or creates a D3D input layout matching the input element description
   *
   * If an input layout matching the input element description has not been requested before, 
   * it will be created. Creation of an input layout is costly, therefore it is recommended to
   * make an attempt to create all input layouts at init time.
   **/
   ID3D10InputLayout * GetInputLayout(const std::vector<InputElementDescription> inputElementDescs,
                                      const Pass & pass);

private:

   ID3D10Device & m_device;

   /**
   * A map of D3D Input Layout, mapped by the Input Element Descriptions that were used to create them
   **/
   struct CompareKeys
   {
      bool operator() (const std::vector<InputElementDescription> & lhs,
                       const std::vector<InputElementDescription> & rhs) const;

   };

   typedef std::map< std::vector<InputElementDescription>, ID3D10InputLayout *, CompareKeys> InputLayoutMap;
   InputLayoutMap m_inputLayouts;
};

#endif