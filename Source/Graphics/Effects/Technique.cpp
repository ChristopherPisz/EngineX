
#include "Technique.h"

// Common Lib Includes
#include "Exception.h"

// Standard Includes
#include <sstream>

//----------------------------------------------------------------------------
Technique::Technique(ID3D10EffectTechnique * technique)
    :
    m_technique(technique)
{
    // Get the technique's description
    D3D10_TECHNIQUE_DESC techniqueDesc;
    technique->GetDesc(&techniqueDesc);
   
    m_name = techniqueDesc.Name;
   
    // Create passes
    unsigned numPasses = techniqueDesc.Passes;
   
    for(unsigned i = 0; i < numPasses; ++i)
    {
        ID3D10EffectPass * d3dPass = m_technique->GetPassByIndex(i);
      
        Pass * pass = new Pass(d3dPass);
        m_passes.push_back(pass);
    }
}

//----------------------------------------------------------------------------
Technique::~Technique()
{
    for(Passes::iterator it = m_passes.begin(); it != m_passes.end(); ++it)
    {
        delete (*it);
    }
}

//----------------------------------------------------------------------------
const std::string & Technique::GetName() const
{
    return m_name;
}

//----------------------------------------------------------------------------
const unsigned Technique::GetNumPasses() const
{
    return static_cast<unsigned>(m_passes.size());
}

//----------------------------------------------------------------------------
Pass & Technique::GetPass(const unsigned passIndex)
{
    if( passIndex > m_passes.size() -1 )
    {
        std::stringstream msg;
        msg << "Technique does not contain pass: " << passIndex;

        throw Common::Exception(__FILE__, __LINE__, msg.str());
    }

    return *(m_passes[passIndex]);
}

