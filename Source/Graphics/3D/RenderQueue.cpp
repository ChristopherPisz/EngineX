

#include "RenderQueue.h"


// Common Lib Includes
#include "Exception.h"

// Standard Includes
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
RenderQueue::RenderQueue(EffectManager & effectManager)
   :
   m_effectManager(effectManager)
{
}

//----------------------------------------------------------------------------------------------------------------------
RenderQueue::~RenderQueue()
{
}

//----------------------------------------------------------------------------------------------------------------------
void RenderQueue::Insert(Renderable * renderable)
{
    if( !renderable )
    {
        const std::string msg("Renderable is NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    Renderable::RenderType renderType = renderable->GetRenderType();

    if( renderType > Renderable::NUM_RENDER_TYPES  || 
        renderType < Renderable::RENDERTYPE_OPAQUE )
    {
        const std::string msg("Unknown render type");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if the renderable is already in a queue
    std::vector<Renderable *>::iterator it = std::find(m_renderables[renderType].begin(),
                                                        m_renderables[renderType].end(),
                                                        renderable);

    if( it != m_renderables[renderType].end() )
    {
        return;
    }

    // Insert the renderable
    m_renderables[renderType].push_back(renderable);
}

//----------------------------------------------------------------------------------------------------------------------
void RenderQueue::Remove(Renderable * renderable)
{
    Renderable::RenderType renderType = renderable->GetRenderType();

    if( renderType > Renderable::NUM_RENDER_TYPES  || 
        renderType < Renderable::RENDERTYPE_OPAQUE )
    {
        const std::string msg("Unknown render type");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if the renderable is already in a queue
    std::vector<Renderable *>::iterator it = std::find(m_renderables[renderType].begin(),
                                                        m_renderables[renderType].end(),
                                                        renderable);

    // Remove the renderable from the queue
    if( it != m_renderables[renderType].end() )
    {
        m_renderables[renderType].erase(it);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderQueue::Insert(LensFlare * lensFlare)
{
    if( !lensFlare )
    {
        const std::string msg("LensFlare is NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if the lensFlare is already in a queue
    std::vector<LensFlare *>::iterator it = std::find(m_lensFlares.begin(),
                                                        m_lensFlares.end(),
                                                        lensFlare);

    if( it != m_lensFlares.end() )
    {
        return;
    }

    // Insert the renderable
    m_lensFlares.push_back(lensFlare);
}

//----------------------------------------------------------------------------------------------------------------------
void RenderQueue::Remove(LensFlare * lensFlare)
{   
    if( !lensFlare )
    {
        const std::string msg("LensFlare is NULL");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }

    // Check if the renderable is already in a queue
    std::vector<LensFlare *>::iterator it = std::find(m_lensFlares.begin(),
                                                        m_lensFlares.end(),
                                                        lensFlare);

    // Remove the renderable from the queue
    if( it != m_lensFlares.end() )
    {
        m_lensFlares.erase(it);
    }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderQueue::Render()
{
    // Sort all contained objects
    Sort();

    // Render the renderables
    for(int i = 0; i < Renderable::NUM_RENDER_TYPES; ++i)
    {
        if( m_renderables[i].empty() )
        {
            continue;
        }

        for(std::vector<Renderable *>::iterator it = m_renderables[i].begin(); it != m_renderables[i].end(); ++it)
        {
            (*it)->Render();
        }
    }

    // Render the Lens Flares
    if( !m_lensFlares.empty() )
    {
        for(std::vector<LensFlare *>::iterator it = m_lensFlares.begin(); it != m_lensFlares.end(); ++it)
        {
            (*it)->Render();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderQueue::Sort()
{
    // Sort the Opaque renderables according to thier sorting criteria
    std::sort(m_renderables[Renderable::RENDERTYPE_OPAQUE].begin(),
              m_renderables[Renderable::RENDERTYPE_OPAQUE].end(), 
              SortCriteria_Opaque());

    // Sort the Transparent renderables according to thier sorting criteria
    D3DXMATRIX view;
    D3DXMATRIX projection;

    m_effectManager.GetViewMatrix(view);

    std::sort(m_renderables[Renderable::RENDERTYPE_TRANSPARENT].begin(),
              m_renderables[Renderable::RENDERTYPE_TRANSPARENT].end(), 
              SortCriteria_Transparent(view));
   
    // TODO - Sort Lens Flares

    // TODO - Not completed


}

//----------------------------------------------------------------------------------------------------------------------
bool RenderQueue::SortCriteria_Opaque::operator() (const Renderable * lhs, const Renderable * rhs) const
{
    // Sort by the effect the renderable is using
    std::string lhs_effectName;
    std::string lhs_techniqueName;
    std::string rhs_effectName;
    std::string rhs_techniqueName;

    lhs->GetEffectName(lhs_effectName, lhs_techniqueName);
    rhs->GetEffectName(rhs_effectName, rhs_techniqueName);

    if( lhs_effectName < rhs_effectName )
    {
        return true;
    }

    // If both renderables are using the same effect, sort by technique
    else if( lhs_effectName == rhs_effectName &&
            lhs_techniqueName <  rhs_techniqueName )
    {
        return true;
    }
   
    return false;
}

//----------------------------------------------------------------------------------------------------------------------
RenderQueue::SortCriteria_Transparent::SortCriteria_Transparent(const D3DXMATRIX & view)
    :
    m_view(view)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool RenderQueue::SortCriteria_Transparent::operator() (Renderable * lhs, Renderable * rhs) const
{  
    // Sort renderables by thier z coordinate in camera space
    D3DXMATRIX worldView;
    float z_lhs;
    float z_rhs;

    D3DXMatrixMultiply(&worldView, &(lhs->GetTransform()), &m_view);
    z_lhs = lhs->GetPosition().z;

    D3DXMatrixMultiply(&worldView, &(rhs->GetTransform()), &m_view);
    z_rhs = rhs->GetPosition().z;
   
    if( z_lhs < z_rhs )
    {
        return true;
    }

    return false;
}