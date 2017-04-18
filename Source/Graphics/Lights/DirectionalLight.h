#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

//------------------------------------------------------------------------------------
/**
* Directional Light
*
* The can be multiple directional lights in a scene.
* An directional light is a source that only has a direction vector and a color defined. 
* It lights all objects (with an equal material) equally from one direction.
*
* For example, a star or the sun might be a directional light in a scene.
*/
struct DirectionalLight
{
   /**
   * Constructor
   *
   * By default, creates a white directional light at full intensity
   */
   DirectionalLight::DirectionalLight(const D3DXVECTOR3 direction = D3DXVECTOR3(0.0f, -1.0f, 0.0f), 
                                      const D3DXCOLOR color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
                                      const bool enabled = true);

   /**
   * Copy Constructor
   */
   DirectionalLight(const DirectionalLight & rhs);

   /**
   * Deconstructor
   */
   ~DirectionalLight();

   /**
   * Assignment Operator
   */
   DirectionalLight & operator = (const DirectionalLight & rhs);

   
   bool        m_enabled;
   D3DXVECTOR3 m_direction;
   D3DXCOLOR   m_color;
};

#endif // DIRECTIONALLIGHT_H