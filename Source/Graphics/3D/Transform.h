
#ifndef TRANSFORM_H
#define TRANSFORM_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>



//--------------------------------------------------------------------------------
class Transform
{
public:

   /**
   * Constructor
   */
	Transform();

   /**
   * Copy Constructor
   */
	Transform(const Transform & rhs);

   /**
   * Deconstructor
   */
	virtual ~Transform();

   /**
   * Assignment Operator
   */
	Transform & operator = (const Transform & rhs);


   //-----
   
   /**
   * Get the object's current postion
   */
	virtual const D3DXVECTOR3 & GetPosition() const;

   /**
   * Get the object's current orientation
   */
   virtual const D3DXQUATERNION & GetOrientation() const;

   /**
   * Gets the object's scaling factor
   */
   virtual const D3DXVECTOR3 & GetScale() const;

   /**
   * Updates and obtains the current transform matrix
   */
   virtual const D3DXMATRIX & GetTransform();

   //-----

   /**
   * Sets the object's postion
   */
	virtual void SetPosition(const D3DXVECTOR3 & position);

   /**
   * Sets the object's orientation
   */
   virtual void SetOrientation(const D3DXQUATERNION & orientation);

   /**
   * Sets the object's scaling factor
   */
   virtual void SetScale(const D3DXVECTOR3 & scale);

   //-----

   /**
   * Moves the the object
   *
   * @param distance - number of units to move the object in the given direction
   * @param direction - direction vector in which to move the object
   * @pram objectSpace - If true, then the direction is in object space, otherwise the direction is in world space
   */
	virtual void ApplyTranslation(const float distance, const D3DXVECTOR3 & direction, const bool objectSpace = false);

   /**
   * Rotates the object
   *
   * @param angle - radians to rotate clockwise when looking down the axis toward the origin in object space
   * @param objectAxis - specifies the axis to rotate around
   */
   virtual void ApplyRotation(const float angle, const D3DXVECTOR3 & objectAxis);

   
   //-----

   /**
   * Set the 3D object's orientation using an interpolated rotation from the 
   * current orientation to a specified orientation, given a rotation speed
   *
   * @param target_orientation - orientation of the object at speed = 1
   * @param speed - How far along the interpolated curve to rotate. A number between 0 and 1, 
   *                where 0 rotates to the current orientation and 1 rotates to the target orientation 
   */
	virtual void Slerp(const D3DXQUATERNION & target_orientation, float speed);

protected:

   /**
   * Updates the transform matrix to reflect the object's current position, orientation, and scale
   */
   virtual void Update();

   /**
   * Transforms an axis from object space to world space
   */
   D3DXVECTOR3 ObjectAxisToWorld(const D3DXVECTOR3 & objectAxis);


   /**
   * The position, orientation, and scale in matrix form
   */
   D3DXMATRIX     m_transform;

   /**
   * Seperate components of the total transform
   *
   * These are seperate components that change when the various methods are called 
   * The transform matrix is updated when needed, to reflect those changes
   */
   D3DXVECTOR3    m_position;
   D3DXQUATERNION m_orientation;
   D3DXVECTOR3    m_scale;

   /**
   * Whether or not the transform matrix needs to be updated to reflect changes
   * to position, orientation, or scale.
   */
   bool           m_needUpdated;


private:


};

#endif // TRANSFORM_H
