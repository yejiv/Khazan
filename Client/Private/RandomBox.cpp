#include "RandomBox.h"
#include "GameInstance.h"

CRandomBox::CRandomBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice,pContext }
{
}

CRandomBox::CRandomBox(const CRandomBox& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CRandomBox::Initialize_Prototype(_int iLevel)
{
    m_iPrototypeIndex = iLevel;

    return S_OK;
}

HRESULT CRandomBox::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pModelCom->Set_Animation(0);

    return S_OK;
}

void CRandomBox::Priority_Update(_float fTimeDelta)
{
}

void CRandomBox::Update(_float fTimeDelta)
{
    m_pModelCom->Set_Animation(1);
    m_pModelCom->Play_Animation(fTimeDelta);
}

void CRandomBox::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
}

HRESULT CRandomBox::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    // 카메라 위치 바인딩 (자체 림 라이트)
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float3)), E_FAIL);

    _float fEdgeIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float)), E_FAIL);

    _float fShadeIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float)), E_FAIL);

    _float fDiffusePower = 2.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fDiffusePower", &fDiffusePower, sizeof(_float)), E_FAIL);

    _float fRimPower = 1.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float)), E_FAIL);

    _float fRimIntensity = 0.5f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &fRimIntensity, sizeof(_float)), E_FAIL);

    _float3 vRimColor = _float3(0.9f, 1.f, 0.8f);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &vRimColor, sizeof(_float3)), E_FAIL);

    _float fRimEmissive = 2.f;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &fRimEmissive, sizeof(_float)), E_FAIL);
    
    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        _uint iMtrlFlags = 0;

        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            iMtrlFlags |= M_DIFFUSE;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0)))
            iMtrlFlags |= M_NORMAL;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0)))
            iMtrlFlags |= M_EMISSIVE;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0)))
            iMtrlFlags |= M_SPECULAR;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", i, aiTextureType_METALNESS, 0)))
            iMtrlFlags |= M_METALIC;
        if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", i, aiTextureType_SHININESS, 0)))
            iMtrlFlags |= M_ROUGHNESS;

        CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_MtrlFlags", &iMtrlFlags, sizeof(_uint)), E_FAIL);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(20);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CRandomBox::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(m_iPrototypeIndex, TEXT("Prototype_Component_Model_NPC_DanjinJar_A"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRandomBox::Bind_ShaderResources()
{
    m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix");
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

CRandomBox* CRandomBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CRandomBox* pInstance = new CRandomBox(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CRandomBox"));
    }
    return pInstance;
}

CGameObject* CRandomBox::Clone(void* pArg)
{
    CRandomBox* pInstance = new CRandomBox(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CRandomBox"));
    }

    return pInstance;
}

void CRandomBox::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
