#ifndef POINTLIGHT_H
#define POINTLIGHT_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

//------------------------------------------------------------------------------------
/**
* Point Light
*
* A point light is a source of light that shines equally in all directions. 
* For example, a light bulb. 
* 
* The intensity of the light is a calculated in the shader, instead of being set by the application.
*/
struct PointLight
{   
   /**
   * Constructor
   */
   PointLight(const D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
              const D3DXCOLOR color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f),
              const float range = 1.0f,
              const float falloff = 1.0f);

   /**
   * Copy Constructor
   */
   PointLight(const PointLight & rhs);

   /**
   * Deconstructor
   */
   ~PointLight();

   /**
   * Assignment Operator
   */
   PointLight & operator = (const PointLight & rhs);


   D3DXVECTOR3 m_position;
   D3DXCOLOR   m_color;
   float       m_range;
   float       m_falloff;
};



#endif // POINTLIGHT_H