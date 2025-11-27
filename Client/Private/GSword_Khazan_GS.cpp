#include "GSword_Khazan_GS.h"
//#include "Khazan_Sample.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CGSword_Khazan_GS::CGSword_Khazan_GS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{

}

CGSword_Khazan_GS::CGSword_Khazan_GS(const CGSword_Khazan_GS& Prototype)
    : CPartObject{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);

}

_float4x4* CGSword_Khazan_GS::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}

HRESULT CGSword_Khazan_GS::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGSword_Khazan_GS::Initialize_Clone(void* pArg)
{
    GSWORD_KHAZAN_GS_DESC* pDesc = static_cast<GSWORD_KHAZAN_GS_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pClientInstance->Set_ChangePlayerEquipmentCallBack([this](EQUIPMENTTYPE type, const _wstring& strPartName) {Change_Weapon(type, strPartName); });

    m_matOffset = XMMatrixRotationY(XMConvertToRadians(180.0f)) * XMMatrixRotationX(XMConvertToRadians(-90.0f));
    m_pModelCom->Set_RootBone(0);
    m_pModelCom->Set_Transform(&m_CombinedWorldMatrix);

    /* 충돌 겹쳐지게*/
    m_isGhost = true;

    return S_OK;
}

void CGSword_Khazan_GS::Priority_Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;

}

void CGSword_Khazan_GS::Update(_float fTimeDelta)
{
    if (!m_isEnble)
        return;
    
	_matrix matWeapon = m_isEquip ? XMLoadFloat4x4(m_pWeaponR_Matrix) :  XMLoadFloat4x4(m_pBackPack_Matrix);

    matWeapon.r[0] = XMVector4Normalize(matWeapon.r[0]);
    matWeapon.r[1] = XMVector4Normalize(matWeapon.r[1]);
    matWeapon.r[2] = XMVector4Normalize(matWeapon.r[2]);

    m_pModelCom->Update_BoneCombinedMatrices();

   XMStoreFloat4x4(&m_CombinedWorldMatrix, m_matOffset * matWeapon * XMLoadFloat4x4(m_pParentMatrix) );

   m_pMotionTrailCom->Update(fTimeDelta);
   if (m_isActiveMotionTrail) 
       m_pMotionTrailCom->Start_MotionTrail(fTimeDelta);


}

void CGSword_Khazan_GS::Late_Update(_float fTimeDelta)
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

HRESULT CGSword_Khazan_GS::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);


        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CGSword_Khazan_GS::Render_Shadow()
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

const MOTIONTRAIL_CONFIG& CGSword_Khazan_GS::Get_MotionTrailConfig()
{
    return m_pMotionTrailCom->Get_Config();
}

void CGSword_Khazan_GS::Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config)
{
    m_pMotionTrailCom->Set_Config(Config);
}

void CGSword_Khazan_GS::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

_bool CGSword_Khazan_GS::isEnableMotionTrail()
{
    return m_pMotionTrailCom->isEnable();
}

void CGSword_Khazan_GS::Start_MotionTrail(_float fDuration)
{
    m_pMotionTrailCom->Start_MotionTrail(fDuration);
}

void CGSword_Khazan_GS::Change_Weapon(EQUIPMENTTYPE type, const _wstring& strPartName)
{
    if (m_pModelCom)
        Safe_Release(m_pModelCom);

    if (strPartName == TEXT("Meteor_GSword")) m_pModelCom = m_pModelCom_Meteor_GSword;
    else if (strPartName == TEXT("Execution_GSword")) m_pModelCom = m_pModelCom_Execution_GSword;
    else if (strPartName == TEXT("Flash_Spear")) m_pModelCom = m_pModelCom_Flash_Spear;
    else if (strPartName == TEXT("Punish_Spear")) m_pModelCom = m_pModelCom_Punish_Spear;
    Safe_AddRef(m_pModelCom);

}

HRESULT CGSword_Khazan_GS::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Spear_Punish"),
        TEXT("Com_Model1"), reinterpret_cast<CComponent**>(&m_pModelCom_Punish_Spear), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Spear_Flash"),
        TEXT("Com_Model2"), reinterpret_cast<CComponent**>(&m_pModelCom_Flash_Spear), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_GSword_Meteor"),
        TEXT("Com_Model3"), reinterpret_cast<CComponent**>(&m_pModelCom_Meteor_GSword), nullptr)))
        return E_FAIL;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_GSword_Execution"),
        TEXT("Com_Model4"), reinterpret_cast<CComponent**>(&m_pModelCom_Execution_GSword), nullptr)))
        return E_FAIL;

    /* Test !!!!*/
#ifdef _DEBUG
    m_pClientInstance->Set_PlayerEquipment(EQUIPMENTTYPE::GSWORD, 4002);
#endif // _DEBUG


    CPlayerData_Manager::PLAYER_EQUIPMENT equipment = m_pClientInstance->Get_PlayerEquipment();
    if (equipment.isSpear)
        m_pModelCom = equipment.iSpear == 4011 ? m_pModelCom_Punish_Spear : m_pModelCom_Flash_Spear;
    if (equipment.isGSword)
        m_pModelCom = equipment.iGSword == 4001 ? m_pModelCom_Meteor_GSword : m_pModelCom_Execution_GSword;
    Safe_AddRef(m_pModelCom);

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = false;
    MTDesc.Config.vLifeTime = { 0.f, 0.3f };
    MTDesc.Config.vStartColor = { 1.f, 1.f, 1.f };
    MTDesc.Config.vTargetColor = { 1.f, 1.f, 1.f };
    MTDesc.Config.fRimPower = 2.f;
    MTDesc.Config.fRimIntensity = 1.f;
    MTDesc.Config.fEmissiveIntensity = 2.f;
    MTDesc.Config.isIndividualColor = true;
    MTDesc.Config.fColorUpdateSpeed = 1000.f;
    MTDesc.Config.fInterval = 0.1f;
    MTDesc.Config.iMaxFrames = 10.f;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrailCom), &MTDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGSword_Khazan_GS::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}


CGSword_Khazan_GS* CGSword_Khazan_GS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGSword_Khazan_GS* pInstance = new CGSword_Khazan_GS(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CGSword_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGSword_Khazan_GS::Clone(void* pArg)
{
    CGSword_Khazan_GS* pInstance = new CGSword_Khazan_GS(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CGSword_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGSword_Khazan_GS::Free()
{
    __super::Free();

    Safe_Release(m_pParentTransform);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pMotionTrailCom);
    //Safe_Release(m_pColliderCom);
}
