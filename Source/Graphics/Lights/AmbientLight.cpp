
#include "AmbientLight.h"

//----------------------------------------------------------------------------
AmbientLight::AmbientLight(const float intensity, const D3DXCOLOR color)
   :
   m_intensity(intensity),
   m_color(color)
{
}

//----------------------------------------------------------------------------
AmbientLight::AmbientLight(const AmbientLight & rhs)
   :
   m_intensity(rhs.m_intensity),
   m_color(rhs.m_color)
{
}

//----------------------------------------------------------------------------
AmbientLight::~AmbientLight()
{
}

//----------------------------------------------------------------------------
AmbientLight & AmbientLight::operator = (const AmbientLight & rhs)
{
   m_intensity = rhs.m_intensity;
   m_color     = rhs.m_color;

   return *this;
}
