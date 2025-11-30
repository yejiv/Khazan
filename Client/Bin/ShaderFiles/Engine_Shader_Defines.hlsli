#include "Engine_Shader_Function.hlsli"

SamplerComparisonState ComparisonSampler
{
    filter = comparison_min_mag_linear_mip_point;
    AddressU = border;
    AddressV = border;
    BorderColor = float4(1.f, 1.f, 1.f, 1.f);
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

sampler PointClampSampler = sampler_state
{
    filter = min_mag_mip_point;
    AddressU = clamp;
    AddressV = clamp;
};

sampler BorderSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = border;
    AddressV = border;
    BorderColor = float4(0.f, 0.f, 0.f, 0.f);
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
    DepthEnable = true;     // 깊이 테스트 O
    DepthWriteMask = all;   // 깊이 기록 O
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None
{
    DepthEnable = false;    // 깊이 테스트 X
    DepthWriteMask = zero;  // 깊이 기록 X
};

DepthStencilState DSS_DepthTestOnly
{
    DepthEnable = true;     // 깊이 테스트 O
    DepthWriteMask = zero;  // 깊이 기록 X
    DepthFunc = LESS_EQUAL; 
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


BlendState BS_WeightBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend[0] = ONE;
    DestBlend[0] = ONE;
    BlendOp[0] = Add;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ONE;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 0x0F;

    SrcBlend[1] = zero;
    DestBlend[1] = INV_SRC_COLOR;
    BlendOp[1] = Add;
    SrcBlendAlpha[1] = zero;
    DestBlendAlpha[1] = INV_SRC_ALPHA;
    BlendOpAlpha[1] = Add;
    RenderTargetWriteMask[1] = 0x0F; // 전체 기록
};
