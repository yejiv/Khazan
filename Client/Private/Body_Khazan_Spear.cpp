#include "Body_Khazan_Spear.h"
#include "Khazan_Spear.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "MeshTrail.h"
#include "Spear_Khazan_Spear.h"
#include "Damage_Text.h"
#include "Target_BrutalAttack.h"
#include "Yetuga.h"
#include "WeaponObject.h"
#include "Khazan_SoundHelper.h"

using  SOUND_TYPE = CKhazan_SoundHelper::PC_SOUND_GROUPTYPE;
using  SOUND_CHANNEL = CKhazan_SoundHelper::PC_SOUND_CHANNEL;

CBody_Khazan_Spear::CBody_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
 
{
}

CBody_Khazan_Spear::CBody_Khazan_Spear(const CBody_Khazan_Spear& Prototype)
    : CPartObject{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);

}

_float4x4* CBody_Khazan_Spear::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);
}



void CBody_Khazan_Spear::Set_Spear(CSpear_Khazan_Spear* pSpear)
{
    m_pSpear = pSpear; Safe_AddRef(m_pSpear);
}

HRESULT CBody_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_Spear::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_SPEAR_DESC* pDesc = static_cast<BODY_KHAZAN_SPEAR_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentStatus = pDesc->pStatus;
    m_pHitReaction = pDesc->pHitReation;
    m_iCurState = *m_pParentState;
    m_pGuardRotationTarget = pDesc->pGuardRotationTarget;
    m_pParentTransform = pDesc->pParentTransform;
    m_pParentIsCanStaminaRecovery = pDesc->pParentIsCanStaminaRecovery;
    m_pHealIndex = pDesc->pHealIndex;
    Safe_AddRef(m_pParentTransform);

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* Î∂?Î™? ?ä∏?ûú?ä§?èº ?ó∞Í≤? */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

    /* ?Ç¨?ö¥?ìú ?ó¨?çº  */
    m_pSoundHelper = CKhazan_SoundHelper::Create();

    if (FAILED(Ready_Equipment()))
        return E_FAIL;

    if (FAILED(Ready_AnimationEvent()))
        return E_FAIL;

    if (FAILED(Ready_AnimationEvent_SFX()))
        return E_FAIL;

    /* ?ååÏ∏? ?ûê?èô ?óÖ?ç∞?ù¥?ä∏  */
    m_pClientInstance->Set_ChangePlayerArmorEquipmentCallBack([this](EQUIPMENTTYPE type, const _wstring& strPartName) {Equip_Part(type, strPartName); });

    /* Îº? ?ñâ?†¨ Í∞?Ïß?Í≥†Ïò§Í∏? */
    m_pSpearTip1_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R_SpearTip");
    m_pSpearPole_Matrix = m_pModelCom->Get_BoneMatrix("Weapon_R");

    

    if (FAILED(Ready_Collider()))
        return E_FAIL;

    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();
    m_isCollision = false;


    m_pModelCom->WarmupAnimations();

    m_pParentTransform->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 0.f));

    m_bGuradFX[0] = false;
    m_bGuradFX[1] = false;
    m_iTrailType = 0;
    m_TrailParticleTime = 0.f;

    return S_OK;
}

void CBody_Khazan_Spear::Priority_Update(_float fTimeDelta)
{
    m_pTrail->Priority_Update(fTimeDelta);
    if (m_isHitSound)
    {
        m_pGameInstance->PlaySoundOnce(m_pSoundHelper->Get_NextSoundKey(SOUND_TYPE::GUARD_SUCCESS, SOUND_CHANNEL::WEAPON), 0.8f, Get_SoundChannel(SOUND_CHANNEL::WEAPON));
        m_isHitSound = false;
    }


}

void CBody_Khazan_Spear::Update(_float fTimeDelta)
{


    m_isFinishedAnimation = m_pModelCom->Play_Animation(m_isNotifyAttacking ? fTimeDelta * 1.2f : fTimeDelta);

    Update_CombinedMatrix();
    Update_Collider(fTimeDelta);

    m_pTrail->Update(fTimeDelta);

    Check_Guarding(fTimeDelta);

    if (m_isCollision)
    {
        m_isCollision = false;
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("BloodHit"), XMLoadFloat4(&m_fCollisionPos));
    }

    m_pMotionTrailCom->Update(fTimeDelta);
    if (m_isActiveMotionTrail)
        m_pMotionTrailCom->Start_MotionTrail(fTimeDelta);

    /* Î™®ÏÖò?ä∏?†à?ùºÏ§? ?ã§Î•? ?ï†?ãàÎ©îÏù¥?Öò?ù¥ ?Çò?ò¨ ?ãú ?ÅÑÍ∏?  */
    if (m_isEnableMotionTrail && m_iCurMotionTrailAnimIndex != m_pModelCom->Get_CurAnimIndex())
    {
        m_isEnableMotionTrail = false;
        Trigger_MotionTrail(TEXT(""), false);
    }

    if (m_bGuradFX[0] || m_bGuradFX[1])
        Spawn_Guard_FX();
    m_bGuradFX[0] = false;
    m_bGuradFX[1] = false;

    _vector vSpearPos = XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3];
    _float4 vResultSpearPos{};
    XMStoreFloat4(&vResultSpearPos, vSpearPos);
    m_pGameInstance->Set_LightPosition(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), vResultSpearPos);

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

        // ?éò?ù¥?ìú ?ïÑ?õÉ Í≥ÑÏÇ∞
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
    m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Teleport"), 0, BodyCenter());

    /*  ?ñ¥?ÉùÏΩúÎùº?ù¥?çî on -> ?ã§Î•? ?ï†?ãàÎ©îÏù¥?Öò ?ì§?ñ¥?ò§Î©? ?ÅÑÍ∏? */
    if (m_isSpearTipActive && m_iCurAnimIndex != m_pModelCom->Get_CurAnimIndex()) {
        m_isNotifyAttacking = false;
        m_isSpearTipActive = false;
        m_isSpearFullExtension = true;
        m_pBodyCom_SpearTip1->Collision_Active(false);
    }

    /*  Î∞îÎîî?ñ¥?ÉùÏΩúÎùº?ù¥?çî on -> ?ã§Î•? ?ï†?ãàÎ©îÏù¥?Öò ?ì§?ñ¥?ò§Î©? ?ÅÑÍ∏? */
    if (m_isBodyAttackActive && m_iCurAnimIndex != m_pModelCom->Get_CurAnimIndex()) {
        m_isBodyAttackActive = false;
        m_isNotifyAttacking = false;
        m_pBodyCom_BodyAttack->Collision_Active(false);
    }

    if (m_pGameInstance->Key_Pressing(DIK_RCONTROL, fTimeDelta))
    {
        if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
            m_isEnableEdge = false;

        if (m_pGameInstance->Key_Down(DIK_NUMPAD3))
            m_isEnableEdge = true;
    }
}

void CBody_Khazan_Spear::Late_Update(_float fTimeDelta)
{
    // ========== Before Render ==========
    //  if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONLIGHT, this)))
    //      return;
    //  //if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
    //  //    return;
    // ========== After Render ==========
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;
    //  if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::MOTIONVECTOR, this)))
    //      return;
    //  if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::OUTLINE, this)))
    //      return;

    m_pTrail->Late_Update(fTimeDelta);
}

HRESULT CBody_Khazan_Spear::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableEdge", &m_isEnableEdge)))
        return E_FAIL;

    _float fEdgeIntensity = 0.3f;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fEdgeIntensity", &fEdgeIntensity, sizeof(_float))))
        return E_FAIL;

    _float fShadeIntensity = 0.2f;
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

    if (FAILED(m_pShaderCom->Bind_Bool("g_isEnableEdge", &m_isEnableEdge)))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    for (auto pModel : m_RenderParts)
        Render_Part(pModel);

    _float4 eyeWhite = _float4(1.0f, 0.95f, 0.95f, 1.0f);
    _float4 pupilCircle = _float4(0.65f, 0.05f, 0.05f, 1.0f);
    _float4 pupilLens = _float4(0.85f, 0.15f, 0.15f, 1.0f);
    _float4 pupilRing = _float4(0.35f, 0.0f, 0.0f, 1.0f);
    _float4 shadingColor = _float4(1.0f, 0.4f, 0.4f, 1.0f);
    _float  pupilScale = 0.4f;

    m_pShaderCom->Bind_RawValue("g_vEyeWhiteColor", &eyeWhite, sizeof(_float4));
    m_pShaderCom->Bind_RawValue("g_vPupilCircle", &pupilCircle, sizeof(_float4));
    m_pShaderCom->Bind_RawValue("g_vPupilLens", &pupilLens, sizeof(_float4));
    m_pShaderCom->Bind_RawValue("g_vPupilRing", &pupilRing, sizeof(_float4));
    m_pShaderCom->Bind_RawValue("g_vShadingColor", &shadingColor, sizeof(_float4));
    m_pShaderCom->Bind_RawValue("g_PupilScale", &pupilScale, sizeof(_float));
    m_pShaderCom->Begin(23);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Render_Shadow()
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
    }

    for (auto pModel : m_RenderParts)
        Render_Part_Shadow(pModel);


    return S_OK;
}

HRESULT CBody_Khazan_Spear::Render_Outline()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutlineColor", &m_OutlineConfig.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutlineSize", &m_OutlineConfig.fSize, sizeof(_float))))
        return E_FAIL;

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
    }

    for (auto pModel : m_RenderParts)
        Render_Part_Outline(pModel);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Render_MotionVector()
{
    //  if (FAILED(Bind_ShaderResources()))
    //      return E_FAIL;

    // ?ù¥?†Ñ ?îÑ?†à?ûÑ ?õî?ìú, Î∑?, ?à¨?òÅ Î∞îÏù∏?ìú ?ï¥Ï§òÏïº ?ï®
    if (FAILED(m_pTransformCom->Bind_PrevWorldMatrix(m_pShaderCom, "g_PrevWorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", m_pGameInstance->Get_PrevTransform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    _uint       iNumViewports = { 1 };
    D3D11_VIEWPORT      ViewportDesc{};

    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
    _float2 vScreenSize = _float2(ViewportDesc.Width, ViewportDesc.Height);
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vScreenSize", &vScreenSize, sizeof(_float2))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    for (auto pModel : m_RenderParts)
        Render_Part_MotionVector(pModel);


    return S_OK;
}

void CBody_Khazan_Spear::Render_Part(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _bool isExclusivePartBones = m_pClientInstance->Get_PlayerEquipment().iLeg != 5009;
    pModel->Update_PartLocalBones_Once(isExclusivePartBones);
    //pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);

        // ÎßàÏä§?Ñ∞?ùò Î≥∏ÏùÑ ?ûê?èô?úºÎ°? ?Ç¨?ö©
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(28);
        pModel->Render(i);
    }
}

void CBody_Khazan_Spear::Render_Part_Shadow(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // ÎßàÏä§?Ñ∞?ùò Î≥∏ÏùÑ ?ûê?èô?úºÎ°? ?Ç¨?ö©
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(2);
        pModel->Render(i);
    }
}

void CBody_Khazan_Spear::Render_Part_Outline(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // ÎßàÏä§?Ñ∞?ùò Î≥∏ÏùÑ ?ûê?èô?úºÎ°? ?Ç¨?ö©
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(3);
        pModel->Render(i);
    }
}

