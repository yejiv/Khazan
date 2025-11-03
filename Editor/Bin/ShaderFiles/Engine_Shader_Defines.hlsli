#include "Engine_Shader_Function.hlsli"

SamplerComparisonState ComparisonSampler
{
    filter = comparison_min_mag_linear_mip_point;
    AddressU = clamp;
    AddressV = clamp;
    ComparisonFunc = less;
};

sampler ClampSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = clamp;
    AddressV = clamp;
};

sampler DefaultSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointSampler = sampler_state
{
    filter = min_mag_mip_point;
    AddressU = wrap;
    AddressV = wrap;
};

sampler SkySampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = wrap;
    AddressV = clamp;
};

RasterizerState RS_Wireframe
{
    FillMode = wireframe;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    CullMode = front;
};

RasterizerState RS_Cull_None
{
    CullMode = none;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

DepthStencilState DSS_Outline
{
    DepthEnable = true;         // 깊이 테스트 활성화
    DepthWriteMask = zero;      // 깊이 기록 비활성화
    DepthFunc = LESS_EQUAL;     // 기본 깊이 함수
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = ONE;
    DestBlend = ONE;
    BlendOp = Add;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = Add;
};
