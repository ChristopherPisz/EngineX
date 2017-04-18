
#ifndef RENDERQUEUE_H
#define RENDERQUEUE_H


// EngineX Includes
#include "Renderable.h"
#include "LensFlare.h"

// Standard Includes
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
/**
* Contains, sorts, and render a collection of objects to rendered
*/
class RenderQueue
{
public:

   /**
   * Constructor
   */
   RenderQueue(EffectManager & effectManager);

   /**
   * Deconstructor
   */
   virtual ~RenderQueue();


   /**
   * Add an object to be rendered
   */
   virtual void Insert(Renderable * renderable);
   
   /**
   * Remove an object from being rendered
   */
   virtual void Remove(Renderable * renderable);


   /**
   * Add a lens flare to be rendered
   */
   virtual void Insert(LensFlare * lensFlare);
    
   /**
   * Remove a lens flare from being rendered
   */
   virtual void Remove(LensFlare * lensFlare);


   /**
   * Sorts and Renders the objects in the queue
   **/
   virtual void Render();

protected:

   /**
   * Sorts all contained objects in thier individual queues
   **/
   virtual void Sort();

   /**
   * Sorting criteria for opaque type renderables
   **/
   struct SortCriteria_Opaque
   {
      bool operator () (const Renderable * lhs,
                        const Renderable * rhs) const;
   };

   /**
   * Sorting criteria for opaque type renderables
   **/
   struct SortCriteria_Transparent
   {
      SortCriteria_Transparent(const D3DXMATRIX & view);

      bool operator () (Renderable * lhs,
                        Renderable * rhs) const;
      
      const D3DXMATRIX m_view;
   };

   EffectManager &           m_effectManager;                             // Contains and creates effects and techniques 
   std::vector<Renderable *> m_renderables[Renderable::NUM_RENDER_TYPES]; // Renderables seperated by type
   std::vector<LensFlare *>  m_lensFlares;                                // LensFlares 
};

#endif