void CBody_Khazan_Spear::Render_Part_MotionVector(CModel* pModel)
{
    if (nullptr == pModel)
        return;

    _bool isExclusivePartBones = m_pClientInstance->Get_PlayerEquipment().iLeg != 5009;
    pModel->Update_PartLocalBones_Once(isExclusivePartBones);
    //pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // ÎßàÏä§?Ñ∞?ùò Î≥∏ÏùÑ ?ûê?èô?úºÎ°? ?Ç¨?ö©
        if (FAILED(pModel->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", i)))
            continue;

        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        // ?Ö∞?ù¥?çî Î∞îÍøî?ïº ?ï®
        //  m_pShaderCom->Begin(11);
        pModel->Render(i);
    }
}

void CBody_Khazan_Spear::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        /* Í≥µÍ≤© ÏΩúÎùº?ù¥?çî */
        if (m_isSpearTipActive && pMyDesc->strName == TEXT("AttackCollisionDesc"))
        {
            CCreature* pMonster = static_cast<CCreature*>(pDesc->pGameObject);
            if (pMonster == nullptr || pMonster->Get_CurrentHP() < 0.f)
                return;

            pMonster->Take_Damage(m_pPlayerData->fBonusDamage, static_cast<HITREACTION>(*m_pHitReaction), this);
            //pMonster->Take_Damage(m_pPlayerData->fDamage , static_cast<HITREACTION>(*m_pHitReaction), nullptr);

            /* Î∏åÎ£®?Éà ?ñ¥?Éù??? ?ÑâÎ∞? ÎßâÍ∏∞ */
            _uint iBrutalAtkAnimIndex[2] = { m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_GrappleAtk01"),m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_GrappleAtk02") };
            _uint iCurAnimIndex = m_pModelCom->Get_CurAnimIndex();
            if (iCurAnimIndex != iBrutalAtkAnimIndex[0] && iCurAnimIndex != iBrutalAtkAnimIndex[1])
                pMonster->KnockBack(
                    XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                        - m_pParentTransform->Get_State(STATE::POSITION))
                    , 15.f, 50.f);

            pMonster->Consume_Stamina(50.f);
            m_isCollision = true;
            CTransform* MonsterTransform = dynamic_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")));
            XMStoreFloat4(&m_fCollisionPos, MonsterTransform->Get_State(STATE::POSITION));

            m_pPlayerData->fCulDoggedness = m_pPlayerData->fCulDoggedness + 0.2f > m_pPlayerData->iMaxDoggednessCount ?
                m_pPlayerData->iMaxDoggednessCount : m_pPlayerData->fCulDoggedness + 0.2f;

        }

        /* Î∞îÎîî Í≥µÍ≤© ÏΩúÎùº?ù¥?çî  */
        if (m_isBodyAttackActive && pMyDesc->strName == TEXT("Player_BodyAttack"))
        {
            CCreature* pMonster = static_cast<CCreature*>(pDesc->pGameObject);
            if (pMonster == nullptr || pMonster->Get_CurrentHP() < 0.f)
                return;

            pMonster->Take_Damage(m_pPlayerData->fBonusDamage, static_cast<HITREACTION>(*m_pHitReaction), this);

            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 10.f, 35.f);

            pMonster->Consume_Stamina(50.f);
            m_isCollision = true;
            CTransform* MonsterTransform = dynamic_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")));
            XMStoreFloat4(&m_fCollisionPos, MonsterTransform->Get_State(STATE::POSITION));
            m_pPlayerData->fCulDoggedness = m_pPlayerData->fCulDoggedness + 0.2f > m_pPlayerData->iMaxDoggednessCount ?
                m_pPlayerData->iMaxDoggednessCount : m_pPlayerData->fCulDoggedness + 0.2f;

        }


        /*  ?ÉêÏß? */
        if (pMyDesc->strName == TEXT("Player_Search"))
        {
            CGameObject* pObj = pDesc->pGameObject;
            if (!pObj || pObj->Get_IsDead()) return;
            lock_guard<mutex> lock(m_CollMonsterMutex);
            if (pObj && (find(m_CollMonsters.begin(), m_CollMonsters.end(), pObj) == m_CollMonsters.end()))
                m_CollMonsters.push_back(pObj);
        }


    }

    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
    {
        /* Î∞©Ïñ¥ ÏΩúÎùº?ù¥?çî  */
        if (m_isSpearPoleActive && pMyDesc->strName == TEXT("GuardCollisionDesc"))
        {
            _matrix mat = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
            *m_pParentStatus |= CKhazan_Spear::GUARD;

            /* ????ä§?ä∏ Í∞??ìú ????ù¥Î∞? */
            if (!m_isJustGuardOnce && m_fJustGuardTime.x <= m_fJustGuardTime.y) {
                *m_pParentStatus |= CKhazan_Spear::JUST_GUARD;
                m_isJustGuardOnce = true;

                /* Î™¨Ïä§?Ñ∞?ïú?Öå ????ä§?ä∏ Í∞??ìú ????ù¥Î∞? Í±¥ÎÇ¥Ï£ºÍ∏∞  */
                if (pDesc->pGameObject == nullptr) return;

                CWeaponObject* pMonster = dynamic_cast<CWeaponObject*>(pDesc->pGameObject);

                if (pMonster == nullptr)
                    return;

                m_bGuradFX[0] = true;

                pMonster->On_JustGuardCallback(true);

                // Vignette
                VIGNETTE_CONFIG Config{};
                Config.vColor = _float3(0.f, 0.f, 0.f);
                Config.fPower = 3.5f;
                Config.fMinIntensity = 0.f;
                Config.fMaxIntensity = 4.f;
                Config.fDuration = 0.5f;
                Config.vFadeTime = _float2(0.25f, 0.25f);
                m_pGameInstance->Start_VignetteAnimation(Config);

                // ?ïë?Å¨ Î≥¥Îùº Ï°∞Î™Ö
                LIGHT_TRANSITION_DESC LightDesc{};
                LightDesc.fDuration = 0.5f;
                LightDesc.vFadeTime = _float2(0.2f, 0.2f);
                LightDesc.vDiffuse = _float4(0.5f, 0.f, 1.f, 1.f);
                LightDesc.vAmbient = _float4(0.5f, 0.f, 1.f, 1.f);
                LightDesc.vSpecular = LightDesc.vDiffuse;
                LightDesc.isReturnToStart = true;
                //LightDesc.Callback = [&]() { m_pGameInstance->Set_LightEnable(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false); };
                m_pGameInstance->Start_LightTransition(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

                // FOV Ï§åÏù∏?ïÑ?õÉ
                FOVModifier tMod{};
                tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
                tMod.fDuration = 0.25f;
                tMod.fFrom = XMConvertToRadians(60.f);
                tMod.fTo = XMConvertToRadians(50.f);
                tMod.iPriority = 1.f;
                tMod.Ease = EaseOutQuad;
                m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

                //?ûà?ä∏?ä§?Éë
                m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.2f, 0.003f, 0.6f);
                m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.2f, 0.003f, 0.6f);
                m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.2f, 0.003f, 0.6f);

            }

            m_bGuradFX[1] = true;

            // Í∑∏ÎÉ• ?Ö∏??? Ï°∞Î™Ö
            LIGHT_TRANSITION_DESC LightDesc{};
            LightDesc.fDuration = 0.5f;
            LightDesc.vFadeTime = _float2(0.2f, 0.2f);
            LightDesc.vDiffuse = _float4(1.f, 1.f, 0.8f, 1.f);
            LightDesc.vAmbient = _float4(1.f, 1.f, 0.8f, 1.f);
            LightDesc.vSpecular = LightDesc.vDiffuse;
            LightDesc.isReturnToStart = true;
            //LightDesc.Callback = [&]() { m_pGameInstance->Set_LightEnable(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false); };
            m_pGameInstance->Start_LightTransition(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

            /* Í∞??ìú?õÑ Ï∂©Îèå?êòÎ©? Ï∂©Îèå?êú Ïß??†ê Î¥êÎùºÎ≥¥Í≤å*/
            Start_GuardRotation(vContactPoint);

            /* ?Ç¨?ö¥?ìú */
            m_isHitSound = true;

            /* ?ä§?ÉúÎØ∏ÎÇò Í∞êÏÜå */
            /* ?ä§?ÉúÎØ∏ÎÇò Í∞êÏÜå */
            m_pPlayerData->fCulStamina -= m_pPlayerData->fUsedStamina * 0.3f;
            *m_pParentIsCanStaminaRecovery = false;
            return;
        }

        /* damaged */

    }

}

void CBody_Khazan_Spear::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CBody_Khazan_Spear::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER) && pMyDesc->strName == TEXT("Player_Search")) {
        CGameObject* pObj = pDesc->pGameObject;

        if (!pObj) return;

        lock_guard<mutex> lock(m_CollMonsterMutex);

        auto it = remove(m_CollMonsters.begin(), m_CollMonsters.end(), pObj);
        if (it != m_CollMonsters.end()) m_CollMonsters.erase(it, m_CollMonsters.end());
    }
}

void CBody_Khazan_Spear::Search_BrutalTarget(_float fTimeDelta)
{
    /* Î∏åÎ£®?Éà Í∞úÏ≤¥?äî ?ïò?ÇòÎß? */
    if (Has_Status(CKhazan_Spear::BRUTAL_BEGIN))
        return;

    //m_fOptimizationSearchTime.x += fTimeDelta;

    //if (m_fOptimizationSearchTime.x < m_fOptimizationSearchTime.y)
    //    return;

    if (m_isBrutalSuccess)
    {
        m_fOptimizationSearchTime.y = 0.3f;
        m_isBrutalSuccess = false;
    }

    m_fOptimizationSearchTime.x = 0.f;

    _vector vPlayerPos = XMVectorSet(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43, 1.f);

    lock_guard<mutex> lock(m_CollMonsterMutex);
    for (CGameObject* monster : m_CollMonsters)
    {
        CMonster* pCreatureMoster = static_cast<CMonster*>(monster);

        if (!monster || !monster->Get_IsActive() || monster->Get_IsDead() || pCreatureMoster->Get_CurrentHP() <= 0.f)
            continue;

        _vector vMonsterPos = monster->Get_Position();

        _vector  vDiff = vPlayerPos - vMonsterPos;
        _float  fDistSq = XMVectorGetX(XMVector3LengthSq(vDiff));
        /* ?ùº?†ï Î≤îÏúÑ?óê ?ã§Í∞?Í∞?Î©?  */
        if (fDistSq < 15.f * 15.f)
        {
            /* ?õÑÎ∞? */
            if (!pCreatureMoster->Get_isSleep()) {
                _float fDot = XMVectorGetX(XMVector3Dot(XMVector3Normalize(monster->Get_Look()), XMVector3Normalize(vDiff)));

                if (-0.7f > fDot)
                {
                    m_pBrutalAttack = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
                    m_pBrutalAttack->Setting_BrutalAttack(pCreatureMoster->Get_LockOnPosition(), 0.f, { 0.f, 0.f });
                    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pBrutalAttack);

                    m_pBrutalmonster = monster;
                    m_isBackBrutal = true;
                    m_isGroggyBrutal = false;

                    Add_Status(CKhazan_Spear::BRUTAL_BEGIN);

                    return;
                }
            }

            /*  Î™¨Ïä§?Ñ∞ Í∑∏Î°úÍ∏? ?ÉÅ?Éú */
            if (pCreatureMoster->Get_IsGroggy())
            {
                m_pBrutalmonster = monster;
                m_isBackBrutal = false;
                m_isGroggyBrutal = true;

                Add_Status(CKhazan_Spear::BRUTAL_BEGIN);

                return;
            }

        }
    }
}

