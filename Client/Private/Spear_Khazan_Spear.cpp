#include "Spear_Khazan_Spear.h"
#include "Khazan_Spear.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CSpear_Khazan_Spear::CSpear_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{

}

CSpear_Khazan_Spear::CSpear_Khazan_Spear(const CSpear_Khazan_Spear& Prototype)
    : CPartObject{ Prototype }
    , m_pClientInstance{CClientInstance::GetInstance()}
{
    Safe_AddRef(m_pClientInstance);
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
    m_pParentStatus = pDesc->pStatus;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pClientInstance->Set_ChangePlayerWeaponEquipmentCallBack([this](EQUIPMENTTYPE type, const _wstring& strPartName) {Change_Weapon(type, strPartName); });

    m_matOffset = XMMatrixRotationX(XMConvertToRadians(-90.0f));
    //int a = m_pModelCom->Get_BoneIndex("Weapon_R");
    m_pModelCom->Set_RootBone(0);
    m_pModelCom->Set_Transform(&m_CombinedWorldMatrix);

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

    XMStoreFloat4x4(&m_CombinedWorldMatrix, m_matOffset * matWeapon * XMLoadFloat4x4(m_pParentMatrix));

    m_pMotionTrailCom->Update(fTimeDelta);

    if (m_isActiveMotionTrail)
        m_pMotionTrailCom->Start_MotionTrail(fTimeDelta);

    // Heal RimLight
    if (m_isEnableHealRimLight)
    {
        m_HealRimLightDesc.fTimeAcc += fTimeDelta;

        if (m_HealRimLightDesc.fDuration <= m_HealRimLightDesc.fTimeAcc)
        {
            m_isEnableHealRimLight = false;
            m_isFinishedHealRimLight = true;
            m_HealRimLightDesc.fTimeAcc = 0.f;
            m_HealRimLightDesc.fTargetIntensity = 0.f;
        }

        _float fIntensityRatio = 1.f;

        // 페이드 아웃 계산
        if (m_HealRimLightDesc.fTimeAcc > m_HealRimLightDesc.vFadeTime.y)
        {
            _float fFadeDuration = m_HealRimLightDesc.fDuration - m_HealRimLightDesc.vFadeTime.y;
            _float fFadeTimeAcc = m_HealRimLightDesc.fTimeAcc - m_HealRimLightDesc.vFadeTime.y;
            _float fRatio = (fFadeTimeAcc / fFadeDuration);
            fIntensityRatio = 1.f - fRatio;
            fIntensityRatio = max(0.f, fIntensityRatio);
        }

        // Fade In
        if (m_HealRimLightDesc.fTimeAcc < m_HealRimLightDesc.vFadeTime.x)
        {
            fIntensityRatio = m_HealRimLightDesc.fTimeAcc / m_HealRimLightDesc.vFadeTime.x;
            fIntensityRatio = min(1.f, fIntensityRatio);
        }

        m_HealRimLightDesc.fRimLightIntensity = m_HealRimLightDesc.fTargetIntensity * fIntensityRatio;
    }
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

    _float fEdgeIntensity = 1.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.5f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fShadeIntensity", &fShadeIntensity, sizeof(_float))))
        return E_FAIL;

    // Heal RimLight
    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableHealRimLight", &m_isEnableHealRimLight)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    _float fRimPower = 2.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &fRimPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimLightIntensity", &m_HealRimLightDesc.fRimLightIntensity, sizeof(_float))))
        return E_FAIL;

    _float fRimEmissive = 2.f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimEmissive", &fRimEmissive, sizeof(_float))))
        return E_FAIL;

    _float3 vRimColor = _float3(1.f, 0.f, 0.f);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &vRimColor, sizeof(_float3))))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);
        m_pModelCom->Bind_Materials(m_pShaderCom, "g_EmissiveTexture", i, aiTextureType_EMISSIVE, 0);

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (m_pClientInstance->Is_CurrentSpear())
        {
            if (m_pClientInstance->Get_PlayerEquipment().iSpear == 4011) //섬광일상
            {
                _float fDiffusePower = 10.f;
                if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiffusePower", &fDiffusePower, sizeof(_float))))
                    return E_FAIL;

                m_pShaderCom->Begin(31);
            }
            else if (m_pClientInstance->Get_PlayerEquipment().iSpear == 4012) //연단된 징벌의 창
                m_pShaderCom->Begin(28);
        }

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

const MOTIONTRAIL_CONFIG& CSpear_Khazan_Spear::Get_MotionTrailConfig()
{
    return m_pMotionTrailCom->Get_Config();
}

void CSpear_Khazan_Spear::Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config)
{
    m_pMotionTrailCom->Set_Config(Config);
}

