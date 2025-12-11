#include "DestinyGem.h"

#include "GameInstance.h"

#include "ClientInstance.h"

#include "Amount.h"

#include "Effect_Prefab.h"


CDestinyGem::CDestinyGem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CDestinyGem::CDestinyGem(const CDestinyGem& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CDestinyGem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDestinyGem::Initialize_Clone(void* pArg)
{
    DESTINYGEM_DESC* pDesc = static_cast<DESTINYGEM_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_Components(pArg), E_FAIL);

    m_pConsumed = pDesc->pConsumed;

    m_iNumGem *= static_cast<_uint>(m_pGameInstance->Rand(2.f, 5.f));

    m_bBlustFX = false; 

    m_fEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("lantern")));
    if (m_fEffect)
    {
        m_fEffect->ResetChildren();
        m_fEffect->UpdatePosition(XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f));
    }

    return S_OK;
}

void CDestinyGem::Priority_Update(_float fTimeDelta)
{
}

void CDestinyGem::Update(_float fTimeDelta)
{
    CHECK_TRUE(m_isDead, );

    if (true == *m_pConsumed)
    {
        m_fTimeAcc += fTimeDelta;
    }

    if (1.f <= m_fTimeAcc)
    {
        if (false == m_bBlustFX)
        { 
            m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("stone_blust"), XMVectorSet(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42, m_CombinedWorldMatrix._43, 1.f));
            m_fEffect->SetClose();
            m_bBlustFX = true;
        }

        m_fDecreaseAlpha += fTimeDelta * 0.2f;
    }

    if (1.f <= m_fDecreaseAlpha)
    {
        static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Add_Value(CAmount::AMOUNT_TYPE::STONE, m_iNumGem);
        Set_IsDead(true);
    }

    Update_CombinedMatrix();
}

void CDestinyGem::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(RENDERGROUP::STATIC, this);
}

HRESULT CDestinyGem::Render()
{
    CHECK_FAILED_MSG(Bind_ShaderResources(), TEXT("CDestinyGem : Bind_ShaderResources 함수 E_FAIL"), E_FAIL);

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    _float fDiffuseRedPower = 10.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffuseRedPower", &fDiffuseRedPower, sizeof(_float))))
        return E_FAIL;

    _float fEmissiveIntensity = 2.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissiveIntensity", &fEmissiveIntensity, sizeof(_float))))
        return E_FAIL;

    _float4 vGemColor = _float4(1.25f, 0.25f, 1.25f, 1.f);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vGemColor", &vGemColor, sizeof(_float4))))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        Bind_Materials(i);

        Bind_DissolveValues();

        CHECK_FAILED_ASSERT(m_pShaderCom->Begin(13), E_FAIL);

        CHECK_FAILED_ASSERT(m_pModelCom->Render(i), E_FAIL);
    }

    return S_OK;
}

HRESULT CDestinyGem::Ready_Components(void* pArg)
{
    DESTINYGEM_DESC* pDesc = static_cast<DESTINYGEM_DESC*>(pArg);
    CHECK_NULLPTR(pDesc, E_FAIL);

    LEVEL eLevel = pDesc->eLevel;
    CHECK_EQUAL_MSG(LEVEL::END, eLevel, TEXT("level==end"), E_FAIL);

    // 개별 쉐이더 생성할지 고민
    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom), nullptr), E_FAIL);

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(eLevel), TEXT("Prototype_Component_Model_DestinyStone_Gem"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr), E_FAIL);

    return S_OK;
}

HRESULT CDestinyGem::Bind_DissolveValues()
{
    CHECK_FAILED(m_pDissolveTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    _float fEdgeWidth = { 0.08f };
    _float4 fEdgeColor = _float4(0.1f, 0.f, 0.f, 1.f);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_fDecreaseAlpha, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &fEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &fEdgeColor, sizeof(_float4));

    return S_OK;
}

HRESULT CDestinyGem::Bind_ShaderResources()
{
    // 월드 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix), E_FAIL);

    // 뷰 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)), E_FAIL);

    // 투영 행렬 쉐이더에 바인딩
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)), E_FAIL);

    return S_OK;
}

HRESULT CDestinyGem::Bind_Materials(_uint iMeshIndex)
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

    //  m_iMtrlFlags &= ~M_EMISSIVE;
    //  m_iMtrlFlags &= ~M_SPECULAR;

    m_pShaderCom->Bind_RawValue("g_MtrlFlags", &m_iMtrlFlags, sizeof(_uint));

    return S_OK;
}

CDestinyGem* CDestinyGem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDestinyGem* pInstance = new CDestinyGem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CDestinyGem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDestinyGem::Clone(void* pArg)
{
    CDestinyGem* pInstance = new CDestinyGem(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CDestinyGem"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDestinyGem::Free()
{
    __super::Free();

    Safe_Release(m_pDissolveTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_fEffect);
}
