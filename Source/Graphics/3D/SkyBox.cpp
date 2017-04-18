
#include "SkyBox.h"

// Common Lib Includes
#include "StringUtility.h"
#include "Exception.h"

// Standard Includes
#include <vector>
#include <string>

//---------------------------------------------------------------------------
SkyBox::SkyBox(ID3D10Device & device,
               TextureManager & textureManager,
               EffectManager & effectManager,
               InputLayoutManager & inputLayoutManager,
               const std::string & textureFileName_cubeFaces)
    :
    m_device(device),
    m_effectManager(effectManager),
    m_inputLayout(NULL),
    m_effectName("skybox"),
    m_techniqueName("RenderDefault")
    {
    // Create geometry to render
    //
    // Geometry will be inward facing quads that are renderable as triangle strips
    std::vector<Position>   positions;
    std::vector<TexCoord2D> texCoords;

    // Pos X
    positions.push_back(Position(  1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position(  1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.16666666666666666666666666666667f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.16666666666666666666666666666667f, 0.0f ));

    // Neg X
    positions.push_back(Position( -1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.16666666666666666666666666666667f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.16666666666666666666666666666667f, 0.0f ));
    positions.push_back(Position( -1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.33333333333333333333333333333333f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.33333333333333333333333333333333f, 0.0f ));
   
    // Pos Y
    positions.push_back(Position( -1.0f,  1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 0.33333333333333333333333333333333f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 0.33333333333333333333333333333333f, 0.0f ));
    positions.push_back(Position(  1.0f,  1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 0.5f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 0.5f, 0.0f ));

    // Neg Y
    positions.push_back(Position( -1.0f, -1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 0.5f, 1.0f ));
    positions.push_back(Position( -1.0f, -1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 0.5f, 0.0f ));
    positions.push_back(Position(  1.0f, -1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 0.66666666666666666666666666666667f, 1.0f ));
    positions.push_back(Position(  1.0f, -1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 0.66666666666666666666666666666667f, 0.0f ));

    // Pos Z
    positions.push_back(Position( -1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.66666666666666666666666666666667f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.66666666666666666666666666666667f, 0.0f ));
    positions.push_back(Position(  1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.83333333333333333333333333333333f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.83333333333333333333333333333333f, 0.0f ));

    // Neg Z
    positions.push_back(Position(  1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.83333333333333333333333333333333f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.83333333333333333333333333333333f, 0.0f ));
    positions.push_back(Position( -1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));

    // Create Buffers
    Buffer::SharedPtr bufPositions(new Buffer(device, POSITION, positions));
    Buffer::SharedPtr bufTexCoords(new Buffer(device, TEXCOORD2D, texCoords));

    m_buffers.push_back(bufPositions);
    m_buffers.push_back(bufTexCoords);

    // Remove extension from the texture filename
    RemoveExtFromFilename(textureFileName_cubeFaces, m_cubeFacesTextureName);
   
    // Load the textures
    try
    {
        textureManager.CreateTextureFromFile(m_cubeFacesTextureName, textureFileName_cubeFaces);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Load an effect and get a material to use
    Effect *    effect    = NULL;
    Technique * technique = NULL;
    Pass *      pass      = NULL;

    try
    {
        effect    = &(effectManager.CreateChildEffect(m_effectName, m_effectName + ".fx"));
        technique = &(effect->GetTechnique(m_techniqueName));
        pass      = &(technique->GetPass(0));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create a material to use
    m_material = effect->CreateMaterial();
    m_material->SetTextureName("textureDiffuse", m_cubeFacesTextureName);

    // Create and store the input layout
    std::vector<InputElementDescription> inputElementDescs;

    for(std::vector<Buffer::SharedPtr>::size_type i = 0; i < m_buffers.size(); ++i)
    {
        std::vector<InputElementDescription> elementDesc;
        InputElementDescription::GetInputElementDesc(elementDesc,
                                                    m_buffers[i]->GetContentType(),
                                                    0,
                                                    static_cast<unsigned>(i),
                                                    false);

        inputElementDescs.insert(inputElementDescs.end(), elementDesc.begin(), elementDesc.end());
    }

    m_inputLayout = inputLayoutManager.GetInputLayout(inputElementDescs, *pass);

    // Store the offsets for the buffers
    for(unsigned i = 0; i < m_buffers.size(); ++i)
    {
        m_offsets.push_back(0);
    }

    // Store the strides for the buffers
    for(unsigned i = 0; i < m_buffers.size(); ++i)
    {
        unsigned stride = GetStride(m_buffers[i]->GetContentType());
        m_strides.push_back(stride);
   }
}

//---------------------------------------------------------------------------
SkyBox::SkyBox(ID3D10Device & device, 
               TextureManager & textureManager, 
               EffectManager & effectManager, 
               InputLayoutManager & inputLayoutManager,
               const std::string & textureFileName_posX,
               const std::string & textureFileName_negX,
               const std::string & textureFileName_posY,
               const std::string & textureFileName_negY,
               const std::string & textureFileName_posZ,
               const std::string & textureFileName_negZ)
    :
    m_device(device),
    m_effectManager(effectManager),
    m_inputLayout(NULL),
    m_effectName("skybox"),
    m_techniqueName("RenderDefault")
{
    // Create geometry to render
    //
    // Geometry will be inward facing quads that are renderable as triangle strips
    std::vector<Position>   positions;
    std::vector<TexCoord2D> texCoords;

    // Pos X
    positions.push_back(Position(  1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position(  1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));

    // Neg X
    positions.push_back(Position( -1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position( -1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));
   
    // Pos Y
    positions.push_back(Position( -1.0f,  1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position(  1.0f,  1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));
	
    // Neg Y
    positions.push_back(Position( -1.0f, -1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position( -1.0f, -1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position(  1.0f, -1.0f, -1.0f));  texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position(  1.0f, -1.0f,  1.0f));  texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));

    // Pos Z
    positions.push_back(Position( -1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position(  1.0f, -1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f,  1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));

    // Neg Z
    positions.push_back(Position(  1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 1.0f ));
    positions.push_back(Position(  1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 0.0f, 0.0f ));
    positions.push_back(Position( -1.0f, -1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 1.0f ));
    positions.push_back(Position( -1.0f,  1.0f, -1.0f )); texCoords.push_back(TexCoord2D( 1.0f, 0.0f ));
  
    // Create Buffers
    Buffer::SharedPtr bufPositions(new Buffer(device, POSITION, positions));
    Buffer::SharedPtr bufTexCoords(new Buffer(device, TEXCOORD2D, texCoords));

    m_buffers.push_back(bufPositions);
    m_buffers.push_back(bufTexCoords);

    // Remove extension from the texture filenames
    RemoveExtFromFilename(textureFileName_posX, m_perSideTextureNames[0]);
    RemoveExtFromFilename(textureFileName_negX, m_perSideTextureNames[1]);
    RemoveExtFromFilename(textureFileName_posY, m_perSideTextureNames[2]);
    RemoveExtFromFilename(textureFileName_negY, m_perSideTextureNames[3]);
    RemoveExtFromFilename(textureFileName_posZ, m_perSideTextureNames[4]);
    RemoveExtFromFilename(textureFileName_negZ, m_perSideTextureNames[5]);
   
    // Load the textures
    try
    {
        textureManager.CreateTextureFromFile(m_perSideTextureNames[0], textureFileName_posX);
        textureManager.CreateTextureFromFile(m_perSideTextureNames[1], textureFileName_negX); 
        textureManager.CreateTextureFromFile(m_perSideTextureNames[2], textureFileName_posY);
        textureManager.CreateTextureFromFile(m_perSideTextureNames[3], textureFileName_negY);
        textureManager.CreateTextureFromFile(m_perSideTextureNames[4], textureFileName_posZ);
        textureManager.CreateTextureFromFile(m_perSideTextureNames[5], textureFileName_negZ);
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Load an effect to use
    Effect *    effect    = NULL;
    Technique * technique = NULL;
    Pass *      pass      = NULL;

    try
    {
        effect    = &(effectManager.CreateChildEffect(m_effectName, m_effectName + ".fx"));
        technique = &(effect->GetTechnique(m_techniqueName));
        pass      = &(technique->GetPass(0));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create and store the input layout
    std::vector<InputElementDescription> inputElementDescs;

    for(std::vector<Buffer::SharedPtr>::size_type i = 0; i < m_buffers.size(); ++i)
    {
        std::vector<InputElementDescription> elementDesc;
        InputElementDescription::GetInputElementDesc(elementDesc,
                                                    m_buffers[i]->GetContentType(),
                                                    0,
                                                    static_cast<unsigned>(i),
                                                    false);

        inputElementDescs.insert(inputElementDescs.end(), elementDesc.begin(), elementDesc.end());
    }

    m_inputLayout = inputLayoutManager.GetInputLayout(inputElementDescs, *pass);

    // Store the offsets for the buffers
    for(unsigned i = 0; i < m_buffers.size(); ++i)
    {
        m_offsets.push_back(0);
    }

    // Store the strides for the buffers
    for(unsigned i = 0; i < m_buffers.size(); ++i)
    {
        unsigned stride = GetStride(m_buffers[i]->GetContentType());
        m_strides.push_back(stride);
    }
}

//---------------------------------------------------------------------------
SkyBox::~SkyBox()
{
}

//---------------------------------------------------------------------------
void SkyBox::Render()
{
    Effect *    effect    = NULL;
    Technique * technique = NULL;
    Pass *      pass      = NULL;

    try
    {
        effect    = &(m_effectManager.GetChildEffect(m_effectName));
        technique = &(effect->GetTechnique(m_techniqueName));
        pass      = &(technique->GetPass(0));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Get the current projection and view matrices
    D3DXMATRIX origView;
    D3DXMATRIX origProjection;
   
    m_effectManager.GetViewMatrix(origView);
    m_effectManager.GetProjectionMatrix(origProjection);

    // Create matrices for rendering the skybox
    D3DXMATRIX newView       = origView;
    D3DXMATRIX newProjection = origProjection;

    // Adjust the view matrix such that it has no translation
    newView._41 = 0.0f;
    newView._42 = 0.0f;
    newView._43 = 0.0f;

    // Adjust the perspective matrix such that the near and far clip are 2 units apart
    float nearPlane = 0.01f;
    float farPlane  = 2.0f;
    float q         = farPlane / (farPlane - nearPlane);

    newProjection._33 = q;
    newProjection._43 = -q * nearPlane; 

    // Bind the matrices for rendering the skybox
    m_effectManager.SetViewMatrix(newView);
    m_effectManager.SetProjectionMatrix(newProjection);

    // Bind the input layout
    m_device.IASetInputLayout(m_inputLayout);

    // Bind the vertex buffers
    //
    // Since we are using shared pointers, we unfortunatly have to copy the D3D buffer interfaces they point to
    // into a seperate vector to pass to D3D when we bind them
   
    std::vector<ID3D10Buffer *> vertexBuffers;
    for(std::vector<Buffer::SharedPtr>::iterator it = m_buffers.begin(); it != m_buffers.end(); ++it)
    {
        vertexBuffers.push_back((*it)->GetD3DBuffer());
    }

    m_device.IASetVertexBuffers(0, static_cast<UINT>(m_buffers.size()), &vertexBuffers[0], &m_strides[0], &m_offsets[0]);             

    // Tell the input assembler how to assemble the vertices into primitives
    m_device.IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // If one texture is used for all faces
    if( !m_cubeFacesTextureName.empty() )
    {
        // Set the material
        effect->SetMaterial(*m_material);

        // Apply pass
        pass->Apply();

        // Render the face (one strip per face from the vertex buffer)  There are 2 primitives per face.
        for( unsigned i = 0; i < 6; ++i )
        {
            m_device.Draw(4, i * 4);
        }
    }

    // Otherwise render one face at a time with seperate textures for each
    else
    {
        // Draw one quad at a time
        for( unsigned i = 0; i < 6; ++i )
        {
            // Set the texture for this primitive
            m_material->SetTextureName("textureDiffuse", m_perSideTextureNames[i]);
            effect->SetMaterial(*m_material);

            // Apply pass
            pass->Apply();

            // Render the face (one strip per face from the vertex buffer)  There are 2 primitives per face.
            m_device.Draw(4, i * 4);
        }
    }

    // Restore the original view and projection matrices
    m_effectManager.SetViewMatrix(origView);
    m_effectManager.SetProjectionMatrix(origProjection);
}