_bool CBody_Khazan_Spear::Check_BrutalAttack(_float fTimeDelta)
{
    /* Ïª®ÌÖå?ù¥?Ñà Ï≤¥ÌÅ¨  */
    lock_guard<mutex> lock(m_CollMonsterMutex);
    for (auto it = m_CollMonsters.begin(); it != m_CollMonsters.end(); )
    {
        CMonster* pCreatureMoster = static_cast<CMonster*>(m_pBrutalmonster);
        if (*it == m_pBrutalmonster && (m_pBrutalmonster->Get_IsDead() || m_pBrutalmonster->Get_IsActive() || pCreatureMoster->Get_CurrentHP() <= 0.f))
            it = m_CollMonsters.erase(it);
        else
            ++it;
    }

    /* Î≤îÏúÑ ?Ç¥?óê Î∏åÎ£®?Éà Í∞??ä• Í∞úÏ≤¥Í∞? ?óÜ?úºÎ©?  */
    if (!Has_Status(CKhazan_Spear::BRUTAL_BEGIN)) {
        return false;
    }

    /* Î∏åÎ£®?Éà ?ñ¥?Éù ?Ñ±Í≥? ?õÑ ?ïÑ?ù¥ÏΩ? Ïß??ö∞Í∏? */
    if (Has_Status(CKhazan_Spear::BRUTAL_SUCCESS))
    {
        Remove_Status(CKhazan_Spear::BRUTAL_BEGIN | CKhazan_Spear::BRUTAL_READY | CKhazan_Spear::BRUTAL_SUCCESS);
        if (m_isBackBrutal) {
            m_pBrutalAttack->Off_BrutalAttack();
            m_isBackBrutal = false;
            m_fOptimizationSearchTime.y = 2.f;
            m_isBrutalSuccess = true;
        }

        return false;
    }

    /* Î™¨Ïä§?Ñ∞Í∞? Ï£ΩÏúºÎ©?  */
    if (!m_pBrutalmonster || m_pBrutalmonster->Get_IsDead()) {
        Remove_Status(CKhazan_Spear::BRUTAL_BEGIN | CKhazan_Spear::BRUTAL_READY | CKhazan_Spear::BRUTAL_SUCCESS);
        if (m_isBackBrutal) {
            m_pBrutalAttack->Off_BrutalAttack();
            m_isBackBrutal = false;
            m_fOptimizationSearchTime.y = 2.f;
            m_isBrutalSuccess = true;

        }
        return false;
    }

    /*  Î™¨Ïä§?Ñ∞Í∞? ?ä¨Î¶ΩÏù¥ ???Î¶¨Î©¥ */
    if (static_cast<CMonster*>(m_pBrutalmonster)->Get_isSleep() == true && m_isBackBrutal)
    {
        Remove_Status(CKhazan_Spear::BRUTAL_BEGIN | CKhazan_Spear::BRUTAL_READY | CKhazan_Spear::BRUTAL_SUCCESS);
        if (m_isBackBrutal) {
            m_pBrutalAttack->Off_BrutalAttack();
            m_isBackBrutal = false;
            m_fOptimizationSearchTime.y = 2.f;
            m_isBrutalSuccess = true;
        }
        return false;
    }

    /* Î∏åÎ£®?Éà Í∞??ä• Î≤îÏúÑ?ù∏Ïß? ?ïÑ?ãåÏß? Ï≤¥ÌÅ¨ */
    _float  fDistSq = XMVectorGetX(XMVector3LengthSq(XMVectorSet(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43, 1.f) - m_pBrutalmonster->Get_Position()));
    if (fDistSq < 15.f * 15.f) {
        if (!Has_Status(CKhazan_Spear::BRUTAL_READY)) {
            Add_Status(CKhazan_Spear::BRUTAL_READY);
            return true;
        }
    }
    else if (fDistSq > 15.f * 15.f + 1.f)
        if (Has_Status(CKhazan_Spear::BRUTAL_READY))
        {
            if (m_isBackBrutal)
                m_pBrutalAttack->Off_BrutalAttack();

            m_pBrutalAttack = nullptr;
            m_pBrutalmonster = nullptr;
            m_isBackBrutal = false;
            m_isGroggyBrutal = false;
            Remove_Status(CKhazan_Spear::BRUTAL_READY | CKhazan_Spear::BRUTAL_BEGIN);

        }


    return false;
}

const MOTIONTRAIL_CONFIG& CBody_Khazan_Spear::Get_MotionTrailConfig()
{
    return m_pMotionTrailCom->Get_Config();
}

void CBody_Khazan_Spear::Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config)
{
    m_pMotionTrailCom->Set_Config(Config);
}

void CBody_Khazan_Spear::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

_bool CBody_Khazan_Spear::isEnableMotionTrail()
{
    return m_pMotionTrailCom->isEnable();
}

void CBody_Khazan_Spear::Start_MotionTrail(_float fDuration)
{
    m_pMotionTrailCom->Start_MotionTrail(fDuration);
}

void CBody_Khazan_Spear::Start_HealRimLight(_float fDuration, const _float2& vFadeTime, _float fMaxIntensity)
{
    if (true == m_isFinishedHealRimLight)
        return;

    m_isEnableHealRimLight = true;
    m_HealRimLightDesc.fDuration = fDuration;
    m_HealRimLightDesc.vFadeTime = vFadeTime;
    m_HealRimLightDesc.vFadeTime.y = m_HealRimLightDesc.fDuration - m_HealRimLightDesc.vFadeTime.y;
    m_HealRimLightDesc.fTargetIntensity = fMaxIntensity;
}

const TRAIL_CONFIG& CBody_Khazan_Spear::Get_TrailConfig() const
{
    return m_pTrail->Get_TrailConfig();
}

void CBody_Khazan_Spear::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_pTrail->Set_TrailConfig(Config);
}

_uint CBody_Khazan_Spear::Get_NumTrailTextures()
{
    return m_pTrail->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CBody_Khazan_Spear::Get_TrailTexture(_uint iIndex)
{
    return m_pTrail->Get_TrailTexture(iIndex);
}

void CBody_Khazan_Spear::AllAttackCollisionActive_Off()
{
    m_isNotifyAttacking = false;
}

void CBody_Khazan_Spear::Update_Collider(_float fTimeDelta)
{
    _matrix matParent = XMLoadFloat4x4(m_pParentMatrix);
    _vector vOutQuat, vOutPos;

    const XMMATRIX matWorld_SpearTip1 = XMLoadFloat4x4(m_pSpearTip1_Matrix) * matParent;
    XMStoreFloat4x4(&m_pSpearTip1_MatrixW_nJolt, matWorld_SpearTip1);
    m_pBodyCom_SpearTip1->Sync_Update(matWorld_SpearTip1);
    m_pBodyCom_SpearTip1->Update(fTimeDelta, matWorld_SpearTip1, vOutQuat, vOutPos);
    XMStoreFloat4x4(&m_pSpearTip1_MatrixW, matWorld_SpearTip1);
    XMStoreFloat3(reinterpret_cast<_float3*>(&m_pSpearTip1_MatrixW._41), vOutPos);

    _vector vOutQuat2, vOutPos2;
    const XMMATRIX matWorld_SpearPole = m_SpearOffset_Matrix * XMLoadFloat4x4(m_pSpearPole_Matrix) * matParent;
    XMStoreFloat4x4(&m_pSpearPole_MatrixW, matWorld_SpearPole);

    m_pBodyCom_SpearPole->Sync_Update(matParent);

    m_pBodyCom_Search->Sync_Update(matParent);

    m_pBodyCom_BodyAttack->Sync_Update(matParent);

}
void CBody_Khazan_Spear::Check_Guarding(_float fTimeDelta)
{
    if (m_isSpearPoleActive)
        m_fJustGuardTime.x += fTimeDelta;

    if (*m_pIsGuarding == true && !m_isSpearPoleActive) {

        m_isSpearPoleActive = true;
        m_fJustGuardTime.x = 0.f;
        m_isJustGuardOnce = false;
    }
    if (*m_pIsGuarding == false && m_isSpearPoleActive) {

        m_isSpearPoleActive = false;
    }

}

void CBody_Khazan_Spear::Update_GuardRotation(_float fTimeDelta)
{
    /* ?Ç¨?ö© xx Î∂?Î™®Ïóê?Ñú ÏßÅÏ†ë Ï≤òÎ¶¨?ï†Í±∞ÏûÑ !! */

    //if (!m_isGuardRotating)
    //    return;

    //m_fGuardRotationTime += fTimeDelta;

    //// ?öå?†Ñ ?ãúÍ∞ÑÏù¥ ?Åù?Ç¨?úºÎ©? ?†ï?ôï?ûà Î™©Ìëú Î∞©Ìñ•?úºÎ°? ?Ñ§?†ï
    //if (m_fGuardRotationTime >= m_fGuardRotationDuration)
    //{
    //    m_isGuardRotating = false;

    //    // ?†ï?ôï?ûà Î™©Ìëú Î∞©Ìñ•?úºÎ°? ?Ñ§?†ï
    //    _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_vTargetRotationDir));
    //    _vector vUp = XMVector3Normalize(XMVector3Cross(m_vTargetRotationDir, vRight));

    //    m_pParentTransform->Set_State(STATE::RIGHT, vRight);
    //    m_pParentTransform->Set_State(STATE::UP, vUp);
    //    m_pParentTransform->Set_State(STATE::LOOK, m_vTargetRotationDir);

    //    return;
    //}

    //// Ease-Out Î≥¥Í∞Ñ (Ï≤òÏùå?óî Îπ†Î•¥Í≤?, ?Åù?úºÎ°? Í∞àÏàòÎ°? Ï≤úÏ≤ú?ûà)
    //_float fRatio = m_fGuardRotationTime / m_fGuardRotationDuration;
    //fRatio = 1.f - (1.f - fRatio) * (1.f - fRatio); // Ease-Out Quadratic

    //// ?òÑ?û¨ Í∞ÅÎèÑ Í≥ÑÏÇ∞
    //_float fCurrentAngle = m_fStartAngle + (m_fTargetAngle - m_fStartAngle) * fRatio;

    //// Î∞©Ìñ• Î≤°ÌÑ∞ ?Éù?Ñ±
    //_vector vNewLook = XMVectorSet(cosf(fCurrentAngle), 0.f, sinf(fCurrentAngle), 0.f);
    //vNewLook = XMVector3Normalize(vNewLook);

    //// Right, Up Î≤°ÌÑ∞ Í≥ÑÏÇ∞
    //_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNewLook));
    //_vector vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

    //// Transform?óê ?†Å?ö©
    //m_pParentTransform->Set_State(STATE::RIGHT, vRight);
    //m_pParentTransform->Set_State(STATE::UP, vUp);
    //m_pParentTransform->Set_State(STATE::LOOK, vNewLook);
}

void CBody_Khazan_Spear::Start_GuardRotation(_float3 vContactPoint)
{
    // 1. Ï∫êÎ¶≠?Ñ∞?ùò ?úÑÏπ?
    _vector vCharacterPos = m_pParentTransform->Get_State(STATE::POSITION);

    // 2. Ï∫êÎ¶≠?Ñ∞ -> ?†ëÏ¥âÏ†ê Î∞©Ìñ• Î≤°ÌÑ∞ Í≥ÑÏÇ∞
    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vCharacterPos;
    vHitDir = XMVectorSetY(vHitDir, 0.f); // XZ ?èâÎ©¥ÏúºÎ°? ?à¨?òÅ
    if (XMVectorGetX(XMVector3Length(vHitDir)) < 1e-4f)
    {
        vHitDir = m_pParentTransform->Get_State(STATE::LOOK);
        vHitDir = XMVectorSetY(vHitDir, 0.f);
        vHitDir = XMVector3Normalize(vHitDir);
    }
    else
        vHitDir = XMVector3Normalize(vHitDir);

    /* Î∂?Î™®Ïóê?Ñú ÏßÅÏ†ë ?öå?†Ñ */
    *m_pParentStatus |= CKhazan_Spear::GUARD_ROTATION_REQUEST;
    XMStoreFloat4(m_pGuardRotationTarget, vHitDir);

    //// 3. ?òÑ?û¨ Ï∫êÎ¶≠?Ñ∞?ùò Forward Î∞©Ìñ•
    //_vector vCurrentForward = m_pParentTransform->Get_State(STATE:: LOOK);
    //vCurrentForward = XMVectorSetY(vCurrentForward, 0.f);
    //vCurrentForward = XMVector3Normalize(vCurrentForward);

    //// 4. ?òÑ?û¨ Í∞ÅÎèÑ??? Î™©Ìëú Í∞ÅÎèÑ Í≥ÑÏÇ∞
    //m_fStartAngle = atan2f(XMVectorGetZ(vCurrentForward), XMVectorGetX(vCurrentForward));
    //m_fTargetAngle = atan2f(XMVectorGetZ(vHitDir), XMVectorGetX(vHitDir));

    //// 5. ÏµúÎã® Í±∞Î¶¨Î°? ?öå?†Ñ?ïò?èÑÎ°? Í∞ÅÎèÑ Î≥¥Ï†ï (-?? ~ ??)
    //_float fAngleDiff = m_fTargetAngle - m_fStartAngle;
    //if (fAngleDiff > XM_PI)
    //    fAngleDiff -= XM_2PI;
    //else if (fAngleDiff < -XM_PI)
    //    fAngleDiff += XM_2PI;

    //m_fTargetAngle = m_fStartAngle + fAngleDiff;

    //// 6. ?öå?†Ñ ?ãú?ûë
    //m_isGuardRotating = true;
    //m_fGuardRotationTime = 0.f;
    //m_vTargetRotationDir = vHitDir;

    //cout << "=== Guard Rotation Start ===" << endl;
    //cout << "Character Pos: " << XMVectorGetX(vCharacterPos) << ", "
    //    << XMVectorGetY(vCharacterPos) << ", "
    //    << XMVectorGetZ(vCharacterPos) << endl;
    //cout << "Contact Point: " << vContactPoint.x << ", "
    //    << vContactPoint.y << ", " << vContactPoint.z << endl;

    //_vector vHitDirDebug = XMLoadFloat3(&vContactPoint) - vCharacterPos;
    //cout << "HitDir Before Y=0: " << XMVectorGetX(vHitDirDebug) << ", "
    //    << XMVectorGetY(vHitDirDebug) << ", "
    //    << XMVectorGetZ(vHitDirDebug) << endl;

    //_float fLength = XMVectorGetX(XMVector3Length(vHitDir));
    //cout << "HitDir Length: " << fLength << endl;
    //if (fLength < 1e-4f)
    //    cout << "HitDir too small! Using Look direction" << endl;
    //cout << "Start Angle: " << XMConvertToDegrees(m_fStartAngle) << "¬∞" << endl;
    //cout << "Target Angle: " << XMConvertToDegrees(m_fTargetAngle) << "¬∞" << endl;
    //cout << "Angle Diff: " << XMConvertToDegrees(fAngleDiff) << "¬∞" << endl;
    //cout << "Guard Rotation Started!" << endl;
    //cout << "===========================" << endl;

}

FMOD_CHANNEL** CBody_Khazan_Spear::Get_SoundChannel(_int iIndex)
{
    if (m_pChannel.size() <= iIndex)
        m_pChannel.resize(iIndex + 1, nullptr);

    return &m_pChannel[iIndex];
}


HRESULT CBody_Khazan_Spear::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_Spear"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), nullptr)))
        return E_FAIL;


    struct PartInfo {
        wstring strPartName;
        wstring strPrototype;
    };

    vector<PartInfo> partInfos = {
        /* Nude Set */
     { TEXT("Nude_Face"), TEXT("Prototype_Component_Model_Khazan_Nude_Face") },
     { TEXT("Nude_Hair1"), TEXT("Prototype_Component_Model_Khazan_Nude_Hair1") },
     { TEXT("Nude_Hair2"), TEXT("Prototype_Component_Model_Khazan_Nude_Hair2") },
     { TEXT("Nude_Torso"), TEXT("Prototype_Component_Model_Khazan_Nude_Torso") },
     { TEXT("Nude_Arm"), TEXT("Prototype_Component_Model_Khazan_Nude_Arm") },
     { TEXT("Nude_Leg"), TEXT("Prototype_Component_Model_Khazan_Nude_Leg") },
     { TEXT("Nude_Shoes"), TEXT("Prototype_Component_Model_Khazan_Nude_Shoes") },

     /* Injured Set */
     { TEXT("Prisoner_Face1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Face1") },
     { TEXT("Prisoner_Hair1"), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair") },
     { TEXT("Prisoner_Torso1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Torso3") },
     { TEXT("Prisoner_Arm1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Arm1") },
     { TEXT("Prisoner_Leg3"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Leg3") },
     { TEXT("Prisoner_Shoes1"), TEXT("Prototype_Component_Model_Khazan_Prisoner_Shoes1") },

     /* Thief Set */
     { TEXT("Thief_Face"), TEXT("Prototype_Component_Model_Khazan_Nude_Face") },
     { TEXT("Thief_Hair"), TEXT("Prototype_Component_Model_Khazan_Thief_Hair") },
     { TEXT("Thief_Torso"), TEXT("Prototype_Component_Model_Khazan_Thief_Torso") },
     { TEXT("Thief_Arm"), TEXT("Prototype_Component_Model_Khazan_Thief_Arm") },
     { TEXT("Thief_Leg"), TEXT("Prototype_Component_Model_Khazan_Thief_Leg") },
     { TEXT("Thief_Shoes"), TEXT("Prototype_Component_Model_Khazan_Thief_Shoes") },

     /* ShadowLandFlow Set */
     { TEXT("ShadowLandFlow_Hair"), TEXT("Prototype_Component_Model_Khazan_ShadowLandFlow_Hair") },
     { TEXT("ShadowLandFlow_Torso"), TEXT("Prototype_Component_Model_Khazan_ShadowLandFlow_Torso") },
     { TEXT("ShadowLandFlow_Arm"), TEXT("Prototype_Component_Model_Khazan_ShadowLandFlow_Arm") },
     { TEXT("ShadowLandFlow_Leg"), TEXT("Prototype_Component_Model_Khazan_ShadowLandFlow_Leg") },
     { TEXT("ShadowLandFlow_Shoes"), TEXT("Prototype_Component_Model_Khazan_ShadowLandFlow_Shoes") },

     /* Event Clothes*/
     { TEXT("Danjin_Hair"), TEXT("Prototype_Component_Model_Khazan_DanJin_Hair") },
     
    };

    // Î™®Îì† ?ååÏ∏? Î°úÎìú
    for (const auto& info : partInfos)
    {
        CModel* pModel = nullptr;
        wstring strComponentTag = TEXT("Com_PartModel_") + info.strPartName;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), info.strPrototype,
            strComponentTag, reinterpret_cast<CComponent**>(&pModel), nullptr)))
            return E_FAIL;

        m_AllParts[info.strPartName] = pModel;
    }

    CMeshTrail::TRAIL_DESC MeshDsc;
    MeshDsc.iTextureIdx = 9;
    MeshDsc.fLifeTime = .25f;
    MeshDsc.iDivisionCount = 10.f;
    MeshDsc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    m_pTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDsc));

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = true;

    for (auto pModel : m_RenderParts)
        MTDesc.OwnerPartModels.push_back(pModel);

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

