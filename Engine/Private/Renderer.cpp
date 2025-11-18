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

    m_fQuartViewportWidth = m_fViewportWidth * 0.25f;
    m_fQuartViewportHeight = m_fViewportHeight * 0.25f;

    if (FAILED(Ready_RenderTargets()))
        return E_FAIL;

    if (FAILED(Ready_MRTs()))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Matrices()))
        return E_FAIL;

    m_threadCLs.resize(m_pGameInstance->Get_ThreadCount(), nullptr);

#ifdef _DEBUG
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Diffuse"), 150.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Normal"), 150.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Specular"), 150.0f, 750.0f, 300.f, 300.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Shade"), 450.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_SSAO"), 450.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_SpecularLight"), 450.0f, 750.0f, 300.f, 300.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_PostScene"), 750.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Outline"), 750.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Emissive"), 750.0f, 750.0f, 300.f, 300.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Fog"), 1050.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Combined"), 1050.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Velocity"), 1050.0f, 750.0f, 300.f, 300.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Brightness"), 1350.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_BlurX"), 1350.0f, 450.0f, 300.f, 300.f)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Ready_RT_Debug(TEXT("RT_Bloom"), 1350.0f, 750.0f, 300.f, 300.f)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Ready_Shadow_Debug(m_fViewportWidth - 150.0f, 150.0f, 300.f, 300.f)))
        return E_FAIL;
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
    {
        MSG_BOX(TEXT("Failed To Render Priority"));
        return E_FAIL;
    }
    
    if (isEnableShadow())
        if (FAILED(Render_Shadow()))
        {
            MSG_BOX(TEXT("Failed To Render Shadow"));
            return E_FAIL;
        }
    
    if (FAILED(Render_Static()))
    {
        MSG_BOX(TEXT("Failed To Render Static"));
        return E_FAIL;
    }
    
    if (FAILED(Render_StaticVelocity()))
    {
        MSG_BOX(TEXT("Failed To Render Static Velocity"));
        return E_FAIL;
    }

    if (FAILED(Render_Decal()))
    {
        MSG_BOX(TEXT("Failed To Render Decal"));
        return E_FAIL;
    }
    
    if (FAILED(Render_Dynamic()))
    {
        MSG_BOX(TEXT("Failed To Render Dynamic"));
        return E_FAIL;
    }

    //  if (FAILED(Render_Outline()))
    //  {
    //      MSG_BOX(TEXT("Failed To Render Outline"));
    //      return E_FAIL;
    //  }
    
    if (isEnableSSAO())
        if (FAILED(Render_SSAO()))
        {
            MSG_BOX(TEXT("Failed To Render SSAO"));
            return E_FAIL;
        }
    
    if (FAILED(Render_Lights()))
    {
        MSG_BOX(TEXT("Failed To Render Lights"));
        return E_FAIL;
    }
    
    if (FAILED(Render_PostScene()))
    {
        MSG_BOX(TEXT("Failed To Render PostScene"));
        return E_FAIL;
    }
    
    if (FAILED(Render_NonLight()))
    {
        MSG_BOX(TEXT("Failed To Render NonLight"));
        return E_FAIL;
    }

    if (FAILED(Render_Blend()))
    {
        MSG_BOX(TEXT("Failed To Render Blend"));
        return E_FAIL;
    }

    if (FAILED(Render_Fog()))
    {
        MSG_BOX(TEXT("Failed To Render Fog"));
        return E_FAIL;
    }
    
    if (FAILED(Render_Brightness()))
    {
        MSG_BOX(TEXT("Failed To Render Brightness"));
        return E_FAIL;
    }

    if (FAILED(Render_Bloom()))
    {
        MSG_BOX(TEXT("Failed To Render Bloom"));
        return E_FAIL;
    }
    
    if (FAILED(Render_Combined()))
    {
        MSG_BOX(TEXT("Failed To Render Combined"));
        return E_FAIL;
    }

    if (FAILED(Render_RadialBlur()))
    {
        MSG_BOX(TEXT("Failed To Render Radial Blur"));
        return E_FAIL;
    }

    if (FAILED(Render_MotionBlur()))
    {
        MSG_BOX(TEXT("Failed To Render Motion Blur"));
        return E_FAIL;
    }

    if (FAILED(Render_LUT()))
    {
        MSG_BOX(TEXT("Failed To Render LUT"));
        return E_FAIL;
    }

    if (FAILED(Render_Distortion()))
    {
        MSG_BOX(TEXT("Failed To Render Distortion"));
        return E_FAIL;
    }

    if (FAILED(Render_UI()))
    {
        MSG_BOX(TEXT("Failed To Render UI"));
        return E_FAIL;
    }

