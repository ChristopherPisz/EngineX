
#include "Image2D.h"

// Common Lib Includes
#include "Exception.h"
#include "StringUtility.h"

// Standard Includes
#include <sstream>

//-------------------------------------------------------------------
Image2D::Image2D(ID3D10Device & device, 
                 InputLayoutManager & inputLayoutManager,
                 TextureManager & textureManager, 
                 EffectManager & effectManager,
                 const std::string & textureFilePath,
                 unsigned rows,
                 unsigned cols)
    :
    Transform           (),
    m_device            (device),
    m_inputLayoutManager(inputLayoutManager),
    m_textureManager    (textureManager),
    m_effectManager     (effectManager),
    m_inputLayout       (nullptr),
    m_numFrames         (rows * cols),
    m_width             (0),
    m_height            (0),
    m_animationMode     (ANIMATION_MODE_NONE),
    m_startFrameIndex   (0),
    m_endFrameIndex     (0),
    m_currentFrameIndex (0),
    m_fps               (0)
{
    // Load the texture
    Common::RemoveExtFromFilename(textureFilePath, m_textureName);
    Texture * texture = NULL;

    try
    {
        texture = &(m_textureManager.CreateTextureFromFile(m_textureName, textureFilePath));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    m_width  = static_cast<float>(texture->GetWidth() / cols);
    m_height = static_cast<float>(texture->GetHeight() / rows);

    if(texture->GetWidth() % cols || texture->GetHeight() % rows)
    {
        std::string msg(textureFilePath);
        msg += " is not evenly divisible into the requested number of frames";
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Create the effect
    Effect *    effect    = NULL;
    Technique * technique = NULL;
    Pass *      pass      = NULL;
   
    try
    {
        effect    = &(m_effectManager.CreateChildEffect("image", "image.fx"));
        technique = &(effect->GetTechnique("RenderWithAlpha"));
        pass      = &(technique->GetPass(0));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create a material
    try
    {
        m_material = effect->CreateMaterial();
        m_material->SetBool("diffuseMapped", true);
        m_material->SetTextureName("diffuseTexture", m_textureName);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create a quad to texture with the image
    std::vector<Position>   quadPositions;
    std::vector<TexCoord2D> quadTexCoords;

    D3DXVECTOR2 positionOffset;
    positionOffset.x = 0.5f * m_width;  // Distance from origin on x axis used to generate vertex positions for a quad
    positionOffset.y = 0.5f * m_height; // Distance from origin on y axis used to generate vertex positions for a quad

    D3DXVECTOR2 textureOffset;
    textureOffset.x = 1.0f / static_cast<float>(cols);	// How much to shift the texture coords for each frame horizontally
    textureOffset.y = 1.0f / static_cast<float>(rows);	// How much to shift the texture coords for each frame vertically

    for(unsigned currentRow = 0; currentRow < rows; ++currentRow)
    {
	    for(unsigned currentColumn = 0; currentColumn < cols; ++currentColumn)
	    {
            quadPositions.push_back(Position( -positionOffset.x, -positionOffset.y, 0.0f));
            quadTexCoords.push_back(TexCoord2D( currentColumn * textureOffset.x, (currentRow + 1) * textureOffset.y));

            quadPositions.push_back(Position( -positionOffset.x,  positionOffset.y, 0.0f));
            quadTexCoords.push_back(TexCoord2D( currentColumn * textureOffset.x, currentRow * textureOffset.y));

            quadPositions.push_back(Position(  positionOffset.x, -positionOffset.y, 0.0f));
            quadTexCoords.push_back(TexCoord2D( (currentColumn + 1) * textureOffset.x, (currentRow + 1) * textureOffset.y));

            quadPositions.push_back(Position(  positionOffset.x,  positionOffset.y, 0.0f));
            quadTexCoords.push_back(TexCoord2D( (currentColumn + 1) * textureOffset.x, currentRow * textureOffset.y));
	    }
    }

    Buffer::SharedPtr quadPositionBuffer(new Buffer(m_device, POSITION, quadPositions, false));
    Buffer::SharedPtr quadTexCoordBuffer(new Buffer(m_device, TEXCOORD2D, quadTexCoords, false));
    m_quadBuffers.push_back(quadPositionBuffer);
    m_quadBuffers.push_back(quadTexCoordBuffer);

    // Create an input layout
    std::vector<InputElementDescription> inputElementDescs;
    std::vector<InputElementDescription> thisElementDesc;

    InputElementDescription::GetInputElementDesc(thisElementDesc,
                                                m_quadBuffers[0]->GetContentType(), 
                                                0,
                                                0,
                                                m_quadBuffers[0]->IsPerInstance());

    inputElementDescs.insert(inputElementDescs.end(), thisElementDesc.begin(), thisElementDesc.end());

    InputElementDescription::GetInputElementDesc(thisElementDesc,
                                                m_quadBuffers[1]->GetContentType(), 
                                                0,
                                                1,
                                                m_quadBuffers[1]->IsPerInstance());

    inputElementDescs.insert(inputElementDescs.end(), thisElementDesc.begin(), thisElementDesc.end());

    m_inputLayout = m_inputLayoutManager.GetInputLayout(inputElementDescs, *pass);

    // Calculate the strides
    m_strides.push_back(GetStride(m_quadBuffers[0]->GetContentType()));
    m_strides.push_back(GetStride(m_quadBuffers[1]->GetContentType()));
}

//-------------------------------------------------------------------
Image2D::~Image2D()
{
}

//-------------------------------------------------------------------
unsigned Image2D::GetNumFrames() const
{
   return m_numFrames;
}

//-------------------------------------------------------------------
unsigned Image2D::GetCurrentFrameIndex() const
{
   return m_currentFrameIndex;
}

//-------------------------------------------------------------------
void Image2D::SetCurrentFrameIndex(unsigned index)
{
    // Check for a valid index
    if( index < 0 || index >= m_numFrames )
    {
        std::stringstream msg;
        msg << "Frame index " << index << "does not exist. Valid frame indices are 0 to " << (m_numFrames - 1); 
        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    // Stop any animation in progress
    if( m_animationTimer.IsStarted() )
    {
        StopAnimation();
    }

    // Set the frame to be rendered
    m_currentFrameIndex = index;
}

//-------------------------------------------------------------------
void Image2D::StartAnimation(unsigned startFrameIndex, unsigned endFrameIndex, AnimationMode animationMode, double fps)
{
    // Check for valid indices
    if( startFrameIndex <  0            || 
        startFrameIndex >= m_numFrames  ||
        endFrameIndex   <  0            || 
        endFrameIndex   >= m_numFrames  ||
        startFrameIndex >  endFrameIndex )
    {
        std::stringstream msg;
        msg << "Invalid start index: " << startFrameIndex << " or end index: " << endFrameIndex << ". ";
        msg << "Valid frame indices are 0 to " << (m_numFrames - 1); 
        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    // Set up the animation
    m_startFrameIndex   = startFrameIndex;
    m_currentFrameIndex = startFrameIndex;
    m_endFrameIndex     = endFrameIndex;
    m_animationMode     = animationMode;
    m_fps               = fps;

    // Handle 'silly but possible' call
    if( animationMode == ANIMATION_MODE_NONE )
    {
        StopAnimation();
        return;
    }

    // Start the animation
    m_animationTimer.Start();
}

//-------------------------------------------------------------------
void Image2D::StopAnimation()
{
    m_startFrameIndex   = m_currentFrameIndex;
    m_endFrameIndex     = m_currentFrameIndex;
    m_animationMode     = ANIMATION_MODE_NONE;
    m_fps               = 0;

    m_animationTimer.Stop();
}

//-------------------------------------------------------------------
void Image2D::Render()
{
    // Get the original matrices
    D3DXMATRIX origProjection;
    D3DXMATRIX origView;

    m_effectManager.GetViewMatrix(origView);
    m_effectManager.GetProjectionMatrix(origProjection);

    // Create new matrices for rendering 2D
    D3DXMATRIX     newProjection;	
    D3DXMATRIX     newView;
    D3DXMATRIX     world;
    UINT           numViewports = 1;
    D3D10_VIEWPORT viewport;

    m_device.RSGetViewports(&numViewports, &viewport);

    D3DXMatrixIdentity(&newView);
    D3DXMatrixOrthoLH(&newProjection, static_cast<float>(viewport.Width), static_cast<float>(viewport.Height), 0.0f, 1.0f);

    m_effectManager.SetViewMatrix(newView);
    m_effectManager.SetProjectionMatrix(newProjection);

    // Set the world matrix and material
    Effect *    effect    = NULL;
    Technique * technique = NULL;
    Pass *      pass      = NULL;

    try
    {
        effect    = &(m_effectManager.GetChildEffect("image"));
        technique = &(effect->GetTechnique("RenderWithAlpha"));
        pass      = &(technique->GetPass(0));

        effect->SetWorldMatrix(GetTransform());
        effect->SetMaterial(*m_material);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Bind the input layout
    m_device.IASetInputLayout(m_inputLayout);
    m_device.IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Bind the vertex buffers
    std::vector<ID3D10Buffer *> vertexBuffers;
    vertexBuffers.push_back( m_quadBuffers[0]->GetD3DBuffer() );
    vertexBuffers.push_back( m_quadBuffers[1]->GetD3DBuffer() );

    std::vector<unsigned> offsets;
    offsets.push_back(0);
    offsets.push_back(0);

    m_device.IASetVertexBuffers(0,
                                static_cast<UINT>(vertexBuffers.size()),
                                &vertexBuffers[0], 
                                &m_strides[0], 
                                &offsets[0]);

    // Calculate the frame to be rendered
    CalculateAnimationFrame();
    unsigned startVertex = (m_currentFrameIndex) * 4;

    // Apply the pass
    pass->Apply();

    // Draw
    m_device.Draw(4, startVertex);

    // Restore the original matrices
    m_effectManager.SetProjectionMatrix(origProjection);
    m_effectManager.SetViewMatrix(origView);
}

//-------------------------------------------------------------------
const D3DXVECTOR2 Image2D::GetPosition2D() const
{
    return D3DXVECTOR2(m_position.x, m_position.y);
}

//-------------------------------------------------------------------
const float Image2D::GetDepth2D() const
{
    return m_position.z;
}

//-------------------------------------------------------------------
const D3DXVECTOR2 Image2D::GetScale2D() const
{
    return D3DXVECTOR2(m_scale.x, m_scale.y);
}

//-------------------------------------------------------------------
void Image2D::SetPosition2D(const D3DXVECTOR2 & position)
{
    m_position.x = position.x;
    m_position.y = position.y;
}

//-------------------------------------------------------------------
void Image2D::SetDepth2D(const float depth)
{
    m_position.z = depth;
}

//-------------------------------------------------------------------
void Image2D::SetScale2D(const D3DXVECTOR2 & scale)
{
    m_scale = D3DXVECTOR3(scale.x, scale.y, 1.0f);
}

//-------------------------------------------------------------------
void Image2D::CalculateAnimationFrame()
{
    // Check if an animation is playing
    if( !m_animationTimer.IsStarted() )
    {
        return;
    }

    // Make the calculation
    double   frameInterval     = 1.0 / m_fps;
    double   elapsedTime       = m_animationTimer.GetElapsedTime();
    unsigned framesToAdvance   = static_cast<unsigned>(elapsedTime / frameInterval);
    unsigned framesInAnimation = m_endFrameIndex - m_startFrameIndex + 1;

    switch(m_animationMode)
    {
        case ANIMATION_MODE_ONCE :
        {
            if( framesToAdvance > m_endFrameIndex )
            {
                m_currentFrameIndex = m_endFrameIndex;
                StopAnimation();
            }
            else
            {
                m_currentFrameIndex = m_startFrameIndex + framesToAdvance;
            }

            break;
        }

        case ANIMATION_MODE_LOOP :
        {
            m_currentFrameIndex = m_startFrameIndex + (framesToAdvance % framesInAnimation);
         
            break;
        }
      
        case ANIMATION_MODE_BOUNCE :
        {
            bool backward = ( (framesToAdvance / (framesInAnimation - 1)) % 2 != 0 );

            if( !backward )
            {
                m_currentFrameIndex = m_startFrameIndex + (framesToAdvance % (framesInAnimation - 1));
            }
            else
            {
                m_currentFrameIndex = m_endFrameIndex - (framesToAdvance % (framesInAnimation - 1));
            }

            break;
        }

        case ANIMATION_MODE_NONE :
        default:
        {
            break;
        }
    }
}