HRESULT CBody_Khazan_Spear::Ready_AnimationEvent()
{
#pragma region Effect
    m_pModelCom->Register_Event("FastAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {UpdateSpearWind(true); FX_Trail(); });
    m_pModelCom->Register_Event("FastAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {SpawnSpearWind(); });
    m_pModelCom->Register_Event("FastAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {UpdateSpearWind(true); FX_Trail(); });
    m_pModelCom->Register_Event("FastAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {SpawnSpearWind(); });
    m_pModelCom->Register_Event("FastAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {UpdateSpearWind(true); FX_Trail(); });
    m_pModelCom->Register_Event("FastAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {SpawnSpearWind(); });

    m_pModelCom->Register_Event("StrongAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_BlueTrail(); });
    m_pModelCom->Register_Event("StrongAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    m_pModelCom->Register_Event("StrongAtk_Charge_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    m_pModelCom->Register_Event("StrongAtk_Charge_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk_Charge_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    m_pModelCom->Register_Event("StrongAtk_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        FX_StrongAtk_Charge_Blust1(m_pParentTransform->Get_WorldMatrix().r[3]);

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.2f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });
    m_pModelCom->Register_Event("StrongAtk_Charge_Stamp", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {   
        _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt);
        _matrix W_withOffset = XMMatrixTranslation(-1.f, 0.f, 1.f) * W;
        _vector V_FinalPosition = W_withOffset.r[3];
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Stamp"), V_FinalPosition); }
    );

    /*Î≥¥Î¶Ñ?ã¨ ?ä∏?†à?ùº*/
    m_pModelCom->Register_Event("Full_Moon_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        Set_BlueTrail();

        FOVModifier tMod{};
        tMod.strID = TEXT("FullMoon");
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });
    m_pModelCom->Register_Event("Full_Moon_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("Full_Moon_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Set_BaseTrail();
        m_pClientInstance->ActiveCamera_KillFov(TEXT("FullMoon"));
        });
    /*Î≥¥Î¶Ñ?ã¨ Blust*/
    m_pModelCom->Register_Event("Full_Moon_Spike0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_StrongAtk_Charge_Blust3(m_pParentTransform->Get_WorldMatrix().r[3]); });
    m_pModelCom->Register_Event("Full_Moon_Spike1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Spear_Spike(); });
    /*?ã¨Îπ? Î≤†Í∏∞*/
    m_pModelCom->Register_Event("LightningSpear_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    m_pModelCom->Register_Event("LightningSpear_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {FX_Trail(); });
    m_pModelCom->Register_Event("LightningSpear_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });
    m_pModelCom->Register_Event("LightningSpear_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {FX_StrongAtk_Charge_Blust6(m_pParentTransform->Get_WorldMatrix().r[3]); });
    /*?Çò?Ñ† Ï∞åÎ•¥Í∏?*/
    m_pModelCom->Register_Event("SpiralSpear_Spike_Tmp", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        EffectID_SpiralSpear = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Sphere_Blood"), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);

        FX_StrongAtk_Charge_Blust1(m_pParentTransform->Get_WorldMatrix().r[3]);

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    m_pModelCom->Register_Event("SpiralSpear_Spike_Tmp", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _matrix mat = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Sphere_Blood"), EffectID_SpiralSpear, mat, mat.r[3]);
        _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt);

        _vector S, Q, T;

        if (!XMMatrixDecompose(&S, &Q, &T, W))
        { 
            XMFLOAT4X4 m; XMStoreFloat4x4(&m, W); 
            _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
            _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
            _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f)); 

            _matrix RotationMatrix(
                r0,
                r1,
                r2,
                XMVectorSet(0.f, 0.f, 0.f, 1.f)
            );

            Q = XMQuaternionRotationMatrix(RotationMatrix);
        }
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiralSpear_SpearFX"), EffectID_SpiralSpear, Q, W.r[3]); 
        });

    m_pModelCom->Register_Event("SpiralSpear_Spike_Tmp_Stop", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiralSpear_SpearFX"), EffectID_SpiralSpear);
        });

    m_pModelCom->Register_Event("SpiralSpear_Spike1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //SpawnSpearWind();
        // Radial Blur

        RADIAL_BLUR_DESC RBDesc{};
        RBDesc.vCenterUV = _float2(0.5f, 0.5f);
        RBDesc.fSampleRadius = 0.05f;
        RBDesc.vMaskRadius = _float2(0.f, 0.3f);
        RBDesc.fExponent = 1.f;
        RBDesc.iNumSamples = 16;
        RBDesc.fAttenuation = 0.1f;
        RBDesc.fStrength = 0.5f;       // == Target Strength(0 ~ 1) -> ?ù¥ Í∞ïÎèÑÎ•? ÏµúÎ??Í∞íÏúºÎ°? ?Ç¨?ö©?ïò?ó¨ Î≥¥Í∞Ñ ?†Å?ö©?ê®
        RBDesc.fDuration = 1.2f;
        RBDesc.vFadeTime = _float2(0.25f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RBDesc);
        });

    m_pModelCom->Register_Event("SpiralSpear_Spike1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    m_pModelCom->Register_Event("SpiralSpear_Spike1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        UpdateSpearWind(false);
        FX_Trail();
        });

    m_pModelCom->Register_Event("SpiralSpear_Spike1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    m_pModelCom->Register_Event("SpaceTimeCutter_Trail0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_BlueTrail(); });
    m_pModelCom->Register_Event("SpaceTimeCutter_Trail0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("SpaceTimeCutter_Trail0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    /* Ï∞∞ÎÇò Î≤†Í∏∞ */
    m_pModelCom->Register_Event("SprintAtk_Fast_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_BlueTrail(); });
    m_pModelCom->Register_Event("SprintAtk_Fast_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("SprintAtk_Fast_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    /* Í∏âÏÜå ???Í≤? */
    m_pModelCom->Register_Event("SprintAtk_Strong_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_BlueTrail(); });
    m_pModelCom->Register_Event("SprintAtk_Strong_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("SprintAtk_Strong_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    /* ?Çò?Ñ† Ï∞åÎ•¥Í∏? */
    m_pModelCom->Register_Event("Tempest_Wind", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { UpdateSpearRedWind(true); });
    m_pModelCom->Register_Event("Tempest_Wind", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SpawnSpearRedWind();  });
    m_pModelCom->Register_Event("Tempest_SpiralSpear_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust11"), m_pParentTransform->Get_WorldMatrix().r[3]); });
    //        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust11"), m_pParentTransform->Get_WorldMatrix().r[3]);

    /* Í∏âÏÜå ???Í≤? */
    m_pModelCom->Register_Event("Crescent_Land", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Spear_Crescent_Land"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);
        });

    /* ?Çò?Ñ† Ï∞åÎ•¥Í∏? :: ?Üå?ö©?èå?ù¥*/

    m_pModelCom->Register_Event("Tempest_TwisterSpear_SphereWind_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { UpdateSpearRedWind(true); });
    m_pModelCom->Register_Event("Tempest_TwisterSpear_SphereWind_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { SpawnSpearRedWind();  });

    m_pModelCom->Register_Event("Tempest_TwisterSpear_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust11"), m_pParentTransform->Get_WorldMatrix().r[3]); });

    m_pModelCom->Register_Event("PureMind_Pin", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("JumpSpear"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]); });


    // ?ïΩÍ≥? ?ä§?Ç¨ ?ëêÎ≤àÏ?∞Í∫º
    m_pModelCom->Register_Event("FastBlust0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //?ã¨Îπ? ?Éú?Ñ∏ Î∞∞Ïõ†?úºÎ©?
        if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::SPEARSKILL::MOONLIGHT_STANCE))
            m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust5"), BodyCenter()); });

    m_pModelCom->Register_Event("FastBlust1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        //?ã¨Îπ? ?Éú?Ñ∏ Î∞∞Ïõ†?úºÎ©?
        if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::SPEARSKILL::MOONLIGHT_STANCE))
            m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust12"), BodyCenter()); });

    m_pModelCom->Register_Event("FastBlust2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust5"), BodyCenter()); });

    m_pModelCom->Register_Event("FastBlust3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Spear_Spike(); });

    m_pModelCom->Register_Event("FallATK_Land", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
                //m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Spear_FallAtk_Land"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW).r[3]);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.25f, 0.5f);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Spear_FallAtk_Land"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);

        });

    //brutal
    // m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //     _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
    //     //Decompose_Rotation(mat_arm, XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90));
    //     _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
    //     EffectID_SpiralSpear = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), world, world.r[3]);
    //     });
    // m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
    //     _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
    //     _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
    //     m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear, world, world.r[3]);
    //     });
    // m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //     m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear);
    //     m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutalParticle"), m_pParentTransform->Get_WorldMatrix(), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);
    //     m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);
    //     });

    // m_pModelCom->Register_Event("Brutal_FX1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //     m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);
    //     });

    // m_pModelCom->Register_Event("Brutal_FX1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //     m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust12"), m_pParentTransform->Get_WorldMatrix().r[3]);
    //     });

    // m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    // m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    // m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    // m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    // m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    // m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    //brutal
    //m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
    //    //Decompose_Rotation(mat_arm, XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90));
    //    _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
    //    EffectID_SpiralSpear = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), world, world.r[3]);
    //    });
    //m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
    //    _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
    //    _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
    //    m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear, world, world.r[3]);
    //    });
    //m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear);
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutalParticle"), m_pParentTransform->Get_WorldMatrix(), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);
    //    });

    //m_pModelCom->Register_Event("Brutal_FX1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);
    //    });

    //m_pModelCom->Register_Event("Brutal_FX1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust12"), m_pParentTransform->Get_WorldMatrix().r[3]);
    //    });

    //m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    //m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    //m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    //m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    //m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    //m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });


    //brutal
    m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
        //Decompose_Rotation(mat_arm, XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90));
        _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
        EffectID_SpiralSpear = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), world, world.r[3]);
        BrutalAtk_ScreenEffect0();
        });
    m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
        _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear, world, world.r[3]);
        });
    m_pModelCom->Register_Event("Brutal_FX0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutalParticle"), m_pParentTransform->Get_WorldMatrix(), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_pSpearPole_MatrixW).r[3]);
        });

    m_pModelCom->Register_Event("Brutal_FX1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("Brutal_FX1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust12"), m_pParentTransform->Get_WorldMatrix().r[3]);
        });

    // 0 : ?åî ?úòÍ∞êÎäî Í±? ?ãú?ûë ?óî?Ñ∞, ?Åù?Çò?äî Í±? ?óë?ãú?ä∏ ?Ñ∞Ïß??äî ?åå?ã∞?Å¥
    // 1 : Ï∞ΩÏóê?Ñú ?Ñ∞Ïß??äî Í≤? ?óî?Ñ∞ Î™∏Ïóê?Ñú ?Ñ∞Ïß??äîÍ≤? ?óë?ãú?ä∏

    m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_RedTrail(); });
    m_pModelCom->Register_Event("Brutal_Trail0", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        Set_RedTrail(); 
        BrutalAtk_ScreenEffect1();
        });
    m_pModelCom->Register_Event("Brutal_Trail1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

#pragma endregion

#pragma region ScreenEffect

    // Í∏âÏÜå ???Í≤?
    m_pModelCom->Register_Event("SprintAtk_Strong_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), true);

        RADIAL_BLUR_DESC RBDesc{};

        RBDesc.vCenterUV = _float2(0.5f, 0.5f);
        RBDesc.fSampleRadius = 0.05f;
        RBDesc.vMaskRadius = _float2(0.f, 0.3f);
        RBDesc.fExponent = 1.f;
        RBDesc.iNumSamples = 16;
        RBDesc.fAttenuation = 0.1f;
        RBDesc.fStrength = 0.4f;       // == Target Strength(0 ~ 1) -> ?ù¥ Í∞ïÎèÑÎ•? ÏµúÎ??Í∞íÏúºÎ°? ?Ç¨?ö©?ïò?ó¨ Î≥¥Í∞Ñ ?†Å?ö©?ê®
        RBDesc.fDuration = 1.2f;
        RBDesc.vFadeTime = _float2(0.25f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RBDesc);

        m_isEnableMotionTrail = true;
        m_iCurMotionTrailAnimIndex = m_pModelCom->Get_CurAnimIndex();

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 0.7f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(50.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

        });

    m_pModelCom->Register_Event("SprintAtk_Strong_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), false);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(0.7f, 0.5f);
        m_isEnableMotionTrail = false;
        });

    // Í∞ïÍ≥µÍ≤?
    m_pModelCom->Register_Event("StrongAtk01_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        // Distortion

        DISTORTION_DESC Desc{};

        _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
        _float fPosY = XMVectorGetY(vCenterPos);
        _float fOffset = 2.f;
        vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
        XMStoreFloat3(&Desc.vCenter, vCenterPos);
        Desc.fRange = 1.f;
        Desc.fPower = 0.01f;
        Desc.fDuration = 0.3f;
        Desc.vFadeTime = _float2(0.1f, 0.1f);
        Desc.fSpeed = 1.f;
        Desc.iNoiseIndex = 4;
        m_pGameInstance->Start_Distortion(Desc);
        });

    m_pModelCom->Register_Event("StrongAtk02_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        // Distortion

        DISTORTION_DESC Desc{};

        _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
        _float fPosY = XMVectorGetY(vCenterPos);
        _float fOffset = 2.f;
        vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
        XMStoreFloat3(&Desc.vCenter, vCenterPos);
        Desc.fRange = 1.f;
        Desc.fPower = 0.01f;
        Desc.fDuration = 0.3f;
        Desc.vFadeTime = _float2(0.1f, 0.1f);
        Desc.fSpeed = 1.f;
        Desc.iNoiseIndex = 4;
        m_pGameInstance->Start_Distortion(Desc);
        });

    m_pModelCom->Register_Event("StrongAtk03_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        RADIAL_BLUR_DESC RBDesc{};

        RBDesc.vCenterUV = _float2(0.5f, 0.5f);
        RBDesc.fSampleRadius = 0.05f;
        RBDesc.vMaskRadius = _float2(0.f, 0.3f);
        RBDesc.fExponent = 1.f;
        RBDesc.iNumSamples = 16;
        RBDesc.fAttenuation = 0.1f;
        RBDesc.fStrength = 0.7f;       // == Target Strength(0 ~ 1) -> ?ù¥ Í∞ïÎèÑÎ•? ÏµúÎ??Í∞íÏúºÎ°? ?Ç¨?ö©?ïò?ó¨ Î≥¥Í∞Ñ ?†Å?ö©?ê®
        RBDesc.fDuration = 1.f;
        RBDesc.vFadeTime = _float2(0.25f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RBDesc);

        // Vignette

        VIGNETTE_CONFIG Config{};
        Config.vColor = _float3(0.f, 0.f, 0.f);
        Config.fPower = 3.5f;
        Config.fMinIntensity = 0.f;
        Config.fMaxIntensity = 4.f;
        Config.fDuration = 1.f;
        Config.vFadeTime = _float2(0.5f, 0.5f);
        m_pGameInstance->Start_VignetteAnimation(Config);

        m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.5f, 0.2f, 0.5f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.5f, 0.2f, 0.5f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.5f, 0.2f, 0.5f);
        });

    // Í∞ïÍ≥µÍ≤? Ï∞®Ï??
    m_pModelCom->Register_Event("StrongAtk_Charge_MotionTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_RedDefault"), true);

        m_isEnableMotionTrail = true;
        m_iCurMotionTrailAnimIndex = m_pModelCom->Get_CurAnimIndex();
        });

    m_pModelCom->Register_Event("StrongAtk_Charge_MotionTrail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_RedDefault"), false);
        m_isEnableMotionTrail = false;
        });

    // Í∞ïÏäµ
    m_pModelCom->Register_Event("PureMind_SeismicKick_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        RADIAL_BLUR_DESC RBDesc{};
        RBDesc.vCenterUV = _float2(0.5f, 0.5f);
        RBDesc.fSampleRadius = 0.05f;
        RBDesc.vMaskRadius = _float2(0.f, 0.3f);
        RBDesc.fExponent = 1.f;
        RBDesc.iNumSamples = 16;
        RBDesc.fAttenuation = 0.1f;
        RBDesc.fStrength = 0.7f;
        RBDesc.fDuration = 1.f;
        RBDesc.vFadeTime = _float2(0.25f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RBDesc);

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 0.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(50.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

        });

    // Ï∞∞ÎÇò Î≤†Í∏∞
    m_pModelCom->Register_Event("SprintAtk_Fast_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), true);
        RADIAL_BLUR_DESC RBDesc{};
        RBDesc.vCenterUV = _float2(0.5f, 0.5f);
        RBDesc.fSampleRadius = 0.05f;
        RBDesc.vMaskRadius = _float2(0.f, 0.3f);
        RBDesc.fExponent = 1.f;
        RBDesc.iNumSamples = 16;
        RBDesc.fAttenuation = 0.1f;
        RBDesc.fStrength = 0.5f;       // == Target Strength(0 ~ 1) -> ?ù¥ Í∞ïÎèÑÎ•? ÏµúÎ??Í∞íÏúºÎ°? ?Ç¨?ö©?ïò?ó¨ Î≥¥Í∞Ñ ?†Å?ö©?ê®
        RBDesc.fDuration = 1.25f;
        RBDesc.vFadeTime = _float2(0.25f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RBDesc);

        m_isEnableMotionTrail = true;
        m_iCurMotionTrailAnimIndex = m_pModelCom->Get_CurAnimIndex();

        FOVModifier tMod{};
        tMod.strID = TEXT("SprintAtk");
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.25f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(50.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

        });

    m_pModelCom->Register_Event("SprintAtk_Fast_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), false);
        m_isEnableMotionTrail = false;
        m_pClientInstance->ActiveCamera_KillFov(TEXT("SprintAtk"));
        });

    // Í∑∏Î¶º?ûê Ï∞∏Í≤©
    m_pModelCom->Register_Event("Tempest_MoonVeil_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        RADIAL_BLUR_DESC RBDesc{};

        RBDesc.vCenterUV = _float2(0.5f, 0.5f);
        RBDesc.fSampleRadius = 0.05f;
        RBDesc.vMaskRadius = _float2(0.f, 0.3f);
        RBDesc.fExponent = 1.f;
        RBDesc.iNumSamples = 16;
        RBDesc.fAttenuation = 0.1f;
        RBDesc.fStrength = 0.4f;       // == Target Strength(0 ~ 1) -> ?ù¥ Í∞ïÎèÑÎ•? ÏµúÎ??Í∞íÏúºÎ°? ?Ç¨?ö©?ïò?ó¨ Î≥¥Í∞Ñ ?†Å?ö©?ê®
        RBDesc.fDuration = 1.5f;
        RBDesc.vFadeTime = _float2(0.25f, 0.5f);
        m_pGameInstance->Start_RadialBlur(RBDesc);

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    m_pModelCom->Register_Event("Tempest_MoonVeil_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {

        // Distortion
        DISTORTION_DESC Desc{};
        _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
        _float fPosY = XMVectorGetY(vCenterPos);
        _float fOffset = 2.f;
        vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
        XMStoreFloat3(&Desc.vCenter, vCenterPos);
        Desc.fRange = 0.5f;
        Desc.fPower = 0.01f;
        Desc.fDuration = 1.f;
        Desc.vFadeTime = _float2(0.5f, 0.2f);
        Desc.fSpeed = 1.f;
        Desc.iNoiseIndex = 9;
        m_pGameInstance->Start_Distortion(Desc);
        });

    // ?ã∑Ïß?
    m_pModelCom->Register_Event("Dodge_MotionTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), true);
        m_isEnableMotionTrail = true;
        m_iCurMotionTrailAnimIndex = m_pModelCom->Get_CurAnimIndex();

        FOVModifier tMod{};
        tMod.strID = TEXT("Dodge");
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 0.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(50.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });
    m_pModelCom->Register_Event("Dodge_MotionTrail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), false);
        m_isEnableMotionTrail = false;

        m_pClientInstance->ActiveCamera_KillFov(TEXT("Dodge"));
        });

    // ?ã∑Ïß? ?ñ¥?Éù
    m_pModelCom->Register_Event("DodgeAtk_MotionTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), true);
        m_isEnableMotionTrail = true;
        m_iCurMotionTrailAnimIndex = m_pModelCom->Get_CurAnimIndex();
        });
    m_pModelCom->Register_Event("DodgeAtk_MotionTrail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), false);
        m_isEnableMotionTrail = false;
        });