#ifdef _DEBUG
    if (isEnableDebugRender())
        if (FAILED(Render_Debug()))
        {
            MSG_BOX(TEXT("Failed To Render Debug"));
            return E_FAIL;
        }
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
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PostScene"))))
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
    if (FAILED(SetUp_Viewport(g_iMaxWidth, g_iMaxHeight)))
        return E_FAIL;

    m_pGameInstance->Backup_RT();

    m_pGameInstance->Clear_ShadowDSV();
    m_pGameInstance->Bind_ShadowDSV();

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render_Shadow();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)].clear();

    m_pGameInstance->Restore_RT();

    if (FAILED(SetUp_Viewport(m_fViewportWidth, m_fViewportHeight)))
        return E_FAIL;

    m_pContext->VSSetShader(nullptr, nullptr, 0);

    return S_OK;
}

HRESULT CRenderer::Render_Static()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    vector<CGameObject*> Deferred, Immediate;
    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::STATIC)]) {
        
        if (nullptr != pRenderObject)
        {
            if (pRenderObject->Get_IsDeferred())
            {
                Deferred.push_back(pRenderObject);
            }
            else
            {
                Immediate.push_back(pRenderObject);
            }
        }
        
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::STATIC)].clear();

    Deferred_JobAndImmediate(Deferred, Immediate);

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_StaticVelocity()
{
    // Velocity Map 렌더링
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Velocity"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_MotionBlur_ShaderResources(m_pShader)))
        return E_FAIL;

    // 정적 오브젝트만 스크린 모션 블러 적용
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    m_pShader->Begin(14);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    // 이전 뷰 투영 행렬 갱신
    m_pGameInstance->Update_MotionBlur_PrevMatrices();

    return S_OK;
}

HRESULT CRenderer::Render_Decal()
{
    /* Diffuse */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Decal"), false)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_Decals()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Dynamic()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"), false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::DYNAMIC)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::DYNAMIC)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_DynamicVelocity()
{
    // Velocity Map 렌더링
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Velocity"), false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::MOTION)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render_Motion();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::MOTION)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Outline()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Outline"))))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::OUTLINE)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render_Outline();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::OUTLINE)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    _float2 vScreenSize = _float2(m_fViewportWidth, m_fViewportHeight);
    if (FAILED(m_pShader->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_SSAO_ShaderResources(m_pShader)))
        return E_FAIL;

    m_pShader->Begin(7);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_SSAO"), m_pShader, "g_SSAOTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fToonShadeLevel", &m_fToonShadeLevel, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_vSpecularPower", &m_vSpecularPower, sizeof(_float2))))
        return E_FAIL;

	_bool isSSAO = isEnableSSAO();
	if (FAILED(m_pShader->Bind_Bool("g_isEnableSSAO", &isSSAO)))
		return E_FAIL;
	_bool isToonShade = isEnableToonShade();
	if (FAILED(m_pShader->Bind_Bool("g_isEnableToonShade", &isToonShade)))
		return E_FAIL;

    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer, m_pGameInstance->Get_CurrentLevelID());

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_PostScene()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PostScene"), false)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_SpecularLight"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(m_pShader->Bind_Bool("g_isEnableShadow", &m_isEnableShadow)))
        return E_FAIL;
