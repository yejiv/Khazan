#include "Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

}

HRESULT CRenderer::Initialize()
{
    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    m_fViewportWidth = ViewportDesc.Width;
    m_fViewportHeight = ViewportDesc.Height;

    /* For.Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
        return E_FAIL;

    /* For.Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* For.Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_World */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_World"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* For.Target_LightDepth */
    //  if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iMaxWidth, g_iMaxHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1.0f, 1.0f, 1.0f))))
    //      return E_FAIL;

    /* For.Target_BackBuffer */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BackBuffer"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* For.Target_BlurX */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* For.MRT_GameObjects : 게임 오브젝트들의 정보를 저장받기위한 타겟들 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_World"))))
        return E_FAIL;

    /* For.MRT_LightAcc : 빛들의 연산 결과를 누적한다. */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

    /* For.MRT_Shadow : 광원기준으로 보여지는 장면을 그려준다.  */
    //  if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
    //      return E_FAIL;

    /* For.MRT_BackBuffer : 원래 백버퍼에 그렸어야할 최종 장면을 그려놓는 타겟  */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BackBuffer"), TEXT("Target_BackBuffer"))))
        return E_FAIL;

    /* For.MRT_Blur : 원래 백버퍼에 그렸어야할 최종 장면을 그려놓는 타겟  */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_BlurX"))))
        return E_FAIL;

    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Shade"), 450.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_Specular"), 450.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    //  if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth"), ViewportDesc.Width - 300.0f, 300.0f, 600.f, 600.f)))
    //      return E_FAIL;
    //  if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth_0"), ViewportDesc.Width - 150.0f, 150.0f, 300.f, 300.f)))
    //      return E_FAIL;
    //  if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth_1"), ViewportDesc.Width - 150.0f, 450.0f, 300.f, 300.f)))
    //      return E_FAIL;
    //  if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("Target_LightDepth_2"), ViewportDesc.Width - 150.0f, 750.0f, 300.f, 300.f)))
    //      return E_FAIL;
#endif

    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
    if (nullptr == pRenderObject)
        return E_FAIL;

    m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);

    Safe_AddRef(pRenderObject);

    return S_OK;
}

HRESULT CRenderer::Draw()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;

#ifdef _DEBUG
    if (m_isEnableShadow)
        if (FAILED(Render_Shadow()))
            return E_FAIL;
#else
    if (FAILED(Render_Shadow()))
        return E_FAIL;
#endif

    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    if (FAILED(Render_Lights()))
        return E_FAIL;

    if (FAILED(Render_Combined()))
        return E_FAIL;

    //if (FAILED(Render_Blur()))
    //    return E_FAIL;

    if (FAILED(Render_NonLight()))
        return E_FAIL;

    if (FAILED(Render_Blend()))
        return E_FAIL;

    if (FAILED(Render_UI()))
        return E_FAIL;


#ifdef _DEBUG
    if (m_pGameInstance->Key_Down(DIK_PRIOR))
        m_isEnableDebugRender = true;
    else if (m_pGameInstance->Key_Down(DIK_NEXT))
        m_isEnableDebugRender = false;

    if (m_isEnableDebugRender)
        if (FAILED(Render_Debug()))
            return E_FAIL;
#endif

    return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
    m_DebugComponent.push_back(pComponent);

    Safe_AddRef(pComponent);

    return S_OK;
}
#endif

HRESULT CRenderer::Render_Priority()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BackBuffer"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
    {
        m_CascadeObjects.push_back(pRenderObject);
        Safe_AddRef(pRenderObject);
    }

    for (_uint i = 0; i < m_pGameInstance->Get_NumCascades(); ++i)
    {
        m_pGameInstance->Backup_RT();

        m_pGameInstance->Set_CurrentCascade(i);
    
        if (FAILED(SetUp_Viewport(g_iMaxWidth, g_iMaxHeight)))
            return E_FAIL;

        m_pGameInstance->Bind_ShadowDSV(i);

        for (auto& pShadowObject : m_CascadeObjects)
        {
            pShadowObject->Render_Shadow();
        }

        m_pGameInstance->Restore_RT();
    }
    
    for (auto& pShadowObject : m_CascadeObjects)
        Safe_Release(pShadowObject);
    m_CascadeObjects.clear();
    
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
        Safe_Release(pRenderObject);
    m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)].clear();
    
    if (FAILED(SetUp_Viewport(m_fViewportWidth, m_fViewportHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer, m_pGameInstance->Get_CurrentLevelID());

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Combined()
{
    /* 백버퍼 */
    //if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BackBuffer"), nullptr, false)))
    //    return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

#ifdef _DEBUG

    if (m_isEnableShadow)
    {
        _uint iNumCascades = m_pGameInstance->Get_NumCascades();

        if (FAILED(m_pShader->Bind_FloatArray("g_Splits", m_pGameInstance->Get_CascadeSplits(), iNumCascades)))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_RawValue("g_iNumCascades", &iNumCascades, sizeof(_uint))))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_Matrices("g_LightViewMatrices", m_pGameInstance->Get_ShadowLightViewMatrices(), iNumCascades)))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_Matrices("g_LightProjMatrices", m_pGameInstance->Get_ShadowLightProjMatrices(), iNumCascades)))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Bind_ShadowSRVArray(m_pShader, "g_TextureArray")))
            return E_FAIL;

        _float fBias = m_pGameInstance->Get_ShadowBias();
        if (FAILED(m_pShader->Bind_RawValue("g_fBias", &fBias, sizeof(_float))))
            return E_FAIL;

        _float2 vShadowMapSize = _float2(g_iMaxWidth, g_iMaxHeight);
        if (FAILED(m_pShader->Bind_RawValue("g_vShadowMapSize", &vShadowMapSize, sizeof(_float2))))
            return E_FAIL;

        _int iEnableShadowFlag = static_cast<_int>(m_isEnableShadow);
        if (FAILED(m_pShader->Bind_RawValue("g_iEnableShadowFlag", &iEnableShadowFlag, sizeof(_int))))
            return E_FAIL;
    }
    else
        m_pGameInstance->Clear_ShadowDSVs();