#pragma endregion


#pragma region Collider  
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {Event_AttackTiming(true); });
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {Event_AttackTiming(false);  });
    m_pModelCom->Register_Event("BodyAttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_iCurAnimIndex = m_pModelCom->Get_CurAnimIndex(); 
        m_isBodyAttackActive = true; 
        m_isNotifyAttacking = true;
        m_pBodyCom_BodyAttack->Collision_Active(true); });
    m_pModelCom->Register_Event("BodyAttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_isBodyAttackActive = false;
        m_isNotifyAttacking = false;
        m_pBodyCom_BodyAttack->Collision_Active(false); });
    m_pModelCom->Register_Event("SpearOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pSpear->Set_Equipped(true);
        m_pClientInstance->Set_PlayerInput(true); });
    m_pModelCom->Register_Event("SpearOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pSpear->Set_Equipped(false);
        m_pClientInstance->Set_PlayerInput(false);
        });
    m_pModelCom->Register_Event("HeaL1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pPlayerData->fCulHp += m_pPlayerData->fLachrymaItemRegen;
        if (m_pPlayerData->fCulHp > m_pPlayerData->fMaxHp)
            m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp; }); //?ùº?Å¨Î¶¨Îßà
    m_pModelCom->Register_Event("Heal2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {

        if (*m_pHealIndex == 1)
            m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;
       
        if (*m_pHealIndex == 3) 
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 2 });
        
        if (*m_pHealIndex == 4) 
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 3 });
        
        if (*m_pHealIndex == 5) 
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 4 });
        
        if (*m_pHealIndex == 6) 
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 5 });

        }); //?ûê?Öú
