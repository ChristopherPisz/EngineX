

// Project Includes
#include "Pass.h"

// Common Lib Includes
#include "Exception.h"

//----------------------------------------------------------------------------
Pass::Pass(ID3D10EffectPass * pass)
    :
    m_pass(pass)
{
    // Get the pass description
    D3D10_PASS_DESC passDesc;
    m_pass->GetDesc(&passDesc);

    // Get the types of vertex buffers this pass expects
    D3D10_PASS_SHADER_DESC passShaderDesc;
    pass->GetVertexShaderDesc(&passShaderDesc);

    ID3D10EffectShaderVariable * shaderVariable = passShaderDesc.pShaderVariable;
    unsigned shaderIndex                        = passShaderDesc.ShaderIndex;

    D3D10_EFFECT_SHADER_DESC effectShaderDesc;
    shaderVariable->GetShaderDesc(shaderIndex, &effectShaderDesc);

    for(unsigned i = 0; i < effectShaderDesc.NumInputSignatureEntries; ++i)
    {
        D3D10_SIGNATURE_PARAMETER_DESC signatureParamDesc;
        shaderVariable->GetInputSignatureElementDesc(shaderIndex, i, &signatureParamDesc);
      
        m_sigParamDescs.push_back(signatureParamDesc);
    }
}

//----------------------------------------------------------------------------
Pass::~Pass()
{
}

//----------------------------------------------------------------------------
const D3D10_PASS_DESC Pass::GetPassDesc() const
{
    D3D10_PASS_DESC passDesc;
    m_pass->GetDesc(&passDesc);

    return passDesc;
}

//----------------------------------------------------------------------------
const std::vector<D3D10_SIGNATURE_PARAMETER_DESC> & Pass::GetSignatureParamDescs() const
{
    return m_sigParamDescs;
}

//----------------------------------------------------------------------------
void Pass::Apply()
{
    if( FAILED(m_pass->Apply(0)) )
    {
        const std::string msg("Failed to apply pass.");
        throw Common::Exception(__FILE__, __LINE__, msg);
    }
}