#endif

    if (false == isEnableShadow())
    {
        m_pGameInstance->Clear_ShadowDSV();

        m_pShader->Begin(3);

        m_pVIBuffer->Bind_Resources();
        m_pVIBuffer->Render();

        if (FAILED(m_pGameInstance->End_MRT()))
            return E_FAIL;

        return S_OK;
    }

    if (FAILED(m_pGameInstance->Bind_Shadow_ShaderResources(m_pShader)))
        return E_FAIL;

    m_pShader->Begin(3);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EmissiveAcc"), false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_EmissiveAcc"), false)))
        return E_FAIL;

    for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)])
    {
        if (nullptr != pRenderObject)
            pRenderObject->Render();

        Safe_Release(pRenderObject);
    }

    m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Fog()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Fog"))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Fog_ShaderResources(m_pShader)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_PostScene"), m_pShader, "g_PostSceneTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    m_pShader->Begin(9);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Brightness()
{
    if (FAILED(SetUp_Viewport(m_fQuartViewportWidth, m_fQuartViewportHeight)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Brightness"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Emissive"), m_pShader, "g_EmissiveTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_PostScene"), m_pShader, "g_PostSceneTexture")))
        return E_FAIL;

    m_pShader->Begin(15);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(SetUp_Viewport(m_fViewportWidth, m_fViewportHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Bloom()
{
    if (FAILED(SetUp_Viewport(m_fQuartViewportWidth, m_fQuartViewportHeight)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_GaussianBlur_ShaderResources(m_pShader)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Brightness"), m_pShader, "g_BrightTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fViewportWidth", &m_fQuartViewportWidth, sizeof(_float))))
        return E_FAIL;

    m_pShader->Begin(4);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_BlurX"), m_pShader, "g_BlurXTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fViewportHeight", &m_fQuartViewportHeight, sizeof(_float))))
        return E_FAIL;

    m_pShader->Begin(5);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    if (FAILED(SetUp_Viewport(m_fViewportWidth, m_fViewportHeight)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Combined()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Combined"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_PostScene"), m_pShader, "g_PostSceneTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Emissive"), m_pShader, "g_EmissiveTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Bloom"), m_pShader, "g_BloomTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Fog"), m_pShader, "g_FogTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Outline"), m_pShader, "g_OutlineTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fOutlineAlpha", &m_OutlineConfig.fAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_RawValue("g_fOutlineBias", &m_OutlineConfig.fBias, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Vignette_ShaderResources(m_pShader)))
        return E_FAIL;

#ifdef _DEBUG
    if (FAILED(m_pShader->Bind_Bool("g_isEnableShadow", &m_isEnableShadow)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Bool("g_isEnableFog", &m_isEnableFog)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Bool("g_isEnableOutline", &m_isEnableOutline)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Bool("g_isEnableDistortion", &m_isEnableDistortion)))
        return E_FAIL;
#endif

    m_pShader->Begin(8);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_RadialBlur"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Combined"), m_pShader, "g_CombinedTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RadialBlur_ShaderResources(m_pShader)))
        return E_FAIL;
    
    m_pShader->Begin(12);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
    // Screen Space Motion Blur
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_MotionBlur"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_MotionBlur_ShaderResources(m_pShader)))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_RadialBlur"), m_pShader, "g_CombinedTexture")))
        return E_FAIL;
    
    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_Velocity"), m_pShader, "g_VelocityTexture")))
        return E_FAIL;

    m_pShader->Begin(13);
    
    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();
    
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_LUT()
{
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LUT"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_MotionBlur"), m_pShader, "g_CombinedTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_LUT_ShaderResources(m_pShader)))
        return E_FAIL;

    m_pShader->Begin(11);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
    if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(TEXT("RT_LookUpTable"), m_pShader, "g_CombinedTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Distortion_ShaderResources(m_pShader)))
        return E_FAIL;

    _float fAspect = m_fViewportWidth / m_fViewportHeight;
    if (FAILED(m_pShader->Bind_RawValue("g_fAspect", &fAspect, sizeof(_float))))
        return E_FAIL;

    m_pShader->Begin(10);

    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

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

HRESULT CRenderer::Ready_RenderTargets()
{
    /* RT_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Diffuse"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 1.f, 1.f, 0.f))))
        return E_FAIL;

    /* RT_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Normal"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* RT_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Depth"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_World */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_World"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Shade"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_SSAO */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_SSAO"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* RT_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Specular"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_SpecularLight -> LightAcc MRT */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_SpecularLight"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Emissive */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Emissive"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_PostScene */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_PostScene"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_BlurX */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_BlurX"), m_fQuartViewportWidth, m_fQuartViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Bloom == RT_BlurY */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Bloom"), m_fQuartViewportWidth, m_fQuartViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Brightness */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Brightness"), m_fQuartViewportWidth, m_fQuartViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Fog */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Fog"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Outline */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Outline"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
        return E_FAIL;

    /* RT_Combined */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Combined"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_LookUpTable */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_LookUpTable"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_RadialBlur */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_RadialBlur"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_MotionBlur */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_MotionBlur"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* RT_Velocity */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Velocity"), m_fViewportWidth, m_fViewportHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Ready_MRTs()
{
    /* MRT_GameObjects : 게임 오브젝트들의 정보를 저장받기위한 타겟들 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("RT_Diffuse"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("RT_Normal"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("RT_Depth"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("RT_World"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("RT_Specular"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("RT_Emissive"))))
        return E_FAIL;

    /* MRT_LightAcc : 빛들의 연산 결과를 누적 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("RT_Shade"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("RT_SpecularLight"))))
        return E_FAIL;

    /* MRT_PostScene : 원래 백버퍼에 그렸어야할 최종 장면을 그려놓는 타겟  */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PostScene"), TEXT("RT_PostScene"))))
        return E_FAIL;

    /* MRT_EmissiveAcc : Emissive 결과 누적, Blur 적용할 객체들을 설정하는 타겟 */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_EmissiveAcc"), TEXT("RT_PostScene"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_EmissiveAcc"), TEXT("RT_Emissive"))))
        return E_FAIL;

    /* MRT_Bloom */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Brightness"), TEXT("RT_Brightness"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX"), TEXT("RT_BlurX"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY"), TEXT("RT_Bloom"))))
        return E_FAIL;

    /* MRT_SSAO */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("RT_SSAO"))))
        return E_FAIL;

    /* MRT_Fog */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Fog"), TEXT("RT_Fog"))))
        return E_FAIL;

    /* MRT_Outline */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Outline"), TEXT("RT_Outline"))))
        return E_FAIL;

    /* MRT_Decal */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Decal"), TEXT("RT_Diffuse"))))
        return E_FAIL;

    /* MRT_Combined */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Combined"), TEXT("RT_Combined"))))
        return E_FAIL;

    /* MRT_LUT */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LUT"), TEXT("RT_LookUpTable"))))
        return E_FAIL;

    /* MRT_RadialBlur */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_RadialBlur"), TEXT("RT_RadialBlur"))))
        return E_FAIL;

    /* MRT_MotionBlur */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_MotionBlur"), TEXT("RT_MotionBlur"))))
        return E_FAIL;

    /* MRT_Velocity */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Velocity"), TEXT("RT_Velocity"))))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Ready_Components()
{
    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Ready_Matrices()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(m_fViewportWidth, m_fViewportHeight, 1.f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(m_fViewportWidth, m_fViewportHeight, 0.f, 1.f));

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

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

void CRenderer::InitCLSlots(uint32_t N) {
    std::lock_guard<std::mutex> lk(m_Mutex);
    m_threadCLs.assign(N, nullptr);
}

void CRenderer::StoreRecordedCL(uint32_t idx, ID3D11CommandList* pCL) {
    std::lock_guard<std::mutex> lk(m_Mutex);
    Safe_Release(m_threadCLs[idx]);
    m_threadCLs[idx] = pCL; // 소유권 보유
}

ID3D11CommandList* CRenderer::ConsumeRecordedCL(uint32_t idx) {
    lock_guard<mutex> lk(m_Mutex);
    ID3D11CommandList* p = m_threadCLs[idx];
    m_threadCLs[idx] = nullptr;
    return p; // 호출자가 Release
}
void CRenderer::Deferred_JobAndImmediate(vector<class CGameObject*>& Deferred, vector<class CGameObject*>& Immediate)
{
    // [메인] 4) 뷰포트/상태 캐시 (Get은 메인만!)
    D3D11_VIEWPORT cachedVP{}; UINT vpCount = 1;
    m_pContext->RSGetViewports(&vpCount, &cachedVP);

    const uint32_t Context_Count = m_pGameInstance->GetDeferredContext_Count();
    auto chunks = SplitEvenly(Deferred, Context_Count);
    vector<future<HRESULT>> futures;
    futures.reserve(Context_Count);

    ID3D11DepthStencilView* pCurDSV = m_pGameInstance->Get_CurrentDSV_AddRef();

    for (uint32_t i = 0; i < Context_Count; ++i) {
        auto batch = move(chunks[i]);
        futures.emplace_back(
            m_pGameInstance->Add_Task([this, i, batch = move(batch), cachedVP, pCurDSV]() mutable -> HRESULT
                {
                    // 빈 배치는 커맨드리스트 없이 스킵
                    if (batch.empty()) { StoreRecordedCL(i, nullptr); return S_OK; }

                    ID3D11DeviceContext* pDef = m_pGameInstance->GetDeferredContext(i);

                    // (A) 디퍼드 컨텍스트에 같은 MRT를 '바인딩' (클리어X)
                    if (FAILED(m_pGameInstance->Apply_MRT_OnContext(TEXT("MRT_GameObjects"), pDef, pCurDSV, /*isClear=*/false)))
                        return E_FAIL;

                    // (B) 뷰포트/필요상태 SET (Get 금지)
                    pDef->RSSetViewports(1, &cachedVP);
                    // pDef->OMSetBlendState(pBlend, blendFactor, sampleMask) ... (선택)

                    // (C) 녹화
                    for (auto* o : batch) { o->Deferred_Render(pDef); Safe_Release(o); }
                    batch.clear();

                    // (D) CL 생성 및 기록
                    ID3D11CommandList* pCL = nullptr;
                    HRESULT hr = pDef->FinishCommandList(FALSE, &pCL);
                    if (FAILED(hr)) return hr;

                    StoreRecordedCL(i, pCL);
                    return S_OK;
                })
        );
    }

    for (auto& pRenderObject : Immediate)
    {
        pRenderObject->Render();
        Safe_Release(pRenderObject);
    }

    // [메인] 6) 워커 완료 대기 → 순서대로 Execute
    for (auto& f : futures) if (f.valid()) f.get();

    for (uint32_t i = 0; i < Context_Count; ++i) {
        ID3D11CommandList* CommandList = ConsumeRecordedCL(i);
        if (CommandList) {
            m_pContext->ExecuteCommandList(CommandList, TRUE);
            CommandList->Release();
        }
    }
    Deferred.clear();
    Immediate.clear();


    Safe_Release(pCurDSV);

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

    m_pGameInstance->Render_Shadow_Debug(m_pShader, m_pVIBuffer);

    // 디버그 렌더 이후 월드 행렬 복구
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    return S_OK;
}

#endif

_bool CRenderer::isEnableShadow()
{
#ifdef _DEBUG
    return m_isEnableShadow;
#else
    return true;
#endif
}

_bool CRenderer::isEnableSSAO()
{
#ifdef _DEBUG
    return m_isEnableSSAO;
#else
    return true;
#endif
}

_bool CRenderer::isEnableFog()
{
#ifdef _DEBUG
    return m_isEnableFog;
#else
    return true;
#endif
}

_bool CRenderer::isEnableToonShade()
{
#ifdef _DEBUG
    return m_isEnableToonShade;
#else
    return true;
#endif
}

#ifdef _DEBUG
_bool CRenderer::isEnableDebugRender()
{
    if (m_pGameInstance->Key_Down(DIK_PRIOR))
        m_isEnableDebugRender = true;
    else if (m_pGameInstance->Key_Down(DIK_NEXT))
        m_isEnableDebugRender = false;

    return m_isEnableDebugRender;
}
#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CRenderer"));
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
