#include "Prop_Object.h"

#include "GameInstance.h"
#include "Body.h"

CProp_Object::CProp_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp { pDevice, pContext }
{
}

CProp_Object::CProp_Object(const CProp_Object& Prototype)
    : CProp { Prototype }
{
}

HRESULT CProp_Object::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CProp_Object::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    PROP_OBJECT_DESC* pDesc = static_cast<PROP_OBJECT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_pTransformCom->Set_WorldMatrix_4x4(pDesc->WorldMatrix);

    m_isDeferredContext = true;

    if (isCollider())
    {
        CHECK_FAILED(Ready_Collision(pArg), E_FAIL);
    }
    
    if (isSnow())
    {
        if (isIce())
            m_eShaderPass = SHADER_PASS::SNOWMAP_ICE;
        else
            m_eShaderPass = SHADER_PASS::SNOWMAP;
    }
    else
    {
        if (isIce())
            m_eShaderPass = SHADER_PASS::MAP_ICE;
        else
            m_eShaderPass = SHADER_PASS::MAP;
    }

    return S_OK;
}

void CProp_Object::Priority_Update(_float fTimeDelta)
{
}

void CProp_Object::Update(_float fTimeDelta)
{
}

void CProp_Object::Late_Update(_float fTimeDelta)
{
    /*
    if (isBackGround())     m_pGameInstance->Add_RenderGroup(RENDERGROUP::PRIORITY, this);
    else if (isBlended())   m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
    else if (isShadow())    m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this);
    else                    m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
    */

    if (false == (isCollider() && isBackGround()))
        m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CProp_Object::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        if (true == isSnow()) CHECK_FAILED(Bind_ShaderResources_ForSnowMap(i), E_FAIL);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(ENUM_CLASS(m_eShaderPass)), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CProp_Object::Deferred_Render(ID3D11DeviceContext* pDeferredContext)
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Deferred_Bind_Materials(i);

        if (true == isSnow()) CHECK_FAILED(Deferred_Bind_ShaderResources_ForSnowMap(i, m_pDeferredShader), E_FAIL);

        CHECK_FAILED_ASSERT(m_pDeferredShader->Begin(ENUM_CLASS(m_eShaderPass), pDeferredContext), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Deferred_Render(i, pDeferredContext), E_FAIL);
    }

    return S_OK;
}

HRESULT CProp_Object::Render_Shadow()
{
    if (false == isShadow())
        return S_OK;

    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::PROJ))))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(2);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT CProp_Object::Ready_Components(void* pArg)
{
    PROP_OBJECT_DESC* pDesc = static_cast<PROP_OBJECT_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL(LEVEL::END, eLevel, E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), m_szModelName,
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DeferredShader_VtxMesh"),
        TEXT("Com_DeferredShader"), reinterpret_cast<CComponent**>(&m_pDeferredShader), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CProp_Object::Ready_Collision(void* pArg)
{
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
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MAP_STATIC);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    BodyDesc.vPos = vPos;
    BodyDesc.vQuat = vQuat;
    BodyDesc.vShapeOffset = _float3(0.f, 0.0f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProp_Object::Bind_ShaderResources()
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

HRESULT CProp_Object::Bind_DeferredShaderResources()
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

HRESULT CProp_Object::Bind_Materials(_uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;

    isSpecular = false;
    isEmissive = false;

    m_pShaderCom->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pShaderCom->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));

    return S_OK;
}

HRESULT CProp_Object::Deferred_Bind_Materials(_uint iMeshIndex)
{
    _bool isDiffuse = { false };
    _bool isNormal = { false };
    _bool isEmissive = { false };
    _bool isSpecular = { false };

    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_DiffuseTexture", iMeshIndex, aiTextureType_DIFFUSE, 0)))
        isDiffuse = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_NormalTexture", iMeshIndex, aiTextureType_NORMALS, 0)))
        isNormal = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_EmissiveTexture", iMeshIndex, aiTextureType_EMISSIVE, 0)))
        isEmissive = true;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pDeferredShader, "g_SpecularTexture", iMeshIndex, aiTextureType_SPECULAR, 0)))
        isSpecular = true;

    isSpecular = false;
    isEmissive = false;

    m_pDeferredShader->Bind_RawValue("g_isDiffuse", &isDiffuse, sizeof(_bool));
    m_pDeferredShader->Bind_RawValue("g_isNormal", &isNormal, sizeof(_bool));
    m_pDeferredShader->Bind_RawValue("g_isEmissive", &isEmissive, sizeof(_bool));
    m_pDeferredShader->Bind_RawValue("g_isSpecular", &isSpecular, sizeof(_bool));

    return S_OK;
}

CProp_Object* CProp_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CProp_Object* pInstance = new CProp_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProp_Object::Clone(void* pArg)
{
    CProp_Object* pInstance = new CProp_Object(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CProp_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProp_Object::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pBodyCom);
    Safe_Release(m_pDeferredShader);
}
