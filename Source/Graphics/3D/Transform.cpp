

#include "Transform.h"



//--------------------------------------------------------------------------------
Transform::Transform()
   :
   m_position(0.0f, 0.0f, 0.0f),
   m_scale(1.0f, 1.0f, 1.0f)
{
   D3DXMatrixIdentity(&m_transform);
   D3DXQuaternionRotationMatrix(&m_orientation, &m_transform);
}

//--------------------------------------------------------------------------------
Transform::Transform(const Transform & rhs)
   :
   m_transform(rhs.m_transform),
   m_position(rhs.m_position),
   m_orientation(rhs.m_orientation),
   m_scale(rhs.m_scale),
   m_needUpdated(rhs.m_needUpdated)
{
}

//--------------------------------------------------------------------------------   
Transform::~Transform()
{
}

//--------------------------------------------------------------------------------
Transform & Transform::operator = (const Transform & rhs)
{
   if( this == &rhs )
   {
      return *this;
   }

   m_transform   = rhs.m_transform;
   m_position    = rhs.m_position;
   m_orientation = rhs.m_orientation;
   m_scale       = rhs.m_scale;
   m_needUpdated = rhs.m_needUpdated;

   return *this;
}

//--------------------------------------------------------------------------------   
const D3DXMATRIX & Transform::GetTransform()
{
   if(m_needUpdated)
   {
      Update();
   }

   return m_transform;
}

//--------------------------------------------------------------------------------
void Transform::ApplyTranslation(const float distance, const D3DXVECTOR3 & direction, const bool objectSpace)
{
   // Normalize the direction
   D3DXVECTOR3 vector;
   D3DXVec3Normalize(&vector, &direction);

   // If we are moving in object space
   if( objectSpace )
   {
      // Transform the direction from object space to a direction in world space
      vector = ObjectAxisToWorld(vector);
   }
	
   // Translate the object
   m_position += vector * distance;

   // Flag the transform matrix to be updated
   m_needUpdated = true;
}

//--------------------------------------------------------------------------------
void Transform::ApplyRotation(const float angle, const D3DXVECTOR3 & objectAxis)
{
   // Normalize the axis
   D3DXVECTOR3 axis;
   D3DXVec3Normalize(&axis, &objectAxis);

   // Transform the axis to world space
   axis = ObjectAxisToWorld(axis);

   // Rotate the 3D object
   D3DXQUATERNION rotation;
   D3DXQuaternionRotationAxis(&rotation, &axis, angle);

   m_orientation *= rotation;
   D3DXQuaternionNormalize(&m_orientation , &m_orientation);

   // Flag the transform matrix to be updated
   m_needUpdated = true;
}

//--------------------------------------------------------------------------------
const D3DXVECTOR3 & Transform::GetPosition() const
{
   return m_position;
}

//--------------------------------------------------------------------------------
void Transform::SetPosition(const D3DXVECTOR3 & position)
{
   m_position    = position;
   m_needUpdated = true;
}
	
//--------------------------------------------------------------------------------
const D3DXQUATERNION & Transform::GetOrientation() const
{
   return m_orientation;
}

//--------------------------------------------------------------------------------
void Transform::SetOrientation(const D3DXQUATERNION & orientation)
{
   m_orientation = orientation;
   m_needUpdated = true;
}

//--------------------------------------------------------------------------------
const D3DXVECTOR3 & Transform::GetScale() const
{
   return m_scale;
}

//--------------------------------------------------------------------------------
void Transform::SetScale(const D3DXVECTOR3 & scale)
{
   m_scale       = scale;
   m_needUpdated = true;
}

//--------------------------------------------------------------------------------
void Transform::Slerp(const D3DXQUATERNION & target_orientation, float speed)
{ 
   // Get an interpolated orientation
   D3DXQUATERNION interpolatedRotation;
   
   D3DXQuaternionSlerp(&interpolatedRotation, 
                       &m_orientation,
                       &target_orientation,
                       speed);

   // Apply interpolated rotation
   m_orientation = interpolatedRotation;
   D3DXQuaternionNormalize(&m_orientation , &m_orientation);

   // Flag the view matrix for updating
   m_needUpdated	= true;
}

//---------------------------------------------------------------------------------
void Transform::Update()
{
   // Calculate the scaling
   D3DXMATRIX matScale;
   D3DXMatrixScaling(&matScale, m_scale.x, m_scale.y, m_scale.z);

   // Calcuate the translation
   D3DXMATRIX matTranslation;
   D3DXMatrixTranslation(&matTranslation, m_position.x, m_position.y, m_position.z);

   // Calculate the rotation
   D3DXMATRIX matRotation;
   D3DXMatrixRotationQuaternion(&matRotation, &m_orientation);

   // Apply scaling, translation, and rotation matrices to the transform
   D3DXMatrixMultiply(&m_transform, &matScale, &matRotation);
   D3DXMatrixMultiply(&m_transform, &m_transform, &matTranslation);

   // The world matrix is up to date
   m_needUpdated = false;
}

//-------------------------------------------------------------------------------
D3DXVECTOR3 Transform::ObjectAxisToWorld(const D3DXVECTOR3 & objectAxis)
{
   // Build a matrix from the 3D object's orientation
   D3DXMATRIX	matRotation;
   D3DXMatrixRotationQuaternion(&matRotation, &m_orientation); 

   // Transform the axis vector by the matrix.
   D3DXVECTOR3	worldVector;
   D3DXVec3TransformCoord(&worldVector, &objectAxis, &matRotation);

   return worldVector;
}