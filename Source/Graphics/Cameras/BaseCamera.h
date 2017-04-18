
#ifndef BASECAMERA_H
#define BASECAMERA_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

//----------------------------------------------------------------------------
class BaseCamera
{
public:

   /**
   * Constructor
   */
   BaseCamera();

   /**
   * Deconstructor
   */
	virtual ~BaseCamera();


   /**
   * Obtains the current view matrix
   */
   virtual const D3DXMATRIX & GetViewMatrix() = 0;

   /**
   * Obtains the current projection matrix
   */
   virtual const D3DMATRIX & GetProjectionMatrix() const = 0;


protected:

   D3DXMATRIX  m_projection;
   

private:

};

#endif BASECAMERA_H
