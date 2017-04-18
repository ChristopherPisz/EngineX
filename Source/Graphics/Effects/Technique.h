#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include "Pass.h"

// DirectX Includes
#include <d3d10.h>
#include <dxgi.h>
#include <d3dx10.h>

// Standard Includes
#include <vector>

class Effect;

//----------------------------------------------------------------------------
class Technique
{
public:

   /** Only an Effect should construct a Technique */
   friend class Effect;

   /**
   * Desconstructor
   **/
   ~Technique();



   /**
   * Gets the name of this technique 
   **/
   const std::string & GetName() const;



   /**
   * Gets the number of pass this technique contains
   **/
   const unsigned GetNumPasses() const;

   /**
   * Gets a pass from the technique
   **/
   Pass & GetPass(const unsigned passIndex);

private:

   /**
   * Constructor
   *
   * Only an Effect should construct a Technique
   **/
   Technique(ID3D10EffectTechnique * technique);

   /** No copy allowed */
   Technique(const Technique & rhs);

   /** No assignment allowed */
   Technique & operator = (const Technique & rhs);



   ID3D10EffectTechnique * m_technique;
   
   std::string m_name;

   typedef std::vector<Pass *> Passes;
   Passes m_passes;
};

#endif // TECHNIQUE_H