#else

    _uint iNumCascades = m_pGameInstance->Get_NumCascades();

    if (FAILED(m_pShader->Bind_FloatArray("g_Splits", m_pGameInstance->Get_CascadeSplits(), iNumCascades)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_iNumCascades", &iNumCascades, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrices("g_LightViewMatrices", m_pGameInstance->Get_ShadowLightViewMatrices(), iNumCascades)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrices("g_LightProjMatrices", m_pGameInstance->Get_ShadowLightProjMatrices(), iNumCascades)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_ShadowSRVArray(m_pShader, "g_TextureArray")))
        return E_FAIL;

    _float fBias = m_pGameInstance->Get_ShadowBias();
    if (FAILED(m_pShader->Bind_RawValue("g_fBias", &fBias, sizeof(_float))))
        return E_FAIL;

    _float2 vShadowMapSize = _float2(g_iMaxWidth, g_iMaxHeight);
    if (FAILED(m_pShader->Bind_RawValue("g_vShadowMapSize", &vShadowMapSize, sizeof(_float2))))
        return E_FAIL;

#endif

    m_pShader->Begin(3);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    //if (FAILED(m_pGameInstance->End_MRT()))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Blur()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BackBuffer"), m_pShader, "g_BackBufferTexture")))
        return E_FAIL;

    m_pShader->Begin(4);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("Target_BlurX"), m_pShader, "g_BlurXTexture")))
        return E_FAIL;

    m_pShader->Begin(5);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)].clear();

    return S_OK;
}


HRESULT CRenderer::Render_Blend()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].clear();

    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)].clear();

    return S_OK;
}

HRESULT CRenderer::SetUp_Viewport(_float fWidth, _float fHeight)
{
    D3D11_VIEWPORT			ViewPortDesc;
    ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width = fWidth;
    ViewPortDesc.Height = fHeight;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);

    return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{
    for (auto& pDebugCom : m_DebugComponent)
    {
        if (nullptr != pDebugCom)
            pDebugCom->Render();

        Safe_Release(pDebugCom);
    }
    m_DebugComponent.clear();


    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pGameInstance->Render_RT_Debug(m_pShader, m_pVIBuffer);

    // Texture2DArrayRender

    _uint iNumCascades = m_pGameInstance->Get_NumCascades();

    _int iCol = 1;
    _int iRow = (iNumCascades + iCol - 1) / iCol;

    _float fWidth = 300.f;
    _float fHeight = 300.f;

    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    _float fX = ViewportDesc.Width - 150.0f;
    _float fY = 150.f;

    for (_uint i = 0; i < iNumCascades; ++i)
    {
        _int iCX = i % iCol;
        _int iCY = i / iCol;

        // 월드 행렬 스케일링, 위치 조정
        _matrix WorldMatrix = XMMatrixScaling(fWidth, fHeight, 1.f);
        WorldMatrix.r[3] = XMVectorSetX(WorldMatrix.r[3], fX - ViewportDesc.Width * 0.5f);
        WorldMatrix.r[3] = XMVectorSetY(WorldMatrix.r[3], -fY + ViewportDesc.Height * 0.5f);

        _float4x4 ResultWolrdMatrix{};
        XMStoreFloat4x4(&ResultWolrdMatrix, WorldMatrix);

        // 월드 뷰 투영 바인딩, SRV 바인딩, 슬라이스 인덱스 바인딩, 렌더 호출
        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &ResultWolrdMatrix)))
            return E_FAIL;
         
        if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        if (FAILED(m_pGameInstance->Bind_ShadowSRVArray(m_pShader, "g_TextureArray")))
            return E_FAIL;

        if (FAILED(m_pShader->Bind_RawValue("g_iTextureArrayIndex", &i, sizeof(_int))))
            return E_FAIL;

        m_pShader->Begin(6);

        m_pVIBuffer->Bind_Resources();
        m_pVIBuffer->Render();

        fY += fHeight;
    }

    return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CRenderer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRenderer::Free()
{
    __super::Free();

#ifdef DEBUG
    for (auto& pDebugComponent : m_DebugComponent)
        Safe_Release(pDebugComponent);
    m_DebugComponent.clear();
#endif // DEBUG
    
    for (size_t i = 0; i < ENUM_CLASS(RENDERGROUP::END); i++)
    {
        for (auto& pRenderObject : m_RenderObjects[i])
            Safe_Release(pRenderObject);

        m_RenderObjects[i].clear();
    }

    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
