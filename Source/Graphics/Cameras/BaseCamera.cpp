

#include "BaseCamera.h"



//----------------------------------------------------------------------------
BaseCamera::BaseCamera()
{  
   // Derived classes will need to initialize the projection matrix
   // in their own way depending if they are 2D or 3D
   D3DXMatrixIdentity(&m_projection);           
}

//----------------------------------------------------------------------------
BaseCamera::~BaseCamera()
{
}
