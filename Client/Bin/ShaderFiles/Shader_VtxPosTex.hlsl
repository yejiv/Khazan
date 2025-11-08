
#include "Engine_Shader_Defines.hlsli"



/*
VTXPOSTEX
LPCSTR SemanticName;
UINT SemanticIndex;
DXGI_FORMAT Format;
UINT InputSlot;
UINT AlignedByteOffset;
D3D11_INPUT_CLASSIFICATION InputSlotClass;
UINT InstanceDataStepRate;
*/

/*
D3D11_INPUT_ELEMENT_DESC Elements[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
*/

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_DepthTexture;
float4 g_vColor;
float2 g_ViewportSize;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

};

/* 정점쉐이더 : 정점 위치의 스페이스 변환(로컬 -> 월드 -> 뷰 -> 투영). */
/*          : 정점의 구성을 변경.(in:3개, out:2개 or 5개) */
/*          : 정점 단위(정점 하나당 VS_MAIN한번호출) */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    /* 정점의 로컬위치 * 월드 * 뷰 * 투영 */

    float4x4 matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

VS_OUT VS_TRAIL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = float4(In.vPosition, 1.f);
    Out.vProjPos = Out.vPosition;

    return Out;
}

VS_OUT VS_SCREEN_TRAIL(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    // 스크린 좌표 -> 클립 공간(-1 ~ +1)으로 변환
    Out.vPosition.x = (In.vPosition.x / g_ViewportSize.x) * 2.0f - 1.0f;
    Out.vPosition.y = (In.vPosition.y / g_ViewportSize.y) * -2.0f + 1.0f;
    Out.vPosition.z = In.vPosition.z;
    Out.vPosition.w = 1.0f;

    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* /W을 수행한다. 투영스페이스로 변환 */
/* 뷰포트로 변환하고.*/
/* 래스터라이즈 : 픽셀을 만든다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;

};

/* 만든 픽셀 각각에 대해서 픽셀 쉐이더를 수행한다. */
/* 픽셀의 색을 결정한다. */


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor = g_vColor;

    return Out;
}

PS_OUT PS_MAIN_BLEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);

    float2 vTexcoord;

    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);



    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);

    return Out;
}


PS_OUT PS_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    //Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    
    vector vEffectTexture = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    //vector vFinalColor = float4(vEffectTexture.rgb, vEffectTexture.r);
    vector vFinalColor = float4(1.f, 1.f, 1.f, vEffectTexture.r);
    Out.vColor = vFinalColor;

    //alpha fading
    Out.vColor.a *= In.vTexcoord.x;

    //소프트 파티클 효과

    //float2 vTexcoord;
    //
    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    //vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, vTexcoord);
    //
    //Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);

    return Out;
}

technique11 DefaultTechnique
{
    /* 특정 패스를 이용해서 점정을 그려냈다. */
    /* 하나의 모델을 그려냈다. */
    /* 모델의 상황에 따라 다른 쉐이딩 기법 세트(명암 + 림라이트 + 스펙큘러 + 노멀맵 + ssao )를 먹여주기위해서 */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }


    pass DefaultPass1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLEND();

    }

    pass TrailPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_TRAIL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    pass ScreenTrailPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SCREEN_TRAIL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    ///* 정점의 정보에 따라 쉐이더 파일을 작성한다. */
    ///* 정점의 정보가 같지만 완전히 다른 취급을 하느 ㄴ객체나 모델을 그리는 방식 -> 렌더링방식에 차이가 생길 수 있다. */ 
    //pass DefaultPass1
    //{
    //    VertexShader = compile vs_5_0 VS_MAIN1();

    //}

}