#pragma endregion

    // ?îÑÎ¶¨Ï¶à?Ñà
    m_pModelCom->Register_Event("Injure_VignetteAnimation", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // Vignette
        VIGNETTE_CONFIG Config{};
        Config.vColor = _float3(0.25f, 0.f, 0.f);
        Config.fPower = 3.5f;
        Config.fMinIntensity = 5.f;
        Config.fMaxIntensity = 10.f;
        Config.fDuration = 1.5f;
        Config.vFadeTime = _float2(0.75f, 0.75f);
        Config.isUseNoise = true;
        Config.iTextureIndex = 1;
        Config.fContrast = 1.f;
        m_pGameInstance->Start_VignetteAnimation(Config, false);
        });

    //m_pModelCom->Register_Event("LanternOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {*m_isEquipLantern = true; });
    //m_pModelCom->Register_Event("LanternOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { *m_isEquipLantern = false;  });

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_AnimationEvent_SFX()
{
    /* (≥Î∆º∆ƒ¿Ã ≈∞ ∞™ , ≥Î∆º∆ƒ¿Ã √÷¥Î ºˆ, ¡¯¿‘¡°, ªÁøÓµÂ ±◊∑Ï ≈∏¿‘, ∫º∑˝, æÓ∂≤ √§≥ŒªÁøÎ, »Æ∑¸¿˚¿∏∑Œ º“∏Æ ¿Áª˝«“ ∂ß ªÁøÎ«“ ∫Ò¿≤(0.f~100.f), »Æ∑¸¿˚¿∏∑Œ º“∏Æ ¿Áª˝«“ ∞«¡ˆ? ) */
    auto Register_EventGroup = [&](const string& strEventKey, _int iCount, ANIM_EVENT_TRIGGERTYPE eTrigger, SOUND_TYPE eSoundType, _float fVolume, SOUND_CHANNEL eChannelType, _float fRandomRatio = 0.f, _bool isRandom = false) {

        if (iCount <= 0) return;


        for (_int i = 1; i <= iCount; ++i)
        {
            string strTempEventKey = strEventKey;

            std::stringstream ss;
            ss << std::setw(2) << std::setfill('0') << i;
            strTempEventKey += "_" + ss.str();

            m_pModelCom->Register_Event(strTempEventKey, eTrigger, [this, strEventKey, eSoundType, fVolume, eChannelType, fRandomRatio, isRandom]() {
                if (isRandom)
                {
                    _float fRandom = m_pGameInstance->Rand(0.f, 100.f);
                    if (fRandom <= fRandomRatio)
                        return;
                }
                if (Has_Status(CKhazan_Spear::INJURED) && strEventKey == "SFX_Damaged_Hard")
                {
                    _float fRandom = m_pGameInstance->Rand(0.f, 100.f);
                    if (fRandom <= 40.f)
                        return;
                }
                if (strEventKey == "SFX_Attack_Spear_Weak1")
                    int a = 10;
                if (m_isPlaySound) m_pGameInstance->PlaySoundOnce(m_pSoundHelper->Get_NextSoundKey(eSoundType, eChannelType), fVolume, Get_SoundChannel(eChannelType)); });
        }
        };



    /* Idle*/
    Register_EventGroup("SFX_Idle", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::IDLE, 1.6f, SOUND_CHANNEL::VOICE, 40.f, true);
    Register_EventGroup("SFX_Idle_Rattle", 3, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::IDLE_RATTLE, 1.f, SOUND_CHANNEL::MOVE);

    /* Move */
    Register_EventGroup("SFX_Move_Injure_R", 9, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_INJURE_L, 2.85f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Injure_L", 9, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_INJURE_R, 2.85f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Walk", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_WALK, 2.22f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Run", 11, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_RUN, 2.22f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Sprint_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT_START, 4.f, SOUND_CHANNEL::VOICE, 70.f, true);
    Register_EventGroup("SFX_Move_Sprint", 16, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT, 1.02f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Sprint_Stop", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT_STOP, 1.02f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Sprint_Rattle", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT_RATTLE, 1.02f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Dodge_Front", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_DODGE_FRONT, 0.87f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Dodge_Rear", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_DODGE_REAR, 0.87f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Dodge_Side", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_DODGE_SIDE, 0.87f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Fall", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_FALL, 1.7f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Randing", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_RANDING, 1.7f, SOUND_CHANNEL::MOVE);

    /* Attack */
    Register_EventGroup("SFX_Attack_Spear_Weak1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_WEAK1, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Weak2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_WEAK2, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Weak3", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_WEAK3, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG1, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG2, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong2_Foley", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG2_FOLEY, 1.f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Attack_Spear_Strong3", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG3, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong3_Foley", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG3_FOLEY, 1.f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Attack_Spear_Strong_Charging_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG_CHARGING_START, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong_Charging_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG_CHARGING_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong_Charging_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG_CHARGING_IMPACT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Dodge", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_DODGE, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Weak_Sprint", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_WEAK_SPRINT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Strong_Sprint", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_STRONG_SPRINT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Brutal1_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_BRUTAL1_START, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Brutal1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_BRUTAL1, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Brutal2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_BRUTAL2, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Spear_Fall", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_SPEAR_FALL, 0.8f, SOUND_CHANNEL::WEAPON);

    /* Skill */
    Register_EventGroup("SFX_Skill_Spear_Moonlight_Slash", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_MOONLIGHT_SLASH, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Full_Moon", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_FULL_MOON, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Spiral_Thrust_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SPIRAL_THRUST_START, 1.f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Spiral_Thrust_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SPIRAL_THRUST_IMPACT, 0.8f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Skill_Spear_Spiral_Thrust_End", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SPIRAL_THRUST_END, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Spiral_Thrust_Whirlwind_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SPIRAL_THRUST_WHIRLWIND_START, 1.f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Spiral_Thrust_Whirlwind_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SPIRAL_THRUST_WHIRLWIND_IMPACT, 0.8f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Skill_Spear_Spiral_Thrust_Whirlwind_End", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SPIRAL_THRUST_WHIRLWIND_END, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Assault", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_ASSAULT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Moment_Slash", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_MOMENT_SLASH, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Critical_Strike", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_CRITICAL_STRIKE, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Shadow_Cleave_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SHADOW_CLEAVE_START, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Spear_Shadow_Cleave_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_SPEAR_SHADOW_CLEAVE_IMPACT, 0.8f, SOUND_CHANNEL::WEAPON);

    /* ATTACK & SKILL VOICE */
    Register_EventGroup("_SFX_Move_Dodge_Front", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_MIDDLE, 0.7f, SOUND_CHANNEL::VOICE);
    Register_EventGroup("_SFX_Move_Dodge_Rear", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_MIDDLE, 0.7f, SOUND_CHANNEL::VOICE);
    Register_EventGroup("_SFX_Move_Dodge_Side", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_MIDDLE, 0.7f, SOUND_CHANNEL::VOICE);
    Register_EventGroup("_SFX_Attack_Spear_Weak1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Weak2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Weak3", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Attack_Spear_Strong1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Strong2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Strong3", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Attack_Spear_Strong_Charging_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Attack_Spear_Dodge", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Weak_Sprint", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Strong_Sprint", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Attack_Spear_Brutal1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_MIDDLE, 1.f, SOUND_CHANNEL::VOICE);//m
    Register_EventGroup("_SFX_Attack_Spear_Brutal2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Attack_Spear_Fall", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Skill_Spear_Moonlight_Slash", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Skill_Spear_Full_Moon", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 1.f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Skill_Spear_Spiral_Thrust_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Skill_Spear_Spiral_Thrust_Whirlwind_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_STRONG, 0.8f, SOUND_CHANNEL::VOICE);//s
    Register_EventGroup("_SFX_Skill_Spear_Moment_Slash", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Skill_Spear_Critical_Strike", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n
    Register_EventGroup("_SFX_Skill_Spear_Shadow_Cleave_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::VOICE_ATTACK_NORMAL, 1.f, SOUND_CHANNEL::VOICE);//n


    /*  Pose  */
    Register_EventGroup("Spear_Pose_Return", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SPEAR_POSE_RETURN, 1.f, SOUND_CHANNEL::WEAPON);

    /* Damaged */
    Register_EventGroup("SFX_Damaged_Normal", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::DAMAGED_NORMAL, 3.f, SOUND_CHANNEL::VOICE);
    Register_EventGroup("SFX_Damaged_Hard", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::DAMAGED_HARD, 3.f, SOUND_CHANNEL::VOICE);

    /* Guard*/
    Register_EventGroup("SFX_Guard_On_Spear", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_ON_SPEAR, 7.f, SOUND_CHANNEL::EFFECT3);
    Register_EventGroup("SFX_Guard_Off_Spear", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_OFF_SPEAR, 7.f, SOUND_CHANNEL::EFFECT3);
    Register_EventGroup("SFX_Guard_Foley", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_FOLEY, 1.f, SOUND_CHANNEL::EFFECT1);
    //Register_EventGroup("SFX_Guard_Success", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_SUCCESS, 1.f, SOUND_CHANNEL::WEAPON);  ÏßÅÏ†ë Ï≤òÎ¶¨ ?Ö∏?ã∞?åå?ù¥x
    Register_EventGroup("SFX_Justguard_Effect", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::JUSTGUARD_EFFECT, 1.f, SOUND_CHANNEL::EFFECT2);
    Register_EventGroup("SFX_Justguard", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::JUSTGUARD, 1.f, SOUND_CHANNEL::WEAPON);

    /* Interaction */
    Register_EventGroup("SFX_Interaction_Lacrima_Get", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_LACRIMA_GET, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Heal", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_HEAL, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Lantern_On", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_LANTERN_ON, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Lantern_Off", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_LANTERN_OFF, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Spear_Armed", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_SPEAR_ARMED, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Spear_Unarmed", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_SPEAR_UNARMED, 1.f, SOUND_CHANNEL::INTERACTION);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_Collider()
{
    CBody::BODY_BOXSHAPE_DESC TipBoxDesc{};
    {
        TipBoxDesc.vExtent = _float3(2.2f, 0.8f, 0.8f);
        TipBoxDesc.eMotion = EMotionType::Kinematic;
        TipBoxDesc.eQuality = EMotionQuality::Discrete; // Í∏∞Î≥∏ Î™®Îìú
        TipBoxDesc.eShapeType = SHAPE::BOX;
        TipBoxDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK); // Ï∂îÌõÑ?óê Enum Monster attack Î≥?Í≤? ?ï†?àò?èÑ

        XMStoreFloat4x4(&m_pSpearTip1_MatrixW, XMLoadFloat4x4(m_pSpearTip1_Matrix) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_pSpearTip1_MatrixW));
        TipBoxDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        TipBoxDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        TipBoxDesc.vShapeOffset = _float3(-1.1f, 0.f, 0.f);
        m_tAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tAttackCollisionDesc.strName = TEXT("AttackCollisionDesc");
        m_tAttackCollisionDesc.pGameObject = this;
        TipBoxDesc.pCollisionDesc = &m_tAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        TipBoxDesc.pCollisionDesc->pInfo = &DamageInfo;

        TipBoxDesc.bIsTrigger = true;
        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body1"), reinterpret_cast<CComponent**>(&m_pBodyCom_SpearTip1), &TipBoxDesc)))
            return E_FAIL;

    }
    CBody::BODY_BOXSHAPE_DESC BodyBoxDesc{};
    {
        BodyBoxDesc.vExtent = _float3(0.4f, 1.1f, 0.4f);
        BodyBoxDesc.eMotion = EMotionType::Kinematic;
        BodyBoxDesc.eQuality = EMotionQuality::Discrete; // Í∏∞Î≥∏ Î™®Îìú
        BodyBoxDesc.eShapeType = SHAPE::BOX;
        BodyBoxDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK); // Ï∂îÌõÑ?óê Enum Monster attack Î≥?Í≤? ?ï†?àò?èÑ

        XMStoreFloat4x4(&m_pSpearPole_MatrixW, XMLoadFloat4x4(m_pSpearPole_Matrix) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_pSpearPole_MatrixW));
        BodyBoxDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        BodyBoxDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        BodyBoxDesc.vShapeOffset = _float3(0.f, 0.55f, 0.f);
        m_tGuardCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tGuardCollisionDesc.strName = TEXT("GuardCollisionDesc");
        m_tGuardCollisionDesc.pGameObject = this;
        BodyBoxDesc.pCollisionDesc = &m_tGuardCollisionDesc;
        BodyBoxDesc.bIsTrigger = true;
        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body2"), reinterpret_cast<CComponent**>(&m_pBodyCom_SpearPole), &BodyBoxDesc)))
            return E_FAIL;
    }

    CBody::BODY_SPHERESHAPE_DESC BodyAttackDesc{};
    {
        BodyAttackDesc.fRadius = 0.5f;
        BodyAttackDesc.bIsTrigger = true;
        BodyAttackDesc.bStartActive = true;
        BodyAttackDesc.eMotion = EMotionType::Kinematic;
        BodyAttackDesc.eQuality = EMotionQuality::Discrete;
        BodyAttackDesc.eShapeType = SHAPE::SPHERE;
        BodyAttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat3(&BodyAttackDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&BodyAttackDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        BodyAttackDesc.vShapeOffset = _float3(0.f, 0.5f, 1.f);
        m_tBodyAttackCollisionDesc.pGameObject = this;
        m_tBodyAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tBodyAttackCollisionDesc.strName = TEXT("Player_BodyAttack");
        BodyAttackDesc.pCollisionDesc = &m_tBodyAttackCollisionDesc;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body4"), reinterpret_cast<CComponent**>(&m_pBodyCom_BodyAttack), &BodyAttackDesc)))
            return E_FAIL;
    }

    CBody::BODY_SPHERESHAPE_DESC BodyDesc{};
    {
        BodyDesc.fRadius = 3.f;
        BodyDesc.bIsTrigger = true;
        BodyDesc.bStartActive = true;
        BodyDesc.eMotion = EMotionType::Kinematic;
        BodyDesc.eQuality = EMotionQuality::Discrete;
        BodyDesc.eShapeType = SHAPE::SPHERE;
        BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH);

        XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tSearchCollisionDesc.pGameObject = this;
        m_tSearchCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH);
        m_tSearchCollisionDesc.strName = TEXT("Player_Search");
        //pCollDesc.pInfo = ?? // ?ûë?Ñ±?ïòÍ∏?
        BodyDesc.pCollisionDesc = &m_tSearchCollisionDesc;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body3"), reinterpret_cast<CComponent**>(&m_pBodyCom_Search), &BodyDesc)))
            return E_FAIL;
    }

    m_pBodyCom_BodyAttack->Collision_Active(false);

    return S_OK;
}

HRESULT CBody_Khazan_Spear::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;


    return S_OK;
}

HRESULT CBody_Khazan_Spear::Ready_Equipment()
{

    const auto& equipData = m_pClientInstance->Get_PlayerEquipment();

    // Í∞? ?ä¨Î°?Î≥? Ï¥àÍ∏∞ ?û•Îπ? ?Ñ§?†ï
    struct SlotMapping {
        EQUIPMENTTYPE type;
        _uint equipIndex;
    };

    vector<SlotMapping> slots = {
        { EQUIPMENTTYPE::HEAD, equipData.iHair },
        { EQUIPMENTTYPE::TORSO, equipData.iTorso },
        { EQUIPMENTTYPE::ARM, equipData.iArm },
        { EQUIPMENTTYPE::LEG, equipData.iLeg },
        { EQUIPMENTTYPE::SHOES, equipData.iShoes },
        { EQUIPMENTTYPE::FACE, equipData.iFace },
    };

    for (const auto& slot : slots)
    {
        if (slot.equipIndex != 0)
        {
            Equip_Part(slot.type, m_pClientInstance->Get_EquipmentName(slot.equipIndex));
        }
    }
    return S_OK;
}

void CBody_Khazan_Spear::Equip_Part(EQUIPMENTTYPE eType, const _wstring& strPartName)
{
    if (eType == EQUIPMENTTYPE::SPEAR
        || eType == EQUIPMENTTYPE::GSWORD
        || eType == EQUIPMENTTYPE::NONE)
        return;

    // Í∏∞Ï°¥ ?û•Ï∞? ?ååÏ∏? ?ï¥?†ú
    auto iter = m_EquippedParts.find(eType);
    if (iter != m_EquippedParts.end())
    {
        auto partIter = m_AllParts.find(iter->second);
        if (partIter != m_AllParts.end())
        {
            m_pModelCom->Detach_Part(partIter->second);
            Safe_Release(partIter->second);
        }
        m_EquippedParts.erase(iter);
    }

    // ?Éà ?ååÏ∏? ?û•Ï∞?
    if (!strPartName.empty())
    {
        auto partIter = m_AllParts.find(strPartName);
        if (partIter != m_AllParts.end())
        {
            m_pModelCom->Attach_Part(partIter->second);
            m_EquippedParts[eType] = strPartName;
            Safe_AddRef(partIter->second);
        }
    }

    // Îπ†Î•∏ ?†å?çî?ö© 
    Update_QuickRenderCache();
}

void CBody_Khazan_Spear::Update_QuickRenderCache()
{
    for (auto pModel : m_RenderParts)
        Safe_Release(pModel);
    m_RenderParts.clear();

    /* ?†å?çî ?àú?Ñú ?†Å?ö© Í∞??ä• */
    static const vector<EQUIPMENTTYPE> renderOrder = {
        EQUIPMENTTYPE::SHOES,
        EQUIPMENTTYPE::LEG,
        EQUIPMENTTYPE::TORSO,
        EQUIPMENTTYPE::ARM,
        EQUIPMENTTYPE::HEAD,
        EQUIPMENTTYPE::FACE,
    };

    for (EQUIPMENTTYPE type : renderOrder)
    {
        auto iter = m_EquippedParts.find(type);
        if (iter != m_EquippedParts.end())
        {
            auto partIter = m_AllParts.find(iter->second);
            if (partIter != m_AllParts.end())
            {
                m_RenderParts.push_back(partIter->second);
                Safe_AddRef(partIter->second);
            }
        }
    }

    /* todo !! ?ó¨Í∏∞Ïóê Î™®ÏÖò?ä∏?†à?ùºÏª¥Ìè¨?Ñå?ä∏?óê  ?ûú?çî?ö© ?ååÏ∏†Î™®?ç∏ Î∞îÍºà?ã§Í≥? ?ÑòÍ≤®Ï£ºÍ∏?. */
    // Part Model?ù¥ ?ûà?äî Í≤ΩÏö∞!!
    m_pMotionTrailCom->Update_PartModels(m_RenderParts);



    // Part Model?ù¥ ?óÜÍ≥? Master ModelÎß? ?ûà?äî Í≤ΩÏö∞!! (Î¨¥Í∏∞)
    //  m_pMotionTrailCom->Update_MasterModel(m_pModelCom);
}

void CBody_Khazan_Spear::FX_Trail()
{
    _matrix tip = XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt);
    _matrix hand = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
    m_pTrail->Add_ControlPoint(tip.r[3], hand.r[3]);

    //if (m_iTrailType)
    //    m_TrailParticleTime += 1.f;
    //
    //if (m_TrailParticleTime > 3.f)
    //{
    //    (m_iTrailType == 1) ?
    //        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("TrailParticle"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW).r[3])
    //        : m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("TrailParticle_R"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW).r[3]);
    //}
    //else
    //    m_TrailParticleTime = 0.f;
    //

    if (m_iTrailType == 1)
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("TrailParticle"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW).r[3]);
    else if(m_iTrailType == 2)
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("TrailParticle_R"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW).r[3]);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust1(_fvector pos)
{

    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust"), pos);


    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = pos;
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 0.5f;
    Desc.fPower = 0.03f;
    Desc.fDuration = 2.f;
    Desc.vFadeTime = _float2(1.f, 0.2f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 9;
    m_pGameInstance->Start_Distortion(Desc);

    // Vignette
    VIGNETTE_CONFIG Config{};
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;
    Config.fMinIntensity = 0.f;
    Config.fMaxIntensity = 4.f;
    Config.fDuration = 2.f;
    Config.vFadeTime = _float2(1.f, 1.f);
    m_pGameInstance->Start_VignetteAnimation(Config);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust2(_fvector pos)
{

    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust2"), pos);

}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust3(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust3"), pos);


    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = pos;
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.02f;
    Desc.fDuration = 1.f;
    Desc.vFadeTime = _float2(0.4f, 0.2f);
    Desc.fSpeed = 1.5f;
    Desc.iNoiseIndex = 18;
    m_pGameInstance->Start_Distortion(Desc);

    // Vignette
    VIGNETTE_CONFIG Config{};
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;
    Config.fMinIntensity = 0.f;
    Config.fMaxIntensity = 4.f;
    Config.fDuration = 1.f;
    Config.vFadeTime = _float2(0.5f, 0.5f);
    m_pGameInstance->Start_VignetteAnimation(Config);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust4(_fvector pos)
{

    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust4"), pos);

}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust5(_fvector pos)
{

    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust5"), pos);

}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_Blust6(_fvector pos)
{

    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust6"), pos);


    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = pos;
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 0.5f;
    Desc.fPower = 0.03f;
    Desc.fDuration = 2.f;
    Desc.vFadeTime = _float2(1.f, 0.2f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 9;
    m_pGameInstance->Start_Distortion(Desc);

    // Vignette
    VIGNETTE_CONFIG Config{};
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;
    Config.fMinIntensity = 0.f;
    Config.fMaxIntensity = 2.f;
    Config.fDuration = 2.f;
    Config.vFadeTime = _float2(1.f, 1.f);
    m_pGameInstance->Start_VignetteAnimation(Config);
}

void CBody_Khazan_Spear::FX_StrongAtk_Charge_BlustSmall(_fvector pos)
{
    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("BlustSmall"), pos);
}

void CBody_Khazan_Spear::Spear_Spike()
{
    _matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt);

    _vector S, Q, T;

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {

        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);


        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));


        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            XMVectorSet(0.f, 0.f, 0.f, 1.f)
        );

        T = W.r[3];
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }


    _vector V_LocalOffset = XMVectorSet(-1.f, 0.f, 1.f, 0.f);
    _vector V_WorldOffset = XMVector3Rotate(V_LocalOffset, Q);
    _vector V_FinalPosition = T + V_WorldOffset;


    EffectID_SpearWind = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust5"), Q, V_FinalPosition);
}

