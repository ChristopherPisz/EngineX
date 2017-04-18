

// Project Includes
#include "SceneObject2D.h"


//-----------------------------------------------------------------
SceneObject2D::SceneObject2D(LPDIRECT3DDEVICE9 device)
{
    // Share the device interface
    m_device = device;
    if(m_device)
    {
        m_device->AddRef();
    }
}

//-----------------------------------------------------------------
SceneObject2D::~SceneObject2D()
{
    if(m_device)
    {
        m_device->Release();
    }
}

