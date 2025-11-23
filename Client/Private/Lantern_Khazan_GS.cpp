#include "Lantern_Khazan_GS.h"
#include "Khazan_Sample.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CLantern_Khazan_GS::CLantern_Khazan_GS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CLantern_Khazan_GS::CLantern_Khazan_GS(const CLantern_Khazan_GS& Prototype)
    : CPartObject{ Prototype }
{
}

HRESULT CLantern_Khazan_GS::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLantern_Khazan_GS::Initialize_Clone(void* pArg)
{
    LANTERN_KHAZAN_SPEAR_DESC* pDesc = static_cast<LANTERN_KHAZAN_SPEAR_DESC*>(pArg);
    m_pParentTransform = pDesc->pParentTransform;
    m_pLHandSocket_Matrix = pDesc->pLHandSocket_Matrix;
    m_pLanternSocket_Matrix = pDesc->pLanternSocket_Matrix;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_matOffset = XMMatrixRotationX(XMConvertToRadians(-90.0f));
    m_pModelCom->Set_RootBone(0);

    return S_OK;
}

void CLantern_Khazan_GS::Priority_Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;
}

void CLantern_Khazan_GS::Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;

    if (!m_isEquip)
    {
        m_fEquipTime2.x += fTimeDelta;
        if (m_fEquipTime2.x >= m_fEquipTime2.y)
        {
            m_pGameInstance->Start_ShadowTransition(1.5f, 0.6f);
            m_pGameInstance->Set_LightEnable(TEXT("Lantern"), ENUM_CLASS(m_eCurrentLevel), false);
            m_isEnble = false;
            return;
        }
    }

    if (m_isEquiping)
    {
        m_fEquipTime1.x += fTimeDelta;
        if (m_fEquipTime1.x >= m_fEquipTime1.y)
        {
            m_isEquiping = false;
        }
    }

    m_pModelCom->Update_BoneCombinedMatrices();

    _matrix matWeapon = m_isEquiping ? XMLoadFloat4x4(m_pLHandSocket_Matrix) : XMLoadFloat4x4(m_pLanternSocket_Matrix);
    matWeapon.r[0] = XMVector4Normalize(matWeapon.r[0]);
    matWeapon.r[1] = XMVector4Normalize(matWeapon.r[1]);
    matWeapon.r[2] = XMVector4Normalize(matWeapon.r[2]);
    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_matOffset * matWeapon * XMLoadFloat4x4(m_pParentMatrix));

    _float4 vPos = *reinterpret_cast<_float4*>(m_CombinedWorldMatrix.m[3]);

    m_pGameInstance->Set_LightPosition(TEXT("Lantern"), ENUM_CLASS(m_eCurrentLevel), vPos);

}

void CLantern_Khazan_GS::Late_Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

}

HRESULT CLantern_Khazan_GS::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(10);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

void CLantern_Khazan_GS::Set_Equipped(bool equip)
{
    if (equip)
    {
        m_isEnble = m_isEquip = true;
        m_fEquipTime1.x = 0.f;
        m_isEquiping = true;
        m_pGameInstance->Start_ShadowTransition(1.5f, 1.f);
        m_pGameInstance->Set_LightEnable(TEXT("Lantern"), ENUM_CLASS(m_eCurrentLevel), true);
    }
    else {
        m_isEquiping = true;
        m_isEquip = false;
        m_fEquipTime2.x = 0.f;
        m_fEquipTime1.x = 0.f;
    }
}

HRESULT CLantern_Khazan_GS::Ready_Components()
{

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(m_eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Lantern"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLantern_Khazan_GS::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

CLantern_Khazan_GS* CLantern_Khazan_GS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLantern_Khazan_GS* pInstance = new CLantern_Khazan_GS(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CLantern_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLantern_Khazan_GS::Clone(void* pArg)
{
    CLantern_Khazan_GS* pInstance = new CLantern_Khazan_GS(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CLantern_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLantern_Khazan_GS::Free()
{
    __super::Free();

    Safe_Release(m_pParentTransform);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
}
