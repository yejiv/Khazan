#include "Spear_Khazan_Spear.h"
#include "Khazan_Sample.h"
#include "GameInstance.h"
#include "ClientInstance.h"
CSpear_Khazan_Spear::CSpear_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{

}

CSpear_Khazan_Spear::CSpear_Khazan_Spear(const CSpear_Khazan_Spear& Prototype)
    : CPartObject{ Prototype }
{

}

_float4x4* CSpear_Khazan_Spear::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}



HRESULT CSpear_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSpear_Khazan_Spear::Initialize_Clone(void* pArg)
{
    SPEAR_KHAZAN_SPEAR_DESC* pDesc = static_cast<SPEAR_KHAZAN_SPEAR_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;



    m_matOffset = XMMatrixRotationX(XMConvertToRadians(-90.0f));
    int a = m_pModelCom->Get_BoneIndex("Weapon_R");
    m_pModelCom->Set_RootBone(0);

    /* 충돌 겹쳐지게*/
    m_isGhost = true;

    return S_OK;
}

void CSpear_Khazan_Spear::Priority_Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;

}

void CSpear_Khazan_Spear::Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;
    
	_matrix matWeapon = m_isEquip ? XMLoadFloat4x4(m_pWeaponR_Matrix) :  XMLoadFloat4x4(m_pBackPack_Matrix);

    matWeapon.r[0] = XMVector4Normalize(matWeapon.r[0]);
    matWeapon.r[1] = XMVector4Normalize(matWeapon.r[1]);
    matWeapon.r[2] = XMVector4Normalize(matWeapon.r[2]);

    m_pModelCom->Update_BoneCombinedMatrices();

   XMStoreFloat4x4(&m_CombinedWorldMatrix, m_matOffset * matWeapon * XMLoadFloat4x4(m_pParentMatrix) );

}

void CSpear_Khazan_Spear::Late_Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

#ifdef _DEBUG

#endif
}

HRESULT CSpear_Khazan_Spear::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);

        /*if (FAILED(m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_DIFFUSE, 0)
            return E_FAIL;        */

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CSpear_Khazan_Spear::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

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

HRESULT CSpear_Khazan_Spear::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Spear_Khazan_Sample"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CSpear_Khazan_Spear::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}


CSpear_Khazan_Spear* CSpear_Khazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSpear_Khazan_Spear* pInstance = new CSpear_Khazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CSpear_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSpear_Khazan_Spear::Clone(void* pArg)
{
    CSpear_Khazan_Spear* pInstance = new CSpear_Khazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CSpear_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpear_Khazan_Spear::Free()
{
    __super::Free();

    Safe_Release(m_pParentTransform);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    //Safe_Release(m_pColliderCom);
}
