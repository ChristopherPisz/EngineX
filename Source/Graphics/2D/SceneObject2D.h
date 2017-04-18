#ifndef SCENEOBJECT2D_H
#define SCENEOBJECT2D_H

#include <d3d9.h>

//----------------------------------------------------------------------------
class SceneObject2D
{
public:
	SceneObject2D(LPDIRECT3DDEVICE9 device = NULL);
	virtual ~SceneObject2D();

	virtual void Render() = 0;

protected:
	LPDIRECT3DDEVICE9		m_device;			// Direct3D rendering device

};

#endif
