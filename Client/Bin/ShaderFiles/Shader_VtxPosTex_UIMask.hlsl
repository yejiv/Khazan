#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4 g_vColor, g_fProgressValue;
float g_fAlpha;
texture2D g_Texture;
texture2D g_MaskTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vLocalPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vLocalPos = float4(In.vPosition, 1.f);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vLocalPos : TEXCOORD0;
    float2 vTexcoord : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MASK_0(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x < 0.3f)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        Out.vColor.a *= g_fAlpha;
    }
    else
    {
        if (In.vTexcoord.x < 0.5f)
        {
            Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        
            float fAlpha = (In.vTexcoord.x - 0.3f) / (0.5f - 0.3f);
            fAlpha = clamp(fAlpha, 0.0f, 1.0f);
        
            Out.vColor.a = 1.f - fAlpha;
            Out.vColor.a *= g_vColor.a;
            Out.vColor.a *= g_fAlpha;
        }
        else
            discard;
    }
        return Out;
}

PS_OUT PS_MASK_1(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x < 0.3f || 0.7f < In.vTexcoord.x)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    }
    else if (In.vTexcoord.x <= 0.5f)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        
        float fAlpha = (In.vTexcoord.x - 0.3f) / (0.5f - 0.3f);
        fAlpha = clamp(1.f - fAlpha, 0.2f, 1.0f);
        
        Out.vColor.a = fAlpha ;
    }
    else if (In.vTexcoord.x <= 0.7f)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        
        float fAlpha = (In.vTexcoord.x - 0.5f) / (0.7f - 0.5f);
        fAlpha = clamp(fAlpha, 0.2f, 1.0f);
        
        Out.vColor.a = fAlpha;
    }
    Out.vColor.a *= g_vColor.a;
    Out.vColor.a *= g_fAlpha;
    return Out;
}

PS_OUT PS_MASK_2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x <= 0.25f )
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    }
    else if (In.vTexcoord.x <= 0.55f)
    {
        Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
        
        float fAlpha = (In.vTexcoord.x - 0.3f) / (0.5f - 0.3f);
        fAlpha = clamp(1.f - fAlpha, 0.f, 1.0f);
        
        Out.vColor.a = fAlpha;
    }
    else 
        discard;
    
    Out.vColor.a *= g_vColor.a;
    Out.vColor.a *= g_fAlpha;
    return Out;
}

PS_OUT PS_MASK_3(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x > 0.7f)
        discard;
    
    Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    
    if (In.vTexcoord.x > 0.5f)
        Out.vColor.a = clamp((0.7f - In.vTexcoord.x) / (0.7f - 0.5f), 0.f, 1.f);    
    
    Out.vColor.a = Out.vColor.a * g_vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_COLOR_BG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
        
    Out.vColor = g_vColor;
        
    Out.vColor.a = Out.vColor.a * g_fAlpha;
    return Out;
}

PS_OUT PS_MASK_BOTTOM(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.y < 0.4f)
        discard;
    
    //Out.vColor = g_Texture.Sample(ClampSampler, In.vTexcoord);
    Out.vColor.r = 0.f;
    Out.vColor.gb = 0.f;
    float fAlpha = (In.vTexcoord.y - 0.4f) / 0.6f;
    fAlpha = clamp(fAlpha, 0.0f, 1.f);
    
    Out.vColor.a = fAlpha * g_vColor.a * g_fAlpha;
    
    return Out;
}
technique11 DefaultTechnique
{
    pass PS_MASK_PASS_0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_0();
    }
    pass PS_MASK_PASS_1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_1();
    }
    pass PS_MASK_PASS_2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_2();
    }
    pass PS_MASK_PASS_3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_3();
    }
    pass PS_COLOR_BG_4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_BG();
    }
    pass PS_MASK_BOTTOM_5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MASK_BOTTOM();
    }

}