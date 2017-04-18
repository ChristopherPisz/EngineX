

/*
* TODO - Inheriting this from renderable is not going to work. There are too many differences that came to light as things
* got fleshed out. I am currently thinking of seperating a lens flare into differant classes for each component: 
* occlusion test, glow, and flare. Each of those could be a renderable with a single material and effect for each.
* Then a lens flare can aggregate all of those. The details of adding the flare itself to the renderqueue still need to
* be thought out. We know the renderqueue will need to know that an occlusion test must be rendered last. I believe we would
* also like the flares to be rendered on top of everything else no matter what. Perhaps a more generic ordering descriptor
* is needed. I shall ponder these things further. Inheriting components from renderable might not be a necessity at all, I
* can always have a special handling of lens flares in a seperate insert method for the queue, but I must analyze what work
* needs to be done in common between all renderables and the lens flare components. A good start is to analyze the render
* method of this LensFlare class and break it down into steps on paper. Then I shall determine who is responsible for each
* step.
*/

#include "LensFlare.h"

// Common Lib Includes
#include "StringUtility.h"
#include "Exception.h"

// Standard Includes
#include <algorithm>

//---------------------------------------------------------------------------
LensFlare::LensFlare(ID3D10Device & device, 
                     InputLayoutManager & inputLayoutManager, 
                     TextureManager & textureManager,
                     EffectManager & effectManager,
                     Position lightPosition,
                     const std::string & glowTextureFilePath,
                     const std::string & flare1TextureFilePath,
                     const std::string & flare2TextureFilePath,
                     const std::string & flare3TextureFilePath)
    :
    m_querySize(100.0f),
    m_glowSize(400.0f),
    m_device(device),
    m_inputLayoutManager(inputLayoutManager),
    m_textureManager(textureManager),
    m_effectManager(effectManager),
    m_lightPosition(lightPosition),
    m_occlusionQuery(NULL),
    m_occlusionQueryActive(false)
{
    // Load the textures
    std::string flare1TextureName;
    std::string flare2TextureName;
    std::string flare3TextureName;

    RemoveExtFromFilename(glowTextureFilePath, m_glowTextureName);
    RemoveExtFromFilename(flare1TextureFilePath, flare1TextureName);
    RemoveExtFromFilename(flare2TextureFilePath, flare2TextureName);
    RemoveExtFromFilename(flare3TextureFilePath, flare3TextureName);

    m_textureManager.CreateTextureFromFile(m_glowTextureName, glowTextureFilePath);
    m_textureManager.CreateTextureFromFile(flare1TextureName, flare1TextureFilePath);
    m_textureManager.CreateTextureFromFile(flare2TextureName, flare2TextureFilePath);
    m_textureManager.CreateTextureFromFile(flare3TextureName, flare3TextureFilePath);

    // Create the effects
    Technique * occlusionTechnique = NULL;
    Technique * glowTechnique      = NULL;
    Technique * flareTechnique     = NULL;
   
    try
    {
        m_effect           = &(m_effectManager.CreateChildEffect("LensFlare", "LensFlare.fx"));
      
        occlusionTechnique = &(m_effect->GetTechnique("OcclusionQuery"));
        m_occlusionPass    = &(occlusionTechnique->GetPass(0));

        glowTechnique      = &(m_effect->GetTechnique("RenderGlow"));
        m_glowPass         = &(glowTechnique->GetPass(0));

        flareTechnique     = &(m_effect->GetTechnique("RenderFlare"));
        m_flarePass        = &(flareTechnique->GetPass(0));
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create materials
    try
    {
        // Occlusion Query material
        m_occlusionMaterial = m_effect->CreateMaterial();
        m_occlusionMaterial->SetFloat4("diffuseColor", static_cast<D3DXVECTOR4>(D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f)));

        // Glow material properties are dynamic and will be set as it is rendered
        m_glowMaterial = m_effect->CreateMaterial();

        // Flare material properties are dynamic and will be set as they are rendered
        m_flareMaterial = m_effect->CreateMaterial();
    }
    catch(Common::Exception & e)
    {
        throw e;
    }

    // Create an occlusion query rectangle shape
    //
    // NOTE - vertices are defined in counter clockwise order, instead of clockwise,
    //        because the projection into 2D screen space flips the quad in the Y
    //        direction and it would get culled as a backface if defined normally
    std::vector<Position>   quadPositions;
    std::vector<TexCoord2D> quadTexCoords;

    quadPositions.push_back(Position( -0.5f, -0.5f, 0.0f));
    quadPositions.push_back(Position(  0.5f, -0.5f, 0.0f));
    quadPositions.push_back(Position( -0.5f,  0.5f, 0.0f));
    quadPositions.push_back(Position(  0.5f,  0.5f, 0.0f));

    quadTexCoords.push_back(TexCoord2D( 0.0f,  0.0f));
    quadTexCoords.push_back(TexCoord2D( 1.0f,  0.0f));
    quadTexCoords.push_back(TexCoord2D( 0.0f,  1.0f));
    quadTexCoords.push_back(TexCoord2D( 1.0f,  1.0f));
   
    m_positionBuffer = Buffer::SharedPtr(new Buffer(m_device, POSITION,   quadPositions, false));
    m_texCoordBuffer = Buffer::SharedPtr(new Buffer(m_device, TEXCOORD2D, quadTexCoords, false));

    // Create input layouts
    std::vector<InputElementDescription> inputElementDescs;
    std::vector<InputElementDescription> thisElementDesc;

    InputElementDescription::GetInputElementDesc(thisElementDesc,
                                                m_positionBuffer->GetContentType(), 
                                                0,
                                                0,
                                                m_positionBuffer->IsPerInstance());

    inputElementDescs.insert(inputElementDescs.end(), thisElementDesc.begin(), thisElementDesc.end());

    m_occlusionInputLayout = m_inputLayoutManager.GetInputLayout(inputElementDescs, *m_occlusionPass);

    InputElementDescription::GetInputElementDesc(thisElementDesc,
                                                m_texCoordBuffer->GetContentType(), 
                                                0,
                                                1,
                                                m_texCoordBuffer->IsPerInstance());

    inputElementDescs.insert(inputElementDescs.end(), thisElementDesc.begin(), thisElementDesc.end());

    m_glowInputLayout  = m_inputLayoutManager.GetInputLayout(inputElementDescs, *m_glowPass);
    m_flareInputLayout = m_inputLayoutManager.GetInputLayout(inputElementDescs, *m_flarePass);

    // Create the occlusion query
    D3D10_QUERY_DESC  queryDesc;
    queryDesc.MiscFlags = 0;
    queryDesc.Query     = D3D10_QUERY_OCCLUSION;
    if( FAILED(m_device.CreateQuery(&queryDesc, &m_occlusionQuery)) )
    {
        const std::string msg("Could not create occlusion query");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Init the flare circle datas
    float flare1Scale = static_cast<float>(m_textureManager.GetTexture(flare1TextureName).GetWidth());
    float flare2Scale = static_cast<float>(m_textureManager.GetTexture(flare2TextureName).GetWidth());
    float flare3Scale = static_cast<float>(m_textureManager.GetTexture(flare3TextureName).GetWidth());

    m_flares.push_back(Flare(-0.5f, flare1Scale * 0.7f, D3DXCOLOR( 0.196078f, 0.019608f, 0.196078f, 1.0f), flare1TextureName));
    m_flares.push_back(Flare( 0.3f, flare1Scale * 0.4f, D3DXCOLOR( 0.392156f, 1.000000f, 0.784314f, 1.0f), flare1TextureName));
    m_flares.push_back(Flare( 1.2f, flare1Scale * 1.0f, D3DXCOLOR( 0.392156f, 0.196078f, 0.196078f, 1.0f), flare1TextureName));
    m_flares.push_back(Flare( 1.5f, flare1Scale * 1.5f, D3DXCOLOR( 0.196078f, 0.392156f, 0.196078f, 1.0f), flare1TextureName));

    m_flares.push_back(Flare(-0.3f, flare2Scale * 0.7f, D3DXCOLOR( 0.784314f, 0.196078f, 0.196078f, 1.0f), flare2TextureName));
    m_flares.push_back(Flare( 0.6f, flare2Scale * 0.9f, D3DXCOLOR( 0.196078f, 0.392156f, 0.196078f, 1.0f), flare2TextureName));
    m_flares.push_back(Flare( 0.7f, flare2Scale * 0.4f, D3DXCOLOR( 0.196078f, 0.784314f, 0.784314f, 1.0f), flare2TextureName));

    m_flares.push_back(Flare(-0.7f, flare3Scale * 0.7f, D3DXCOLOR( 0.196078f, 0.392156f, 0.019608f, 1.0f), flare3TextureName));
    m_flares.push_back(Flare( 0.0f, flare3Scale * 0.6f, D3DXCOLOR( 0.019608f, 0.019608f, 0.019608f, 1.0f), flare3TextureName));
    m_flares.push_back(Flare( 2.0f, flare3Scale * 1.4f, D3DXCOLOR( 0.019608f, 0.196078f, 0.392156f, 1.0f), flare3TextureName));
}


//---------------------------------------------------------------------------
LensFlare::~LensFlare()
{
   // Release the occlusion query
   if( m_occlusionQuery )
   {
      m_occlusionQuery->Release();
   }
}

//---------------------------------------------------------------------------
void LensFlare::Render()
{
   // Get the original matrices
   D3DXMATRIX origProjection;
   D3DXMATRIX origView;

   m_effectManager.GetViewMatrix(origView);
   m_effectManager.GetProjectionMatrix(origProjection);

   // Get the viewport
   UINT           numViewports = 1;
   D3D10_VIEWPORT viewport;

   m_device.RSGetViewports(&numViewports, &viewport);

   D3DXVECTOR2 screenCenter;
   screenCenter.x = static_cast<float>(viewport.Width)  / 2.0f;
   screenCenter.y = static_cast<float>(viewport.Height) / 2.0f;

   // Create new view matrix with the camera at the origin for projection of the position into the background
   D3DXMATRIX newView = origView;
   newView._41 = newView._42 = newView._43 = 0.0f;
   newView._44 = 1.0f;

   // Project the light position into 2D screen space
   D3DXMATRIX identity;
   D3DXMatrixIdentity(&identity);

   D3DXVECTOR3 projectedPosition;
   D3DXVec3Project(&projectedPosition, &m_lightPosition, &viewport, &origProjection, &newView, &identity);

   // Don't draw any flares if the light is behind the camera
   if( (projectedPosition.z < 0.0f) || (projectedPosition.z > 1.0f) )
   {
      return;
   }

   D3DXVECTOR3 lightPosition(projectedPosition.x, projectedPosition.y, projectedPosition.z);

   // Set up matrices to render in screen space
   D3DXMatrixIdentity(&newView);
   m_effectManager.SetViewMatrix(newView);

   D3DXMATRIX newProjection;
   D3DXMatrixOrthoOffCenterLH(&newProjection,
                              0.0f,
                              static_cast<float>(viewport.Width),
                              static_cast<float>(viewport.Height),
                              0.0f,
                              0.0f,
                              1.0f);

   m_effectManager.SetProjectionMatrix(newProjection);

   // Check whether the light is occluded by the scene
   UpdateOcclusion(lightPosition);

   // If it is visible, draw the flare
   if( m_occlusionAlpha > 0 )
   {
      RenderGlow(D3DXVECTOR2(lightPosition.x, lightPosition.y));
      RenderFlares(D3DXVECTOR2(lightPosition.x, lightPosition.y), screenCenter);
   }

   // Restore Matrices
   m_effectManager.SetViewMatrix(origView);
   m_effectManager.SetProjectionMatrix(origProjection);
}
 
//---------------------------------------------------------------------------
void LensFlare::UpdateOcclusion(const D3DXVECTOR3 & lightPosition)
{
   if( m_occlusionQueryActive )
   {
      // If the previous query has not yet completed, wait until it does
      UINT64 queryData;

      if( S_OK != m_occlusionQuery->GetData(&queryData, sizeof(UINT64), 0) )
      {
         return;
      }

      // Use the occlusion query pixel count to work out what percentage of the sun is visible
      const float queryArea = m_querySize * m_querySize;
      m_occlusionAlpha = std::min<float>(static_cast<float>(queryData) / queryArea, 1.0f);
   }

   // Set the world matrix and material
   D3DXMATRIX matScale;
   D3DXMATRIX matTranslation;
   D3DXMATRIX world;

   D3DXMatrixScaling(&matScale, m_querySize, m_querySize, 1.0f);
   D3DXMatrixTranslation(&matTranslation, lightPosition.x, lightPosition.y, lightPosition.z);
   D3DXMatrixMultiply(&world, &matScale, &matTranslation);
   
   try
   {
      m_effect->SetWorldMatrix(world);
      m_effect->SetMaterial(*m_occlusionMaterial);
   }
   catch(Common::Exception & e)
   {
      throw e;
   }

   // Bind the input layout
   m_device.IASetInputLayout(m_occlusionInputLayout);
   m_device.IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

   // Bind the vertex buffers
   ID3D10Buffer * buffer = m_positionBuffer->GetD3DBuffer();
   UINT           stride = GetStride(m_positionBuffer->GetContentType());
   UINT           offset = 0;
   
   m_device.IASetVertexBuffers(0,
                               1,
                               &buffer, 
                               &stride, 
                               &offset);

   // Apply the pass
   m_occlusionPass->Apply();

   // Issue the occlusion query
   m_occlusionQuery->Begin();
   m_device.Draw(4, 0);
   m_occlusionQuery->End();

   // Flag the occlusion query as active
   m_occlusionQueryActive = true;
}

//---------------------------------------------------------------------------
void LensFlare::RenderGlow(const D3DXVECTOR2 & lightPosition)
{
   // Set the world matrix and material
   D3DXMATRIX matScale;
   D3DXMATRIX matTranslation;
   D3DXMATRIX world;

   D3DXMatrixScaling(&matScale, m_glowSize, m_glowSize, 1.0f);
   D3DXMatrixTranslation(&matTranslation, lightPosition.x, lightPosition.y, 0.0f);
   D3DXMatrixMultiply(&world, &matScale, &matTranslation);
   
   m_glowMaterial->SetFloat4("diffuseColor", D3DXVECTOR4(1.0f, 1.0f, 1.0f, m_occlusionAlpha));
   m_glowMaterial->SetTextureName("diffuseTexture", m_glowTextureName);

   try
   {
      m_effect->SetWorldMatrix(world);
      m_effect->SetMaterial(*m_glowMaterial);
   }
   catch(Common::Exception & e)
   {
      throw e;
   }

   // Bind the input layout
   m_device.IASetInputLayout(m_glowInputLayout);
   m_device.IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

   // Bind the vertex buffers
   ID3D10Buffer * buffers[2] = {m_positionBuffer->GetD3DBuffer(), 
                                m_texCoordBuffer->GetD3DBuffer()};
   
   UINT strides[2] = {GetStride(m_positionBuffer->GetContentType()),
                      GetStride(m_texCoordBuffer->GetContentType())};
   
   UINT offsets[2] = {0, 
                      0};
   
   m_device.IASetVertexBuffers(0,
                               2,
                               buffers, 
                               strides, 
                               offsets);

   // Apply the pass
   m_glowPass->Apply();

   // Draw the glow
   m_device.Draw(4, 0);
}

//---------------------------------------------------------------------------
void LensFlare::RenderFlares(const D3DXVECTOR2 & lightPosition, const D3DXVECTOR2 & screenCenter)
{
   D3DXVECTOR2 flareVector = screenCenter - lightPosition;
   
   // Bind the input layout
   m_device.IASetInputLayout(m_flareInputLayout);
   m_device.IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

   // Bind the vertex buffers
   ID3D10Buffer * buffers[2] = {m_positionBuffer->GetD3DBuffer(), 
                                m_texCoordBuffer->GetD3DBuffer()};
   
   UINT strides[2] = {GetStride(m_positionBuffer->GetContentType()),
                      GetStride(m_texCoordBuffer->GetContentType())};
   
   UINT offsets[2] = {0, 
                      0};
   
   m_device.IASetVertexBuffers(0,
                               2,
                               buffers, 
                               strides, 
                               offsets);

   for( std::vector<Flare>::iterator it = m_flares.begin(); it != m_flares.end(); ++it)
   {
      // Set the world matrix and material
      D3DXMATRIX matScale;
      D3DXMATRIX matTranslation;
      D3DXMATRIX world;

      D3DXVECTOR2 flarePosition = lightPosition + flareVector * it->m_position;
      D3DXMatrixScaling(&matScale, it->m_scale, it->m_scale, 1.0f);
      D3DXMatrixTranslation(&matTranslation, flarePosition.x, flarePosition.y, 0.0f);
      D3DXMatrixMultiply(&world, &matScale, &matTranslation);

      D3DXCOLOR flareColor = it->m_color;
      flareColor.a *= m_occlusionAlpha; 
      m_flareMaterial->SetFloat4("diffuseColor", static_cast<D3DXVECTOR4>(flareColor));
      m_flareMaterial->SetTextureName("diffuseTexture", it->m_textureName);
      
      try
      {
         m_effect->SetWorldMatrix(world);
         m_effect->SetMaterial(*m_flareMaterial);
      }
      catch(Common::Exception & e)
      {
         throw e;
      }

      // Apply the pass
      m_flarePass->Apply();

      // Draw the flare
      m_device.Draw(4, 0);
   }

/*
   Viewport viewport = GraphicsDevice.Viewport;

   // Lensflare sprites are positioned at intervals along a line that
   // runs from the 2D light position toward the center of the screen.
   Vector2 screenCenter = new Vector2(viewport.Width, viewport.Height) / 2;
   
   Vector2 flareVector = screenCenter - lightPosition;

   // Draw the flare sprites using additive blending.
   spriteBatch.Begin(SpriteBlendMode.Additive);

   foreach (Flare flare in flares)
   {
       // Compute the position of this flare sprite.
       Vector2 flarePosition = lightPosition + flareVector * flare.Position;

       // Set the flare alpha based on the previous occlusion query result.
       Vector4 flareColor = flare.Color.ToVector4();

       flareColor.W *= occlusionAlpha;

       // Center the sprite texture.
       Vector2 flareOrigin = new Vector2(flare.Texture.Width,
                                         flare.Texture.Height) / 2;

       // Draw the flare.
       spriteBatch.Draw(flare.Texture, flarePosition, null,
                        new Color(flareColor), 1, flareOrigin,
                        flare.Scale, SpriteEffects.None, 0);
   }

   spriteBatch.End();
*/
}

//---------------------------------------------------------------------------
LensFlare::Flare::Flare(float position, float scale, const D3DXCOLOR & color, const std::string & textureName)
   :
m_position(position),
m_scale(scale),
m_color(color),
m_textureName(textureName)
{         
}