void CBody_Khazan_Spear::Set_BaseTrail()
{
    TRAIL_CONFIG Config{};
    Config.fLifeTime = 0.35f;
    Config.iTextureIdx = 9;
    Config.iDivisionCount = 10;
    Config.vColor = _float4(1.f, 1.f, 1.f, 1.f);
    m_pTrail->Set_TrailConfig(Config);
    m_iTrailType = 0;
}

void CBody_Khazan_Spear::Set_BlueTrail()
{
    TRAIL_CONFIG Config{};
    Config.fLifeTime = 0.35f;
    Config.iTextureIdx = 8;
    Config.iDivisionCount = 10;
    Config.vColor = _float4(2.569f, 2.569f, 3.529f, 1.f);
    m_pTrail->Set_TrailConfig(Config);
    m_iTrailType = 1;
}

void CBody_Khazan_Spear::Set_RedTrail()
{
    TRAIL_CONFIG Config{};
    Config.fLifeTime = 0.35f;
    Config.iTextureIdx = 8;
    Config.iDivisionCount = 10;
    Config.vColor = _float4(3.529f, 2.569f, 2.569f, 1.f);
    m_pTrail->Set_TrailConfig(Config);
    m_iTrailType = 2;
}

void CBody_Khazan_Spear::Spawn_Guard_FX()
{ 
    _matrix mat = XMLoadFloat4x4(&m_pSpearPole_MatrixW);
    if (m_bGuradFX[0])
    {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("PerfectGaurd"), mat.r[3]);
        m_bGuradFX[0] = false;
    }
    if (m_bGuradFX[1])
    {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Guard"), mat.r[3]);
        m_bGuradFX[1] = false;
    }
}

