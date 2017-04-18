

#include "PointLight.h"


//----------------------------------------------------------------------------
PointLight::PointLight(const D3DXVECTOR3 position, const D3DXCOLOR color, const float range, const float falloff)
   :
   m_position(position),
   m_color(color),
   m_range(range),
   m_falloff(falloff)
{
}

//----------------------------------------------------------------------------
PointLight::PointLight(const PointLight & rhs)
   :
   m_position(rhs.m_position),
   m_color(rhs.m_color),
   m_range(rhs.m_range),
   m_falloff(rhs.m_falloff)
{
}

//----------------------------------------------------------------------------
PointLight::~PointLight()
{
}

//----------------------------------------------------------------------------
PointLight & PointLight::operator = (const PointLight & rhs)
{
   m_position = rhs.m_position;
   m_color    = rhs.m_color;
   m_range    = rhs.m_range;
   m_falloff  = rhs.m_falloff;

   return *this;
}
