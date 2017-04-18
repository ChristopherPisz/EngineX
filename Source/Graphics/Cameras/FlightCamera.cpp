
#include "FlightCamera.h"



//---------------------------------------------------------------------------------
FlightCamera::FlightCamera(const unsigned int client_width, 
                           const unsigned int client_height,
                           const float nearClip,
                           const float farClip,
                           const D3DXVECTOR3 & position,
                           const D3DXVECTOR3 & lookat,
                           const D3DXVECTOR3 & up)
   :
   BaseCamera(),
   Transform()
{
   // Set the camera's perspective matrix
   D3DXMatrixPerspectiveFovLH(&m_projection,
                              static_cast<float>(D3DX_PI) * 0.25f,
                              static_cast<float>(client_width) / static_cast<float>(client_height),
                              nearClip,
                              farClip);

   // Set the camera's view matrix
   D3DXMatrixLookAtLH(&m_transform, &position, &lookat, &up);

   // Set the camera's current position and orientation
   m_position = position;
   D3DXQuaternionRotationMatrix(&m_orientation, &m_transform);

   // The view matrix is up to date
   m_needUpdated = false;
}

//---------------------------------------------------------------------------------
FlightCamera::~FlightCamera()
{
}

//----------------------------------------------------------------------------
const D3DXMATRIX & FlightCamera::GetViewMatrix()
{
   if(m_needUpdated)
   {
      Update();
   }

   return m_transform;
}

//----------------------------------------------------------------------------
const D3DMATRIX & FlightCamera::GetProjectionMatrix() const
{
   return m_projection;
}

//----------------------------------------------------------------------------
void FlightCamera::SetPerspectiveMatrix(const unsigned clientWidth, 
                                        const unsigned clientHeight,
                                        const float nearClip,
                                        const float farClip)
{
   D3DXMatrixPerspectiveFovLH(&m_projection,
                              static_cast<float>(D3DX_PI) * 0.5f,
                              static_cast<float>(clientWidth) / static_cast<float>(clientHeight),
                              nearClip,
                              farClip);
}

//---------------------------------------------------------------------------------
void FlightCamera::SetLookAt(const D3DXVECTOR3 & position ,
                             const D3DXVECTOR3 & lookat,
                             const D3DXVECTOR3 & up)
{
   // Setup a matrix represetning the given parameters
   D3DXMATRIX matTemp;
   D3DXMatrixLookAtLH(&matTemp, &position, &lookat, &up);

   // Extract the new position and orientation from the matrix
   m_position = position;
   D3DXQuaternionRotationMatrix(&m_orientation, &matTemp);

   // Flag the view matrix for updating
   m_needUpdated = true;
}

//---------------------------------------------------------------------------------
void FlightCamera::Update()
{
   // Calcuate the translation
   D3DXMATRIX matTranslation;
   D3DXMatrixTranslation(&matTranslation,
                         -m_position.x,
                         -m_position.y,
                         -m_position.z);

   // Calculate the rotation, by taking the conjucate of the quaternion
   D3DXMATRIX matRotation;
   D3DXMatrixRotationQuaternion(&matRotation,
                                &D3DXQUATERNION(-m_orientation.x,
                                                -m_orientation.y,
                                                -m_orientation.z,
                                                 m_orientation.w));

   // Apply translation and rotation matrices to view matrix
   //
   // The order of multiplication for a view matrix is T * R, which is the opposite order used to create the world matrix
   D3DXMatrixMultiply(&m_transform, &matTranslation, &matRotation);

   // The view matrix is up to date
   m_needUpdated = false;
}
