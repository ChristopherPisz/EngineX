#ifndef TEXTAREA2D_H
#define TEXTAREA2D_H

#include "Graphics\2D\SceneObject2D.h"
#include "Graphics\2D\Font2D.h"

#include <d3d9.h>

#include <string>

class TextArea2D : public SceneObject2D
{
public:
    TextArea2D(LPDIRECT3DDEVICE9 device = NULL);
    virtual ~TextArea2D();

    virtual void CreateTextArea(D3DXVECTOR2 position, Font2D font = Font2D(), std::string content = std::string("Uninitialized"), float depth = 0.0f, float width = 5.0f, float height = 5.0f, bool active = true);

protected:
    Font2D						m_font;				// Font used with this text area
    LPDIRECT3DVERTEXBUFFER9		m_vertexBuffer;		// Vertex Buffer

    std::string					m_content;			// The text to be displayed

    D3DXVECTOR2					m_position;			// Position of the center of the text area in screen coordinates
    float						m_depth;			// Depth of the text area in world coordinates on the z axis
    unsigned					m_width;			// Width of the text area in pixels
    unsigned					m_height;			// Height of the text area in pixels
    bool						m_active;			// Whether or not this text area is active. If the text area is not active the render function will skip rendering and simply return

};

#endif