void CSpear_Khazan_Spear::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

_bool CSpear_Khazan_Spear::isEnableMotionTrail()
{
    return m_pMotionTrailCom->isEnable();
}

void CSpear_Khazan_Spear::Start_MotionTrail(_float fDuration)
{
    m_pMotionTrailCom->Start_MotionTrail(fDuration);
}

void CSpear_Khazan_Spear::Start_HealRimLight(_float fDuration, const _float2& vFadeTime, _float fMaxIntensity)
{
    if (true == m_isFinishedHealRimLight)
        return;

    m_isEnableHealRimLight = true;
    m_HealRimLightDesc.fDuration = fDuration;
    m_HealRimLightDesc.vFadeTime = vFadeTime;
    m_HealRimLightDesc.vFadeTime.y = m_HealRimLightDesc.fDuration - m_HealRimLightDesc.vFadeTime.y;
    m_HealRimLightDesc.fTargetIntensity = fMaxIntensity;
}

void CSpear_Khazan_Spear::Change_Weapon(EQUIPMENTTYPE type, const _wstring& strPartName)
{
    if (m_pModelCom)
        Safe_Release(m_pModelCom);

    _bool isGSword = false;
    _bool isSpear = false;

    if (strPartName == TEXT("Meteor_GSword")) {
        m_pModelCom = m_pModelCom_Meteor_GSword;
        isGSword = true;
    }
    else if (strPartName == TEXT("Execution_GSword")) {
        m_pModelCom = m_pModelCom_Execution_GSword;
        isGSword = true;
    }
    else if (strPartName == TEXT("Flash_Spear")) {
        m_pModelCom = m_pModelCom_Flash_Spear;
        isSpear = true;
    }
    else if (strPartName == TEXT("Punish_Spear")) {
        m_pModelCom = m_pModelCom_Punish_Spear;
        isSpear = true;
    }
    Safe_AddRef(m_pModelCom);

/*    if (isGSword)
    {
        *m_pParentStatus |= CKhazan_Spear::PLAYER_STATUS::GSWORD;
        *m_pParentStatus &= ~(CKhazan_Spear::PLAYER_STATUS::SPEAR | CKhazan_Spear::PLAYER_STATUS::BAREHAND);
    }
    else */if (isSpear)
    {
        *m_pParentStatus |= CKhazan_Spear::PLAYER_STATUS::SPEAR;
        *m_pParentStatus &= ~(/*CKhazan_Spear::PLAYER_STATUS::GSWORD |*/ CKhazan_Spear::PLAYER_STATUS::BAREHAND);
    }
    else
    {
        *m_pParentStatus |= CKhazan_Spear::PLAYER_STATUS::BAREHAND;
        *m_pParentStatus &= ~(/*CKhazan_Spear::PLAYER_STATUS::GSWORD |*/ CKhazan_Spear::PLAYER_STATUS::SPEAR);
    }

    m_isEnble = true;
}

HRESULT CSpear_Khazan_Spear::Ready_Components()
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

    CPlayerData_Manager::PLAYER_EQUIPMENT equipment =  m_pClientInstance->Get_PlayerEquipment();
    if (equipment.isSpear)
         m_pModelCom = equipment.iSpear == 4011 ? m_pModelCom_Punish_Spear : m_pModelCom_Flash_Spear;
    if (equipment.isGSword)
        m_pModelCom = equipment.iGSword == 4001 ? m_pModelCom_Meteor_GSword : m_pModelCom_Execution_GSword;
    Safe_AddRef(m_pModelCom);

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = false;
    MTDesc.Config.vLifeTime = { 0.f, 0.3f };
    MTDesc.Config.vStartColor = { 0.25f, 0.25f, 0.5f };
    MTDesc.Config.vTargetColor = { 0.5f, 0.5f, 0.5f };
    MTDesc.Config.fRimPower = 2.f;
    MTDesc.Config.fRimIntensity = 1.f;
    MTDesc.Config.fEmissiveIntensity = 2.f;
    MTDesc.Config.isIndividualColor = true;
    MTDesc.Config.fColorUpdateSpeed = 1500.f;
    MTDesc.Config.fInterval = 0.1f;
    MTDesc.Config.iMaxFrames = 10.f;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrailCom), &MTDesc)))
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

    Safe_Release(m_pClientInstance);
    Safe_Release(m_pParentTransform);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pModelCom_Punish_Spear);
    Safe_Release(m_pModelCom_Flash_Spear);
    Safe_Release(m_pModelCom_Meteor_GSword);
    Safe_Release(m_pModelCom_Execution_GSword);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pMotionTrailCom);
    //Safe_Release(m_pColliderCom);
}
