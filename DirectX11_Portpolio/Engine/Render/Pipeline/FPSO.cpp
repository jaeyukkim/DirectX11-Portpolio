#include "HeaderCollection.h"
#include "FPSO.h"

void FPSO::operator=(const FPSO& pso)
{
    m_vertexShader = pso.m_vertexShader;
    m_pixelShader = pso.m_pixelShader;
    m_hullShader = pso.m_hullShader;
    m_domainShader = pso.m_domainShader;
    m_geometryShader = pso.m_geometryShader;
    m_inputLayout = pso.m_inputLayout;
    m_blendState = pso.m_blendState;
    m_depthStencilState = pso.m_depthStencilState;
    m_rasterizerState = pso.m_rasterizerState;
    m_stencilRef = pso.m_stencilRef;
    m_blendFactor = pso.m_blendFactor;
    m_primitiveTopology = pso.m_primitiveTopology;
}

void FPSO::SetBlendFactor(const array<float, 4> InBlendFactor)
{
    m_blendFactor = InBlendFactor;
}
