
#ifndef FLIGHTCAMERA_H
#define FLIGHTCAMERA_H

// EngineX Includes
#include "Graphics/Cameras/BaseCamera.h"
#include "Graphics/3D/Transform.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

//----------------------------------------------------------------------------
class FlightCamera : public BaseCamera , public Transform
{
public:

   /**
   * Constructor
   *
   * Creates a camera at a given position 
   * 
   * By default the camera will look down the positive Z axis, and have an up vector of (0,1,0)
   *
   * @param client_width  - width of the client area in pixels
   * @param client_height - height of the client area in pixels
   * @param nearClip - how close something can be before it is not drawn
   * @param farClip - how far something can be before it is not drawn
   * @param postion - where to put the camera in the world coordinate system
   * @param lookat - point in world space that the camera is focusing on
   * @param up - postive y axis of the camera's orientation
   */
   FlightCamera(const unsigned int client_width, 
                const unsigned int client_height,
                const float nearClip,
                const float farClip,
                const D3DXVECTOR3 & position,
                const D3DXVECTOR3 & lookat,
                const D3DXVECTOR3 & up);

   /**
   * Deconstructor
   */
	~FlightCamera();

   
   /**
   * Updates and obtains the current view matrix
   *
   * @return D3DXMATRIX & - camera's updated view matrix
   */
   virtual const D3DXMATRIX & GetViewMatrix();

   /**
   * Obtains the current perspective matrix
   *
   * @return D3DXMATRIX & - camera's current perspective matrix
   */
   virtual const D3DMATRIX & GetProjectionMatrix() const;

   /**
   * Change the perspective matrix
   *
   * Call this method when the window client area changes
   *
   * @param clientWidth - Width of the window
   * @param clientHeight - Height of the window
   * @param nearClip - Clips anything postioned closer than this distance from the camera
   * @param farClip - Clips anything positioned farther than this distance from the camera
   */
   virtual void SetPerspectiveMatrix(const unsigned clientWidth, 
                                     const unsigned clientHeight,
                                     const float nearClip,
                                     const float farClip);
   
   /**
   * Sets the camera's positon and orientation
   *
   * @param position - Position of the camera
   * @param lookat - The focus point of the camera's view
   * @param up - Which direction, in world space, is the camera's up direction
   */
   void SetLookAt(const D3DXVECTOR3 & position,
                  const D3DXVECTOR3 & lookat,
                  const D3DXVECTOR3 & up);

protected:

   /**
   * Updates the view matrix to reflect the camera's current position and orientation
   */
   virtual void Update();

private:
   
};

#endif FLIGHTCAMERA_H
