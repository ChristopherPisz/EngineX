#ifndef AMBIENTLIGHT_H
#define AMBIENTLIGHT_H

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

//------------------------------------------------------------------------------------
/**
* Ambient Light
*
* There can only be one ambient light in a scene.
* An ambient light had an intensity and a color.
*/
struct AmbientLight
{
   /**
   * Constructor
   *
   * By default, creates a white ambient light at full intensity
   */
   AmbientLight(const float intensity = 1.0f,
                const D3DXCOLOR color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

   /**
   * Copy Constructor
   */
   AmbientLight(const AmbientLight & rhs);

   /**
   * Deconstructor
   */
   ~AmbientLight();

   /**
   * Assignment Operator
   */
   AmbientLight & operator = (const AmbientLight & rhs);


   float       m_intensity;
   D3DXCOLOR   m_color;
};

#endif // AMBIENTLIGHT_H