_vector CBody_Khazan_Spear::BodyCenter()
{
    _vector pos = m_pParentTransform->Get_WorldMatrix().r[3];
    pos = XMVectorSetY(pos, XMVectorGetY(pos) + 1.25f);
    return pos;
}

void CBody_Khazan_Spear::BrutalAtk_ScreenEffect0()
{
    // ?†à?îî?ñº Î∏îÎü¨ ÏßßÍ≤å
    RADIAL_BLUR_DESC RBDesc{};
    RBDesc.vCenterUV = _float2(0.5f, 0.5f);
    RBDesc.fSampleRadius = 0.05f;
    RBDesc.vMaskRadius = _float2(0.f, 0.3f);
    RBDesc.fExponent = 1.f;
    RBDesc.iNumSamples = 16;
    RBDesc.fAttenuation = 0.1f;
    RBDesc.fStrength = 0.7f;       // == Target Strength(0 ~ 1) -> ?ù¥ Í∞ïÎèÑÎ•? ÏµúÎ??Í∞íÏúºÎ°? ?Ç¨?ö©?ïò?ó¨ Î≥¥Í∞Ñ ?†Å?ö©?ê®
    RBDesc.fDuration = 2.f;
    RBDesc.vFadeTime = _float2(1.5f, 0.5f);
    m_pGameInstance->Start_RadialBlur(RBDesc);

    // Fov Ï¢ÅÍ≤å
    FOVModifier tMod{};
    tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
    tMod.fDuration = 1.f;
    tMod.fFrom = XMConvertToRadians(60.f);
    tMod.fTo = XMConvertToRadians(50.f);
    tMod.iPriority = 1.f;
    tMod.Ease = EaseOutQuad;
    m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
}

void CBody_Khazan_Spear::BrutalAtk_ScreenEffect1()
{
    RADIAL_BLUR_DESC RBDesc{};
    RBDesc.vCenterUV = _float2(0.5f, 0.5f);
    RBDesc.fSampleRadius = 0.05f;
    RBDesc.vMaskRadius = _float2(0.f, 0.3f);
    RBDesc.fExponent = 1.f;
    RBDesc.iNumSamples = 16;
    RBDesc.fAttenuation = 0.1f;
    RBDesc.fStrength = 1.f;
    RBDesc.fDuration = 0.75f;
    RBDesc.vFadeTime = _float2(0.35f, 0.35f);
    m_pGameInstance->Start_RadialBlur(RBDesc);

    FOVModifier tMod{};
    tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
    tMod.fDuration = 0.375f;
    tMod.fFrom = XMConvertToRadians(60.f);
    tMod.fTo = XMConvertToRadians(70.f);
    tMod.iPriority = 1.f;
    tMod.Ease = EaseOutQuad;
    m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

    VIGNETTE_CONFIG Config{};
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;
    Config.fMinIntensity = 0.f;
    Config.fMaxIntensity = 4.f;
    Config.fDuration = 0.75f;
    Config.vFadeTime = _float2(0.35f, 0.35f);
    m_pGameInstance->Start_VignetteAnimation(Config);
}

_vector CBody_Khazan_Spear::Decompose_Rotation(_matrix W, _vector localRot, _vector offset)
{
    //_matrix W = XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt);

    _vector S, Q, T;

    _matrix Local_Rotation = XMMatrixRotationQuaternion(localRot);
    W = XMMatrixMultiply(Local_Rotation, W);

    if (!XMMatrixDecompose(&S, &Q, &T, W))
    {
        XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);
        _vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
        _vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
        _vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));

        _matrix RotationMatrix(
            r0,
            r1,
            r2,
            offset
        );
        Q = XMQuaternionRotationMatrix(RotationMatrix);
    }

    return Q;
}

void CBody_Khazan_Spear::UpdateSpearWind(_bool isEnableRadialBlur)
{ 
    m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearWind"), EffectID_SpearWind, XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);

    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.01f;
    Desc.fDuration = 0.5f;
    Desc.vFadeTime = _float2(0.3f, 0.1f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 4;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_Spear::UpdateSpearRedWind(_bool isEnableRadialBlur)
{
    m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Spear_BloodWind"), EffectID_SpearWind, XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);

    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.01f;
    Desc.fDuration = 0.5f;
    Desc.vFadeTime = _float2(0.3f, 0.1f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 4;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_Spear::SpawnSpearWind()
{ 
    EffectID_SpearWind = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpearWind"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);

    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.01f;
    Desc.fDuration = 0.5f;
    Desc.vFadeTime = _float2(0.3f, 0.1f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 4;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_Spear::SpawnSpearRedWind()
{
    EffectID_SpearWind = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Spear_BloodWind"), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt), XMLoadFloat4x4(&m_pSpearTip1_MatrixW_nJolt).r[3]);

    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.01f;
    Desc.fDuration = 0.5f;
    Desc.vFadeTime = _float2(0.3f, 0.1f);
    Desc.fSpeed = 1.f;
    Desc.iNoiseIndex = 4;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_Spear::Event_AttackTiming(_bool isAttackStart)
{
    /* Í≥µÍ≤© ÎßûÏïÑ?Ñú Ï§ëÍ∞Ñ?óê ?ÅäÍ∏? Í≤ΩÏö∞?äî? */
    if (isAttackStart)
    {
        m_isNotifyAttacking = true;
        m_isSpearTipActive = true;
        m_isSpearFullExtension = false;
        m_iCurAnimIndex = m_pModelCom->Get_CurAnimIndex();
        m_pBodyCom_SpearTip1->Collision_Active(true);
       
    }
    else
    {
        m_isNotifyAttacking = false;
        m_isSpearTipActive = false;
        m_isSpearFullExtension = true;
        m_pBodyCom_SpearTip1->Collision_Active(false);
    }

}

CBody_Khazan_Spear* CBody_Khazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Khazan_Spear* pInstance = new CBody_Khazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Khazan_Spear::Clone(void* pArg)
{
    CBody_Khazan_Spear* pInstance = new CBody_Khazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Khazan_Spear::Free()
{
    __super::Free();

    Safe_Release(m_pMotionTrailCom);
    
    Safe_Release(m_pBodyCom_Search);
    Safe_Release(m_pBodyCom_SpearPole);
    Safe_Release(m_pBodyCom_SpearTip1);
    Safe_Release(m_pBodyCom_BodyAttack);

    if (m_pBrutalAttack)
        Safe_Release(m_pBrutalAttack);

    //?òÑ?û¨ Safe_AddRef ?ïòÏß? ?ïäÍ≥? ????Ñú ?Ñê Î∞ñÏïÑ?ë†
    m_pBrutalmonster = nullptr;
    //if (m_pBrutalmonster)
    //    Safe_Release(m_pBrutalmonster);

    Safe_Release(m_pSpear);
    Safe_Release(m_pClientInstance);
    
    if (!m_isPrototype)
    {
        for (auto pModel : m_RenderParts)
            m_pModelCom->Detach_Part(pModel);
    }
    
    Safe_Release(m_pParentTransform);
    Safe_Release(m_pShaderCom);

    for (auto pModel : m_RenderParts)
        Safe_Release(pModel);
    m_RenderParts.clear();

    for (auto EquipPart : m_AllParts)
    {
        for (auto Part : m_EquippedParts)
        {
            if (Part.second == EquipPart.first)
            {
                Safe_Release(EquipPart.second);
                break;
            }
        }
    }

    for (auto partIter : m_AllParts)
        Safe_Release(partIter.second);
    m_AllParts.clear();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pTrail);
    Safe_Release(m_pSoundHelper);

    m_CollMonsters.clear();

}