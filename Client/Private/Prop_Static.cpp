#include "Prop_Static.h"

#include "GameInstance.h"
#include "Body.h"
#include "DeferredShader.h"

CProp_Static::CProp_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Static::CProp_Static(const CProp_Static& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Static::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Static::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    PROP_STATIC_DESC* pDesc = static_cast<PROP_STATIC_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
    m_isDeferredContext = true;

    if (isSnow())
    {
        if (isIce())
            m_eShaderPass = SHADER_PASS::SNOWMAP_ICE;
        else if (isPlant())
            m_eShaderPass = SHADER_PASS::SNOWPLANT;
        else
            m_eShaderPass = SHADER_PASS::SNOWMAP;
    }
    else
    {
        if (isIce())
            m_eShaderPass = SHADER_PASS::MAP_ICE;
        else if (isPlant())
            m_eShaderPass = SHADER_PASS::PLANT;
        else
            m_eShaderPass = SHADER_PASS::MAP;
    }

    return S_OK;
}

void CProp_Static::Priority_Update(_float fTimeDelta)
{
}

void CProp_Static::Update(_float fTimeDelta)
{
    if (isPlant())
    {
        m_fTime += fTimeDelta;
    }
}

void CProp_Static::Late_Update(_float fTimeDelta)
{
    /*
    if (isBackGround())     m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
    else if (isBlended())   m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
    else                    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);

    if (isShadow())    m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this);
    */
    
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CProp_Static::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Static : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    if (isPlant()) CHECK_FAILED(Bind_Waving_Plants(), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Instance_Materials(m_pModelCom, i);

        if (true == isSnow()) CHECK_FAILED(Bind_ShaderResources_ForSnowMap(i), E_FAIL);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(ENUM_CLASS(m_eShaderPass)), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CProp_Static::Deferred_Render(ID3D11DeviceContext* pDeferredContext)
{
    CHECK_FAILED_MSG(Deferred_Bind_ShaderResources(), TEXT("CProp_Static : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    if (isPlant()) CHECK_FAILED(Deferred_Bind_Waving_Plants(), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Deferred_Bind_Instance_Materials(m_pModelCom, i, m_pDeferredShader);

        if (true == isSnow()) CHECK_FAILED(Deferred_Bind_ShaderResources_ForSnowMap(i, m_pDeferredShader), E_FAIL);

        CHECK_FAILED_ASSERT(m_pDeferredShader->Begin(ENUM_CLASS(m_eShaderPass), pDeferredContext), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Deferred_Render(i, pDeferredContext), E_FAIL);
    }

    return S_OK;
}


HRESULT CProp_Static::Ready_Components(void* pArg)
{
    PROP_STATIC_DESC* pDesc = static_cast<PROP_STATIC_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL(LEVEL::END, eLevel, E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_ModelMeshInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DeferredShader_ModelMeshInstance"),
        TEXT("Com_DeferredShader"), reinterpret_cast<CComponent**>(&m_pDeferredShader), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CProp_Static::Ready_Collision(void* pArg)
{
    /*
    CBody::BODY_MESHSHAPE_DESC BodyDesc{};
    BodyDesc.pModel = m_pModelCom;
    BodyDesc.pTransform = m_pTransformCom;
    BodyDesc.bIsTrigger = false;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Static;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::MESH;
    BodyDesc.fFriction = 0.8f;
    BodyDesc.fMass = 1.0f;
    BodyDesc.fRestitution = 0.0f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPos;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.0f, 0.f);
    BodyDesc.pGameObject = this;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;
    */
    return S_OK;
}

HRESULT CProp_Static::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 카메라 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Static::Deferred_Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pDeferredShader, "g_WorldMatrix"), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    // 카메라 바인딩
    CHECK_FAILED(m_pDeferredShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)), E_FAIL);

    return S_OK;
}

HRESULT CProp_Static::Bind_Waving_Plants()
{
    m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_vWindDir", &m_vWindDir, sizeof(_float3));
    m_pShaderCom->Bind_RawValue("g_fWindPower", &m_fWindPower, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fWindSpeed", &m_fWindSpeed, sizeof(_float));

    return S_OK;
}

HRESULT CProp_Static::Deferred_Bind_Waving_Plants()
{
    m_pDeferredShader->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float));
    m_pDeferredShader->Bind_RawValue("g_vWindDir", &m_vWindDir, sizeof(_float3));
    m_pDeferredShader->Bind_RawValue("g_fWindPower", &m_fWindPower, sizeof(_float));
    m_pDeferredShader->Bind_RawValue("g_fWindSpeed", &m_fWindSpeed, sizeof(_float));

    return S_OK;
}

HRESULT CProp_Static::Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pDeferredShader->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

HRESULT CProp_Static::Deferred_Bind_Materials(_uint iMeshIndex)
{
    m_iMtrlFlags = 0;

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        m_iMtrlFlags |= M_DIFFUSE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        m_iMtrlFlags |= M_NORMAL;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        m_iMtrlFlags |= M_EMISSIVE;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        m_iMtrlFlags |= M_SPECULAR;

    m_iMtrlFlags &= ~M_EMISSIVE;
    m_iMtrlFlags &= ~M_SPECULAR;

    m_pDeferredShader->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

CProp_Static* CProp_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Static* pInstance = new CProp_Static(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Static"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Static::Clone(void* pArg)
{
    CProp_Static* pInstance = new CProp_Static(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Static"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Static::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pBodyCom);
    Safe_Release(m_pDeferredShader);
}
