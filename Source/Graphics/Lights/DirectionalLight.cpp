
#include "DirectionalLight.h"

//----------------------------------------------------------------------------
DirectionalLight::DirectionalLight(const D3DXVECTOR3 direction, const D3DXCOLOR color, const bool enabled)
   :
   m_enabled(enabled),
   m_direction(direction),
   m_color(color)
{
}

//----------------------------------------------------------------------------
DirectionalLight::DirectionalLight(const DirectionalLight & rhs)
:
   m_enabled(rhs.m_enabled),
   m_direction(rhs.m_direction),
   m_color(rhs.m_color)
{
}

//----------------------------------------------------------------------------
DirectionalLight::~DirectionalLight()
{
}

//----------------------------------------------------------------------------
DirectionalLight & DirectionalLight::operator = (const DirectionalLight & rhs)
{
   m_enabled   = rhs.m_enabled;
   m_direction = rhs.m_direction;
   m_color     = rhs.m_color;

   return *this;
}
