#include "Destructible_Prop.h"

#include "GameInstance.h"

CDestructible_Prop::CDestructible_Prop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CProp_Interactive { pDevice, pContext }
{
}

CDestructible_Prop::CDestructible_Prop(const CDestructible_Prop& Prototype)
    : CProp_Interactive { Prototype }
{
}

HRESULT CDestructible_Prop::Initialize_Prototype()
{
    CHECK_FAILED(__super::Initialize_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CDestructible_Prop::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    DESTRUCTIBLE_PROP_DESC* pDesc = static_cast<DESTRUCTIBLE_PROP_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    m_eModelType = pDesc->eModelType;

    return S_OK;
}

void CDestructible_Prop::Priority_Update(_float fTimeDelta)
{
}

void CDestructible_Prop::Update(_float fTimeDelta)
{
}

void CDestructible_Prop::Late_Update(_float fTimeDelta)
{
    CHECK_FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this), );
}

HRESULT CDestructible_Prop::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CProp_Object : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(4), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

void CDestructible_Prop::Set_Destructible_ModelType(MODEL_TYPE eModelType)
{
    m_eModelType = eModelType;
}

string CDestructible_Prop::Get_Destructible_ModelType_ByString()
{
    string strCurrModelType = {};

    switch (m_eModelType)
    {
    case MODEL_TYPE::FENCE:
        strCurrModelType = "FENCE";
        break;
    case MODEL_TYPE::POT:
        strCurrModelType = "POT";
        break;
    case MODEL_TYPE::BARREL:
        strCurrModelType = "BARREL";
        break;
    }

    return strCurrModelType;
}

HRESULT CDestructible_Prop::Ready_Components(void* pArg)
{
    DESTRUCTIBLE_PROP_DESC* pDesc = static_cast<DESTRUCTIBLE_PROP_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("LEVEL 안넣음"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_Fence"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CDestructible_Prop::Bind_Materials(_uint iMeshIndex)
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
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalicTexture", iMeshIndex, aiTextureType_METALNESS, 0)))
        m_iMtrlFlags |= M_METALIC;
    if (SUCCEEDED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_RoughnessTexture", iMeshIndex, aiTextureType_SHININESS, 0)))
        m_iMtrlFlags |= M_ROUGHNESS;

    m_iMtrlFlags &= ~M_EMISSIVE;

    //m_iMtrlFlags &= ~M_EMISSIVE;
    //m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

CDestructible_Prop* CDestructible_Prop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDestructible_Prop* pInstance = new CDestructible_Prop(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CDestructible_Prop"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDestructible_Prop::Clone(void* pArg)
{
    CDestructible_Prop* pInstance = new CDestructible_Prop(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDestructible_Prop"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDestructible_Prop::Free()
{
    __super::Free();
}
