#include "Body_Khazan_GS.h"
#include "Khazan_GSword.h"
#include "GSword_Khazan_GS.h"

#include "GameInstance.h"
#include "ClientInstance.h"

#include "Damage_Text.h"
#include "MeshTrail.h"
#include "Target_BrutalAttack.h"
#include "SoftBody.h"
#include "WeaponObject.h"
#include "Monster.h"
#include "Khazan_SoundHelper.h"


using  SOUND_TYPE = CKhazan_SoundHelper::PC_SOUND_GROUPTYPE;
using  SOUND_CHANNEL = CKhazan_SoundHelper::PC_SOUND_CHANNEL;

CBody_Khazan_GS::CBody_Khazan_GS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CBody_Khazan_GS::CBody_Khazan_GS(const CBody_Khazan_GS& Prototype)
    : CPartObject{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);

}
HRESULT CBody_Khazan_GS::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBody_Khazan_GS::Initialize_Clone(void* pArg)
{
    BODY_KHAZAN_GS_DESC* pDesc = static_cast<BODY_KHAZAN_GS_DESC*>(pArg);
    m_pParentState = pDesc->pState;
    m_pParentStatus = pDesc->pStatus;
    m_pHitReaction = pDesc->pHitReation;
    m_iCurState = *m_pParentState;
    m_pGuardRotationTarget = pDesc->pGuardRotationTarget;
    m_pParentTransform = pDesc->pParentTransform;
    Safe_AddRef(m_pParentTransform);
    m_pParentIsCanStaminaRecovery = pDesc->pParentIsCanStaminaRecovery;
    m_pHealIndex = pDesc->pHealIndex;
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    /* 부모 트랜스폼 연결 */
    m_pModelCom->Set_OwnerTransform(&m_pParentTransform);

    if (FAILED(Ready_Equipment()))
        return E_FAIL;

    if (FAILED(Ready_AnimationEvents()))
        return E_FAIL;

    /* 사운드 헬퍼  */
    m_pSoundHelper = CKhazan_SoundHelper::Create();

    if (FAILED(Ready_AnimationEvent_SFX()))
        return E_FAIL;

    /* 뼈 행렬 가지고오기 */
    m_pMatGSwordBody = m_pModelCom->Get_BoneMatrix("FX_R_GSword_02");
    m_pMatGSwordTip = m_pModelCom->Get_BoneMatrix("FX_Weapon_R_Gsword_End");


    if (FAILED(Ready_Colliders()))
        return E_FAIL;

    /* 플레이어 데이터(스탯) 초기화 */
    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();

    /* 파츠 자동 업데이트  */
    m_pClientInstance->Set_ChangePlayerArmorEquipmentCallBack([this](EQUIPMENTTYPE type, const _wstring& strPartName) {Equip_Part(type, strPartName); });

    m_isCollision = false;

    m_pModelCom->WarmupAnimations();
    //m_AllParts[TEXT("Prisoner_Leg3")]->WarmupAnimations();

    m_pParentTransform->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 0.f));

    m_Offset_Matrix = XMMatrixRotationX(XMConvertToRadians(-90));

    m_bGuradFX[0] = false;
    m_bGuradFX[1] = false;
    m_iTrailType = 0;

    return S_OK;
}

void CBody_Khazan_GS::Priority_Update(_float fTimeDelta)
{
    m_pTrail->Priority_Update(fTimeDelta);
}

void CBody_Khazan_GS::Update(_float fTimeDelta)
{
    /* 모션트레일중 다른 애니메이션이 나올 시 끄기  */
    if (m_isEnableAnimEvent && m_iCurAnimEventIndex != m_pModelCom->Get_CurAnimIndex())
    {
        m_isEnableAnimEvent = false;
        Trigger_MotionTrail(TEXT(""), false);
        //Remove_Status(CKhazan_GSword::DODGE_ENDING);
    }

    /* 이펙트 안꺼지는거 끄기  */
    if (m_isEableGiantHuntEvent && m_iCurAnimEventIndex != m_pModelCom->Get_CurAnimIndex())
    {
        m_isEableGiantHuntEvent = false;
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
    }

    /* 애니메이션 */
    _float fTimeDeltaAdjsut = fTimeDelta;
    if (m_isNotifyAttacking)  fTimeDeltaAdjsut *= 1.2f;    //  공격 애니메이션 속도 높이기
    if (Has_Status(CKhazan_GSword::LADDER_SPRINT))  fTimeDeltaAdjsut *= 2.f;   //  사다리 스프린트 애니메이션 속도 높이기 
    m_isFinishedAnimation = m_pModelCom->Play_Animation(fTimeDeltaAdjsut);

    /* 장착중인 파츠들 플래그 초기화 (최적화용) */
    for (auto pPart : m_RenderParts)
        if (pPart)  pPart->Reset_PartLocalBonesFlag();

    Update_CombinedMatrix();

    Update_Colliders(fTimeDelta);

    /* 죽음 처리 */
    if (Update_Dead(fTimeDelta)) return;

    m_pTrail->Update(fTimeDelta);

    Check_Guarding(fTimeDelta);

    if (m_isCollision)
    {
        m_isCollision = false;
        m_pGameInstance->Spawn_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("BloodHit"), XMLoadFloat4(&m_fCollisionPos));
    }

    m_pMotionTrailCom->Update(fTimeDelta);
    if(m_isActiveMotionTrail)
       m_pMotionTrailCom->Start_MotionTrail(fTimeDelta);


    //bool a =  m_pClientInstance->Is_CurrentSpear();
    //bool b = m_pClientInstance->Is_CurrentGSword();

    //if (m_pGameInstance->Key_Pressing(DIK_LSHIFT,fTimeDelta) && m_pGameInstance->Key_Down(DIK_M)){
    //    m_pClientInstance->Lock_Skill((1 << 4) );
    //    m_pClientInstance->Lock_Skill((1 << 8));
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Wind"), m_pParentTransform->Get_State(STATE::POSITION));
    //}

    //if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_N)){
    //    m_pClientInstance->Lock_Skill((1 << 4));
    //    m_pClientInstance->Lock_Skill((1 << 8));
    //    m_pClientInstance->Unlock_Skill(1 << 4);
    //}

    //if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_B)){
    //    m_pClientInstance->Lock_Skill((1 << 4));
    //    m_pClientInstance->Lock_Skill((1 << 8));
    //    m_pClientInstance->Unlock_Skill(1 << 8);
    //}

    // 2Phase Light Setting Test
    _float4 vPos = _float4(m_CombinedWorldMatrix._41, m_CombinedWorldMatrix._42 + 5.f, m_CombinedWorldMatrix._43, m_CombinedWorldMatrix._44);

    m_pGameInstance->Set_LightPosition(TEXT("Player_PointLight_Orange"), ENUM_CLASS(LEVEL::VIPER), vPos);
    m_pGameInstance->Set_LightPosition(TEXT("Player_PointLight_White"), ENUM_CLASS(LEVEL::VIPER), vPos);

    _vector vPosition = XMLoadFloat4(&vPos);
    _vector vLook = XMVectorSet(m_CombinedWorldMatrix._31, m_CombinedWorldMatrix._32, m_CombinedWorldMatrix._33, 0.f);

    _vector vForwardPos = vPosition + vLook * 2.f;
    _float4 vResultForwardPos{};
    XMStoreFloat4(&vResultForwardPos, vForwardPos);
    m_pGameInstance->Set_LightPosition(TEXT("Player_PointLight_Gray"), ENUM_CLASS(LEVEL::VIPER), vResultForwardPos);

    vPosition += vLook * 10.f;
    _float4 vResultPos{};
    XMStoreFloat4(&vResultPos, vPosition);

    m_pGameInstance->Set_LightPosition(TEXT("Viper_Thunder"), ENUM_CLASS(LEVEL::VIPER), vResultPos);

    _vector vGSwordPos = XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3];
    _float4 vResultGSwordPos{};
    XMStoreFloat4(&vResultGSwordPos, vGSwordPos);
    m_pGameInstance->Set_LightPosition(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), vResultGSwordPos);

    if (m_bGuradFX[0] || m_bGuradFX[1])
        Spawn_Guard_FX();
    m_bGuradFX[0] = false;
    m_bGuradFX[1] = false;

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

    m_isAttackActive = m_pBodyCom_Attack->Get_Collision_Active()
        || m_pBodyCom_RangeAttack->Get_Collision_Active()
        || m_pBodyCom_BodyAttack->Get_Collision_Active();
}

void CBody_Khazan_GS::Late_Update(_float fTimeDelta)
{    
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
        return;

    m_pTrail->Late_Update(fTimeDelta);

    //test
    //if (m_pGameInstance->Key_Down(DIK_L) && m_pGameInstance->Key_Down(DIK_K))
    //    m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip).r[3]);
}

HRESULT CBody_Khazan_GS::Render()
{

    //if (*m_isLadderRotationEvent == true )
      //  m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (Has_State(1u) && FAILED(Bind_Dissolve()))
        return E_FAIL;

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

    _uint    iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
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

HRESULT CBody_Khazan_GS::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightViewMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", m_pGameInstance->Get_ShadowLightMatrix(D3DTS::PROJ))))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;
    }

    for (auto pModel : m_RenderParts)
        Render_Part_Shadow(pModel);

    return S_OK;
}

HRESULT CBody_Khazan_GS::Render_Outline()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutlineColor", &m_OutlineConfig.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutlineSize", &m_OutlineConfig.fSize, sizeof(_float))))
        return E_FAIL;

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
    }

    for (auto pModel : m_RenderParts)
        Render_Part_Outline(pModel);

    return S_OK;
}

HRESULT CBody_Khazan_GS::Render_MotionVector()
{
    // 이전 프레임 월드, 뷰, 투영 바인드 해줘야 함
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

void CBody_Khazan_GS::Render_Part(CModel* pModel)
{
    if (nullptr == pModel)
        return;
    
    
    _bool isExclusivePartBones = m_pClientInstance->Get_PlayerEquipment().iLeg != 5009;
    pModel->Update_PartLocalBones_Once(isExclusivePartBones);
    //pModel->Update_PartLocalBones();       

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        pModel->Bind_Materials(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_SpecularTexture", i, aiTextureType_SPECULAR, 0);
        pModel->Bind_Materials(m_pShaderCom, "g_MetalnessTexture", i, aiTextureType_METALNESS, 0);


        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;
        if(Has_State(1u))
            m_pShaderCom->Begin(17);
        else
            m_pShaderCom->Begin(28);
        pModel->Render(i);
    }
}

void CBody_Khazan_GS::Render_Part_Shadow(CModel* pModel)
{
    if (Has_State(1u))
        return; 

    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(2);
        pModel->Render(i);
    }
}

void CBody_Khazan_GS::Render_Part_Outline(CModel* pModel)
{
    if (Has_State(1u))
        return;

    if (nullptr == pModel)
        return;

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        m_pShaderCom->Begin(3);
        pModel->Render(i);
    }
}

void CBody_Khazan_GS::Render_Part_MotionVector(CModel* pModel)
{
    if (Has_State(1u))
        return;

    if (nullptr == pModel)
        return;

    _bool isExclusivePartBones = m_pClientInstance->Get_PlayerEquipment().iLeg != 5009;
    pModel->Update_PartLocalBones_Once(isExclusivePartBones);
    //pModel->Update_PartLocalBones();

    _uint iNumMeshes = pModel->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        // 마스터의 본을 자동으로 사용
        if (FAILED(pModel->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", i)))
            continue;

        if (FAILED(pModel->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            continue;

        // 셰이더 바꿔야 함
        //  m_pShaderCom->Begin(11);
        pModel->Render(i);

    }
}

void CBody_Khazan_GS::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
    {
        /* 방어 콜라이더  */
        if (m_isCollGuard_Active && m_pPlayerData->fCulStamina > 0.f)
        {
            _matrix mat = XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt);
            *m_pParentStatus |= CKhazan_GSword::GUARD;

            /* 저스트 가드 타이밍 */
            if (!m_isJustGuardOnce && m_fJustGuardTime.x <= m_fJustGuardTime.y) {
                *m_pParentStatus |= CKhazan_GSword::JUST_GUARD;
                m_isJustGuardOnce = true;

                /* 몬스터한테 저스트 가드 타이밍 건내주기  */
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

                // 핑크 보라 조명
                LIGHT_TRANSITION_DESC LightDesc{};
                LightDesc.fDuration = 0.5f;
                LightDesc.vFadeTime = _float2(0.2f, 0.2f);
                LightDesc.vDiffuse = _float4(0.5f, 0.f, 1.f, 1.f);
                LightDesc.vAmbient = _float4(0.5f, 0.f, 1.f, 1.f);
                LightDesc.vSpecular = LightDesc.vDiffuse;
                LightDesc.isReturnToStart = true;
                //LightDesc.Callback = [&]() { m_pGameInstance->Set_LightEnable(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false); };
                m_pGameInstance->Start_LightTransition(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);
            
                // FOV 줌인아웃
                FOVModifier tMod{};
                tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
                tMod.fDuration = 0.25f;
                tMod.fFrom = XMConvertToRadians(60.f);
                tMod.fTo = XMConvertToRadians(50.f);
                tMod.iPriority = 1.f;
                tMod.Ease = EaseOutQuad;
                m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

                //히트스탑
                m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.2f, 0.003f, 0.6f);
                m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.2f, 0.003f, 0.6f);
                m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.2f, 0.003f, 0.6f);
            }

            m_bGuradFX[1] = true;

            // 그냥 노란 조명
            LIGHT_TRANSITION_DESC LightDesc{};
            LightDesc.fDuration = 0.5f;
            LightDesc.vFadeTime = _float2(0.2f, 0.2f);
            LightDesc.vDiffuse = _float4(1.f, 1.f, 0.8f, 1.f);
            LightDesc.vAmbient = _float4(1.f, 1.f, 0.8f, 1.f);
            LightDesc.vSpecular = LightDesc.vDiffuse;
            LightDesc.isReturnToStart = true;
            //LightDesc.Callback = [&]() { m_pGameInstance->Set_LightEnable(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), false); };
            m_pGameInstance->Start_LightTransition(TEXT("Player_GuardLight"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), LightDesc);

            /* 가드후 충돌되면 충돌된 지점 봐라보게*/
            Start_GuardRotation(vContactPoint);

            /* 스태미나 감소 */
            //if (!Has_Status(CKhazan_GSword::STAMINA_EXHAUSTION)) {
                m_pPlayerData->fCulStamina -= m_pPlayerData->fUsedStamina * 0.3f;
                *m_pParentIsCanStaminaRecovery = false;
            //}
        }
    }


    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER))
    {
        CCreature* pMonster = static_cast<CCreature*>(pDesc->pGameObject);
        if (pMonster == nullptr || pMonster->Get_CurrentHP() < 0.f)
            return;

        /*  탐지 */
        if (pMyDesc->strName == TEXT("Player_Search"))
        {
            CGameObject* pObj = pDesc->pGameObject;
            if (!pObj || pObj->Get_IsDead()) return;
            lock_guard<mutex> lock(m_CollMonsterMutex);
            if (pObj && (find(m_CollMonsters.begin(), m_CollMonsters.end(), pObj) == m_CollMonsters.end())) {
                m_CollMonsters.push_back(pObj);
            }
            return;
        }

       /* 공격 콜라이더 */
        _bool   isAttack = false;
        if (pMyDesc->strName == TEXT("Player_Attack1"))
        {
            isAttack = true;

            _uint iBrutalAtkAnimIndex[2] = { m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_Grapple_Atk_01"),m_pModelCom->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_Grapple_Atk_02") };
            _uint iCurAnimIndex = m_pModelCom->Get_CurAnimIndex();

            /* 브루탈 어택은 넉백 막기 */
            if (iCurAnimIndex != iBrutalAtkAnimIndex[0] && iCurAnimIndex != iBrutalAtkAnimIndex[1])
                pMonster->KnockBack(
                    XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                        - m_pParentTransform->Get_State(STATE::POSITION))
                    , 15.f, 50.f);

            pMonster->Consume_Stamina(20.f);
            m_pPlayerData->fCulDoggedness = m_pPlayerData->fCulDoggedness+ 0.2f > m_pPlayerData->iMaxDoggednessCount ?
                m_pPlayerData->iMaxDoggednessCount : m_pPlayerData->fCulDoggedness + 0.2f;

        }

        if (pMyDesc->strName == TEXT("Player_RangeAttack"))
        {
            isAttack = true;
            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 10.f, 45.f);
            pMonster->Consume_Stamina(15.f);
            //m_pPlayerData->fCulDoggedness += 0.2f;
        }

        if (pMyDesc->strName == TEXT("Player_BodyAttack"))
        {
            isAttack = true;
            pMonster->KnockBack(
                XMVector4Normalize(static_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION)
                    - m_pParentTransform->Get_State(STATE::POSITION))
                , 18.f, 50.f);
            pMonster->Consume_Stamina(10.f);
            m_pPlayerData->fCulDoggedness = m_pPlayerData->fCulDoggedness + 0.2f > m_pPlayerData->iMaxDoggednessCount ?
                m_pPlayerData->iMaxDoggednessCount : m_pPlayerData->fCulDoggedness + 0.2f;

        }

        if (isAttack)
        {
            pMonster->Take_Damage(m_pPlayerData->fBonusDamage, static_cast<HITREACTION>(*m_pHitReaction), this);
            m_isCollision = true;
            CTransform* MonsterTransform = dynamic_cast<CTransform*>(pDesc->pGameObject->Get_Component(TEXT("Com_Transform")));
            XMStoreFloat4(&m_fCollisionPos, MonsterTransform->Get_State(STATE::POSITION));
        }
    }

 

}

void CBody_Khazan_GS::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CBody_Khazan_GS::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    
   if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTER) && pMyDesc->strName == TEXT("Player_Search")) {
       CGameObject* pObj = pDesc->pGameObject;

       if (!pObj) return;

       lock_guard<mutex> lock(m_CollMonsterMutex);

       auto it = remove(m_CollMonsters.begin(), m_CollMonsters.end(), pObj);
       if (it != m_CollMonsters.end()) m_CollMonsters.erase(it, m_CollMonsters.end());
   }
}

_float4x4* CBody_Khazan_GS::Get_BoneMatrix(const _char* pBoneName)
{
    return m_pModelCom->Get_BoneMatrix(pBoneName);

}

void CBody_Khazan_GS::Set_GSword(CGSword_Khazan_GS* pGS)
{
    m_pGSword = pGS; Safe_AddRef(m_pGSword);
}

void CBody_Khazan_GS::Search_BrutalTarget(_float fTimeDelta)
{
    /* 브루탈 개체는 하나만 */
    if (Has_Status(CKhazan_GSword::BRUTAL_BEGIN))
        return;

   // m_fOptimizationSearchTime.x += fTimeDelta;

    //if (m_fOptimizationSearchTime.x < m_fOptimizationSearchTime.y)
       // return;

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


        /* 일정 범위에 다가가면  */
        if (fDistSq < 15.f * 15.f)
        {
            /* 후방 */
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

                    Add_Status(CKhazan_GSword::BRUTAL_BEGIN);

                    return;
                }
            }

            /*  몬스터 그로기 상태 */
            if (pCreatureMoster->Get_IsGroggy())
            {
                m_pBrutalmonster = monster;
                m_isBackBrutal = false;
                m_isGroggyBrutal = true;

                Add_Status(CKhazan_GSword::BRUTAL_BEGIN);

                return;
            }
        }

    }
}

_bool CBody_Khazan_GS::Check_BrutalAttack(_float fTimeDelta)
{
    /* 컨테이너 체크  */
    lock_guard<mutex> lock(m_CollMonsterMutex);
    for (auto it = m_CollMonsters.begin(); it != m_CollMonsters.end(); )
    {
        CMonster* pCreatureMoster = static_cast<CMonster*>(m_pBrutalmonster);
        if (*it == m_pBrutalmonster && (m_pBrutalmonster->Get_IsDead() || !m_pBrutalmonster->Get_IsActive() || pCreatureMoster->Get_CurrentHP() <= 0.f))
            it = m_CollMonsters.erase(it);
        else
            ++it;
    }

    /* 범위 내에 브루탈 가능 개체가 없으면  */
    if (!Has_Status(CKhazan_GSword::BRUTAL_BEGIN)) {
        return false;
    }

    /* 브루탈 어택 성공 후 아이콘 지우기 */
    if (Has_Status(CKhazan_GSword::BRUTAL_SUCCESS))
    {
        Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
        if (m_isBackBrutal) {
            m_pBrutalAttack->Off_BrutalAttack();
            m_isBackBrutal = false;
            m_fOptimizationSearchTime.y = 2.f;
            m_isBrutalSuccess = true;
        }

        return false;
    }

    /* 몬스터가 죽으면  */
    if (!m_pBrutalmonster || m_pBrutalmonster->Get_IsDead()) {
        Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
        if (m_isBackBrutal) {
            m_pBrutalAttack->Off_BrutalAttack();
            m_isBackBrutal = false;
            m_fOptimizationSearchTime.y = 2.f;
            m_isBrutalSuccess = true;

        }
        return false;
    }

    /*  몬스터가 슬립이 풀리면 */
    if (static_cast<CMonster*>(m_pBrutalmonster)->Get_isSleep() == true && m_isBackBrutal)
    {
        Remove_Status(CKhazan_GSword::BRUTAL_BEGIN | CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_SUCCESS);
        if (m_isBackBrutal) {
            m_pBrutalAttack->Off_BrutalAttack();
            m_isBackBrutal = false;
            m_fOptimizationSearchTime.y = 2.f;
            m_isBrutalSuccess = true;
        }
        return false;
    }

    /* 브루탈 가능 범위인지 아닌지 체크 */
    _float  fDistSq = XMVectorGetX(XMVector3LengthSq(XMVectorSet(m_pParentMatrix->_41, m_pParentMatrix->_42, m_pParentMatrix->_43, 1.f) - m_pBrutalmonster->Get_Position()));
    if (fDistSq < 15.f * 15.f) {
        if (!Has_Status(CKhazan_GSword::BRUTAL_READY)) {
            Add_Status(CKhazan_GSword::BRUTAL_READY);
            return true;
        }
    }
    else if (fDistSq > 15.f * 15.f + 1.f)
        if (Has_Status(CKhazan_GSword::BRUTAL_READY))
        {
            if (m_isBackBrutal)
                m_pBrutalAttack->Off_BrutalAttack();

            m_pBrutalAttack = nullptr;
            m_pBrutalmonster = nullptr;
            m_isBackBrutal = false;
            m_isGroggyBrutal = false;
            Remove_Status(CKhazan_GSword::BRUTAL_READY | CKhazan_GSword::BRUTAL_BEGIN);

        }


    return false;
}

void CBody_Khazan_GS::AllAttackCollisionActive_Off()
{
    m_pBodyCom_Attack->Collision_Active(false);
    m_pBodyCom_RangeAttack->Collision_Active(false);
    m_pBodyCom_BodyAttack->Collision_Active(false);
    m_isNotifyAttacking = false;
}

void CBody_Khazan_GS::Event_AttackTiming(GS_COLLISION eColl, _bool isAttackStart)
{
   // m_iCurSetAnimIndex = m_pModelCom->Get_CurAnimIndex();

    if ((eColl & COLL_ATTACK) && isAttackStart)
        m_pBodyCom_Attack->Collision_Active(true);
    else  if ((eColl & COLL_ATTACK) && !isAttackStart)
        m_pBodyCom_Attack->Collision_Active(false);


    if ((eColl & COLL_RANGEATTACK) && isAttackStart)
        m_pBodyCom_RangeAttack->Collision_Active(true);
    else  if ((eColl & COLL_RANGEATTACK) && !isAttackStart)
        m_pBodyCom_RangeAttack->Collision_Active(false);


    if ((eColl & COLL_BODYATTACK) && isAttackStart)
        m_pBodyCom_BodyAttack->Collision_Active(true);
    else  if ((eColl & COLL_BODYATTACK) && !isAttackStart)
        m_pBodyCom_BodyAttack->Collision_Active(false);


    if ((eColl & COLL_GUARD) && isAttackStart)
        m_pBodyCom_Guard->Collision_Active(true);
    else  if ((eColl & COLL_GUARD) && !isAttackStart)
        m_pBodyCom_Guard->Collision_Active(false);

}

const MOTIONTRAIL_CONFIG& CBody_Khazan_GS::Get_MotionTrailConfig()
{
    return m_pMotionTrailCom->Get_Config();
}

void CBody_Khazan_GS::Set_MotionTrailConfig(const MOTIONTRAIL_CONFIG& Config)
{
    m_pMotionTrailCom->Set_Config(Config);
}

void CBody_Khazan_GS::Set_EnableMotionTrail(_bool isEnable)
{
    m_pMotionTrailCom->Set_Enable(isEnable);
}

_bool CBody_Khazan_GS::isEnableMotionTrail()
{
    return m_pMotionTrailCom->isEnable();
}

void CBody_Khazan_GS::Start_MotionTrail(_float fDuration)
{
    m_pMotionTrailCom->Start_MotionTrail(fDuration);
}

void CBody_Khazan_GS::Start_HealRimLight(_float fDuration, const _float2& vFadeTime, _float fMaxIntensity)
{
    if (true == m_isFinishedHealRimLight)
        return;

    m_isEnableHealRimLight = true;
    m_HealRimLightDesc.fDuration = fDuration;
    m_HealRimLightDesc.vFadeTime = vFadeTime;
    m_HealRimLightDesc.vFadeTime.y = m_HealRimLightDesc.fDuration - m_HealRimLightDesc.vFadeTime.y;
    m_HealRimLightDesc.fTargetIntensity = fMaxIntensity;
}

const TRAIL_CONFIG& CBody_Khazan_GS::Get_TrailConfig() const
{
    return m_pTrail->Get_TrailConfig();
}

void CBody_Khazan_GS::Set_TrailConfig(const TRAIL_CONFIG& Config)
{
    m_pTrail->Set_TrailConfig(Config);
}

_uint CBody_Khazan_GS::Get_NumTrailTextures()
{
    return m_pTrail->Get_NumTrailTextures();
}

ID3D11ShaderResourceView* CBody_Khazan_GS::Get_TrailTexture(_uint iIndex)
{
    return m_pTrail->Get_TrailTexture(iIndex);
}

_bool CBody_Khazan_GS::Update_Dead(_float fTimeDelta)
{
    if (!Has_State(1u))
        return false;

    /* 디졸브 끝나야 true */
    if (m_fDissolveDecreaseAlphaTime.x >= m_fDissolveDecreaseAlphaTime.y)
        return true;

    m_fDissolveDecreaseAlphaTime.x += fTimeDelta;
    m_fDissolveDecreaseAlphaValue = m_fDissolveDecreaseAlphaTime.x / m_fDissolveDecreaseAlphaTime.y;
       
    return false;
}

void CBody_Khazan_GS::Update_Colliders(_float fTimeDelta)
{
    _matrix matParent = XMLoadFloat4x4(m_pParentMatrix);
    _vector vOutQuat, vOutPos;

    const _matrix matWorld_GSwordTip = XMLoadFloat4x4(m_pMatGSwordTip) * matParent;
    XMStoreFloat4x4(&m_matWorldGSwordTip_nJolt, matWorld_GSwordTip);
    m_pBodyCom_Attack->Sync_Update(matWorld_GSwordTip);
    m_pBodyCom_Attack->Update(fTimeDelta, matWorld_GSwordTip, vOutQuat, vOutPos);
    XMStoreFloat4x4(&m_matWorldGSwordTip, matWorld_GSwordTip);
    XMStoreFloat3(reinterpret_cast<_float3*>(&m_matWorldGSwordTip._41), vOutPos);


    const _matrix matWorld_GSwordBody = m_Offset_Matrix * XMLoadFloat4x4(m_pMatGSwordBody) * matParent;
    XMStoreFloat4x4(&m_matWorldGSwordBody, matWorld_GSwordBody);
    XMStoreFloat4x4(&m_matWorldGSwordBody_nJolt, matWorld_GSwordBody);

    //_vector vOutQuat2, vOutPos2;
    //m_pBodyCom_Guard->Update(fTimeDelta, matWorld_GSwordBody, vOutQuat2, vOutPos2);
    //XMStoreFloat4x4(&m_matWorldGSwordBody, matWorld_GSwordBody);
    //XMStoreFloat3(reinterpret_cast<_float3*>(&m_matWorldGSwordBody._41), vOutPos2);

    m_pBodyCom_Guard->Sync_Update(matParent);
    m_pBodyCom_BodyAttack->Sync_Update(matParent);
    m_pBodyCom_RangeAttack->Sync_Update(matParent);
    m_pBodyCom_Search->Sync_Update(matParent);

}

void CBody_Khazan_GS::Check_Guarding(_float fTimeDelta)
{
    if (m_isCollGuard_Active)
        m_fJustGuardTime.x += fTimeDelta;

    if (*m_pIsGuarding == true && !m_isCollGuard_Active) {
        m_pBodyCom_Guard->Collision_Active(true);
        m_isCollGuard_Active = true;
        m_fJustGuardTime.x = 0.f;
        m_isJustGuardOnce = false;
    }
    if (*m_pIsGuarding == false && m_isCollGuard_Active) {

        m_pBodyCom_Guard->Collision_Active(false);
        m_isCollGuard_Active = false;
    }
}

void CBody_Khazan_GS::Update_GuardRotation(_float fTimeDelta)
{
    /* 부모에서 처리 */
}

void CBody_Khazan_GS::Start_GuardRotation(_float3 vContactPoint)
{
    // 1. 캐릭터의 위치
    _vector vCharacterPos = m_pParentTransform->Get_State(STATE::POSITION);

    // 2. 캐릭터 -> 접촉점 방향 벡터 계산
    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vCharacterPos;
    vHitDir = XMVectorSetY(vHitDir, 0.f); // XZ 평면으로 투영
    if (XMVectorGetX(XMVector3Length(vHitDir)) < 1e-4f)
    {
        vHitDir = m_pParentTransform->Get_State(STATE::LOOK);
        vHitDir = XMVectorSetY(vHitDir, 0.f);
        vHitDir = XMVector3Normalize(vHitDir);
    }
    else
        vHitDir = XMVector3Normalize(vHitDir);

    /* 부모에서 직접 회전 */
    *m_pParentStatus |= CKhazan_GSword::GUARD_ROTATION_REQUEST;
    XMStoreFloat4(m_pGuardRotationTarget, vHitDir);

}


void CBody_Khazan_GS::Exception_Animaition()
{
    //if(m_pModelCom->Get_CurAnimIndex() == 120 && *m_pModelCom->Get_CurTrackPosition() < 10.f)
    //{
    //    _vector vPos = m_pParentTransform->Get_State(STATE::POSITION);
    //    vPos.m128_f32[2] += 0.32f;
    //    m_pParentTransform->Set_State(STATE::POSITION, vPos);
    //
    //}
      
}

FMOD_CHANNEL** CBody_Khazan_GS::Get_SoundChannel(_int iIndex)
{
    if (m_pChannel.size() <= iIndex)
        m_pChannel.resize(iIndex + 1, nullptr);

    return &m_pChannel[iIndex];
}

HRESULT CBody_Khazan_GS::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Khazan_GS::Bind_Dissolve()
{
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_DissolveTexture", 0), E_FAIL);

    m_pShaderCom->Bind_RawValue("g_fDecreaseAlpha", &m_fDissolveDecreaseAlphaValue, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeWidth", &m_fDissolveEdgeWidth, sizeof(_float));
    m_pShaderCom->Bind_RawValue("g_fEdgeColor", &m_fDissolveColor, sizeof(_float4));

    return S_OK;
}

HRESULT CBody_Khazan_GS::Ready_Components()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Monster_Dissolve"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr), E_FAIL);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(eCurrentLevel), TEXT("Prototype_Component_Model_Khazan_GSword"),
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

    // 모든 파츠 로드
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
    MeshDsc.iTextureIdx = 22;
    MeshDsc.fLifeTime = 0.22f;
    MeshDsc.iDivisionCount = 10.f;
    MeshDsc.vColor = { 0.6f, 0.f, 0.f, 7.843f };
    MeshDsc.vSubColor = { 0.f, 0.f, 0.f, 3.f };
    m_pTrail = dynamic_cast<CMeshTrail*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MeshTrail"), &MeshDsc));

    CMotionTrail::MOTIONTRAIL_DESC MTDesc{};
    MTDesc.pOwnerMasterModel = m_pModelCom;
    MTDesc.HasPartModels = true;
    for (auto pModel : m_RenderParts)
        MTDesc.OwnerPartModels.push_back(pModel);
    MTDesc.Config.vLifeTime = { 0.f, 0.3f };
    MTDesc.Config.vStartColor = { 0.f, 0.f, 0.1f };
    MTDesc.Config.vTargetColor = { 0.5f, 1.f, 0.7f };
    MTDesc.Config.fRimPower = 2.f;
    MTDesc.Config.fRimIntensity = 1.f;
    MTDesc.Config.fEmissiveIntensity = 2.f;
    MTDesc.Config.isIndividualColor = true;
    MTDesc.Config.fColorUpdateSpeed = 1000.f;
    MTDesc.Config.fInterval = 0.05f;
    MTDesc.Config.iMaxFrames = 10.f;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_MotionTrail"),
        TEXT("Com_MotionTrail"), reinterpret_cast<CComponent**>(&m_pMotionTrailCom), &MTDesc)))
        return E_FAIL;


    return S_OK;

}

HRESULT CBody_Khazan_GS::Ready_Colliders()
{
    CBody::BODY_BOXSHAPE_DESC AttackDesc{};
    {
        AttackDesc.vExtent = _float3(1.f, 1.f, 1.8f);
        AttackDesc.eMotion = EMotionType::Kinematic;
        AttackDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        AttackDesc.eShapeType = SHAPE::BOX;
        AttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat4x4(&m_matWorldGSwordBody, XMLoadFloat4x4(m_pMatGSwordBody) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(&m_matWorldGSwordBody));
        AttackDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        AttackDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        AttackDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tAttackCollisionDesc.pGameObject = this;
        m_tAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tAttackCollisionDesc.strName = TEXT("Player_Attack1");
        AttackDesc.pCollisionDesc = &m_tAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        AttackDesc.pCollisionDesc->pInfo = &DamageInfo;
        AttackDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body1"), reinterpret_cast<CComponent**>(&m_pBodyCom_Attack), &AttackDesc)))
            return E_FAIL;

    }

    CBody::BODY_SPHERESHAPE_DESC RangeAttackDesc{};
    {
        RangeAttackDesc.fRadius = 4.5f;
        RangeAttackDesc.bIsTrigger = true;
        RangeAttackDesc.bStartActive = true;
        RangeAttackDesc.eMotion = EMotionType::Kinematic;
        RangeAttackDesc.eQuality = EMotionQuality::Discrete;
        RangeAttackDesc.eShapeType = SHAPE::SPHERE;
        RangeAttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat3(&RangeAttackDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&RangeAttackDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        RangeAttackDesc.vShapeOffset = _float3(0.f, -1.25f, 0.f);
        m_tRangeAttackCollisionDesc.pGameObject = this;
        m_tRangeAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tRangeAttackCollisionDesc.strName = TEXT("Player_RangeAttack");
        RangeAttackDesc.pCollisionDesc = &m_tRangeAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        RangeAttackDesc.pCollisionDesc->pInfo = &DamageInfo;
        RangeAttackDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body2"), reinterpret_cast<CComponent**>(&m_pBodyCom_RangeAttack), &RangeAttackDesc)))
            return E_FAIL;

    }

    CBody::BODY_SPHERESHAPE_DESC BodyAttackDesc{};
    {
        BodyAttackDesc.fRadius = 1.f;
        BodyAttackDesc.bIsTrigger = true;
        BodyAttackDesc.bStartActive = true;
        BodyAttackDesc.eMotion = EMotionType::Kinematic;
        BodyAttackDesc.eQuality = EMotionQuality::Discrete;
        BodyAttackDesc.eShapeType = SHAPE::SPHERE;
        BodyAttackDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

        XMStoreFloat3(&BodyAttackDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&BodyAttackDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        BodyAttackDesc.vShapeOffset = _float3(0.f, 0.5f, 0.f);
        m_tBodyAttackCollisionDesc.pGameObject = this;
        m_tBodyAttackCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tBodyAttackCollisionDesc.strName = TEXT("Player_BodyAttack");
        BodyAttackDesc.pCollisionDesc = &m_tBodyAttackCollisionDesc;

        DAMAGEINFO DamageInfo = {};
        DamageInfo.fDamage = 50.f;
        DamageInfo.eHitreaction = HITREACTION::KNOCKBACK_NORMAL;
        AttackDesc.pCollisionDesc->pInfo = &DamageInfo;
        AttackDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body3"), reinterpret_cast<CComponent**>(&m_pBodyCom_BodyAttack), &BodyAttackDesc)))
            return E_FAIL;
    }

    CBody::BODY_BOXSHAPE_DESC GuardDesc{};
    {
        GuardDesc.vExtent = _float3(0.4f, 1.1f, 0.4f);
        GuardDesc.eMotion = EMotionType::Kinematic;
        GuardDesc.eQuality = EMotionQuality::Discrete; // 기본 모드
        GuardDesc.eShapeType = SHAPE::BOX;
        GuardDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);

       // XMStoreFloat4x4(&m_matWorldGSwordBody, XMLoadFloat4x4(m_pMatGSwordBody) * XMLoadFloat4x4(m_pParentMatrix));
        _vector vScale, vQuat, vTrans;
        XMMatrixDecompose(&vScale, &vQuat, &vTrans, XMLoadFloat4x4(m_pParentMatrix));
        GuardDesc.vPos = _float3(vTrans.m128_f32[0], vTrans.m128_f32[1], vTrans.m128_f32[2]);
        GuardDesc.vQuat = _float4(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]);
        GuardDesc.vShapeOffset = _float3(0.f, 0.55f, 0.f);
        m_tGuardCollisionDesc.pGameObject = this;
        m_tGuardCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK);
        m_tGuardCollisionDesc.strName = TEXT("Player_Guard");
        GuardDesc.pCollisionDesc = &m_tGuardCollisionDesc;
        GuardDesc.bIsTrigger = true;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body4"), reinterpret_cast<CComponent**>(&m_pBodyCom_Guard), &GuardDesc)))
            return E_FAIL;
    }

    CBody::BODY_SPHERESHAPE_DESC SearchDesc{};
    {
        SearchDesc.fRadius = 14.f;
        SearchDesc.bIsTrigger = true;
        SearchDesc.bStartActive = true;
        SearchDesc.eMotion = EMotionType::Kinematic;
        SearchDesc.eQuality = EMotionQuality::Discrete;
        SearchDesc.eShapeType = SHAPE::SPHERE;
        SearchDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH);

        XMStoreFloat3(&SearchDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION));
        XMStoreFloat4(&SearchDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());
        SearchDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
        m_tSearchCollisionDesc.pGameObject = this;
        m_tSearchCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER_SEARCH);
        m_tSearchCollisionDesc.strName = TEXT("Player_Search");
        SearchDesc.pCollisionDesc = &m_tSearchCollisionDesc;

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
            TEXT("Com_Body5"), reinterpret_cast<CComponent**>(&m_pBodyCom_Search), &SearchDesc)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CBody_Khazan_GS::Ready_AnimationEvents()
{ 
#pragma region Effect
    m_pModelCom->Register_Event("GS_WeakAtk01_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust"), m_pParentTransform->Get_State(STATE::POSITION));
        // 디스토션
        Start_LongDistortion();
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(50.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    m_pModelCom->Register_Event("GS_WeakAtk01_Charge_Ground", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("FerociousMomentum0"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        Spawn_EmissiveDecal(true);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });
    
    m_pModelCom->Register_Event("GS_WeakAtk02_SowardFX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt)); 
        m_iFXIdx_Spining = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_SowardFX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining, rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]); 
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]); 
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_SowardFX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
        });

    m_pModelCom->Register_Event("GS_WeakAtk02_BloodTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Trail"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        }); 

    //강공 차지ww
    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust10"), m_pParentTransform->Get_State(STATE::POSITION)); //흰색

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        }); 

    //강기발현
        //강기발현 차지
    m_pModelCom->Register_Event("GS_StrongAtk01_Force_Release_Charge", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust9"), m_pParentTransform->Get_State(STATE::POSITION));
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 0.6f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

        //Strong Charge - 해금 X
    //m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { });

    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        if (Has_Status(CKhazan_GSword::CHARGING_STRONG_ATTACK)) //차징 됨
        {
            if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::MANIFESTSTRENGTH))    //강기발현
            {
                _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Trail_V"), rot, m_pParentTransform->Get_State(STATE::POSITION));
            }
            //else if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::LIMIT_BREAK))    //한계극복
            //    ;
            else
                ; //그냥 차징 강공
        }
        else
            ; //그냥 차징 강공

        });

    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { 
        if (Has_Status(CKhazan_GSword::CHARGING_STRONG_ATTACK)) //차징 됨
        {
            if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::MANIFESTSTRENGTH))    //강기발현 
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke_Red"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);  
            else
                ; // 그냥 차징 강공
        }
        else
            ; //그냥 차징 안한 강공
        });

    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        if (Has_Status(CKhazan_GSword::CHARGING_STRONG_ATTACK)) //차징 됨
        {
            if (m_pClientInstance->Is_UsedSkill(CPlayerData_Manager::GSWORDSKILL::MANIFESTSTRENGTH))    //강기발현
            {
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Manifest_Strength_Land"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
                // 카메라 쉐이킹
                CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.f);
                // 선형 피 데칼
                Spawn_LinearBloodDecal();
                Start_DefaultDistortion();
            } 
            else
            {
                m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("GS_StrongATK"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
                CClientInstance::GetInstance()->ActiveCamera_Shaking(0.7f, 1.f);
            }
        }
        else
        {
            m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("GS_StrongATK"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
            CClientInstance::GetInstance()->ActiveCamera_Shaking(0.7f, 1.f);
        }
        });

    //거인사냥
    m_pModelCom->Register_Event("GS_Soulbringer_Land_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt));
        m_iFXIdx_Spining = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);  
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle"), m_pParentTransform->Get_State(STATE::POSITION));
        Start_DefaultDistortion();
        m_isEableGiantHuntEvent = true;
        m_iCurAnimEventIndex = m_pModelCom->Get_CurAnimIndex();
        });

    m_pModelCom->Register_Event("GS_Soulbringer_Land_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining, rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("GS_Soulbringer_Land_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Giant_Hunt_Land"), XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        // 이미시브 데칼
        Spawn_EmissiveDecal(false);
        m_isEableGiantHuntEvent = false;
        });

    m_pModelCom->Register_Event("GS_AsheFork_Charge_Blust", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 0.6f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        }); 

    //거대한 포효
    m_pModelCom->Register_Event("GS_WarDeclaration_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Giant_Roar"), m_pParentTransform->Get_State(STATE::POSITION));
        Spawn_CrackDecal();
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        Start_DefaultVignette();
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(75.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    //정면 돌파
    m_pModelCom->Register_Event("GS_ChargeCrash_Wind", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_iFXIdx_BodyWind = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Wind"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        // 레디얼 블러
        Start_DefaultRadialBlur();
        Start_DefaultDistortion();
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(40.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 0.6f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    m_pModelCom->Register_Event("GS_ChargeCrash_Wind", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Body_Wind"), m_iFXIdx_BodyWind, rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_ChargeCrash_Wind", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle"), m_pParentTransform->Get_State(STATE::POSITION));
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.5f, 1.5f);
        });

    //숨통끊기 선혈
    m_pModelCom->Register_Event("GS_GhostSlash_Trail1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_iFXIdx_Trail = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Dawn_BloodTrail1"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        // 레디얼 블러
        Start_DefaultRadialBlur();
        // 비네트
        Start_DefaultVignette();
        });

    m_pModelCom->Register_Event("GS_GhostSlash_Trail2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Dawn_BloodTrail2"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Dawn_BloodTrail1"), m_iFXIdx_Trail, rot, m_pParentTransform->Get_State(STATE::POSITION));
        });

    m_pModelCom->Register_Event("GS_GhostSlash_Trail1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("GS_GhostSlash_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt), XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90)));
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger1"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle2"), rot, m_pParentTransform->Get_State(STATE::POSITION));

        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(45.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    // 내재된 분노 
    m_pModelCom->Register_Event("GS_RasingFurry_Explosion", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Inner_Range_Ground"), m_pParentTransform->Get_State(STATE::POSITION));
        Spawn_CircleBloodDecal();
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        Start_DefaultVignette();
        Start_FullScreenDistortion();
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(75.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 0.6f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    // 귀신 어둠의 그림자 
     m_pModelCom->Register_Event("GS_GhostLiberation_Blust", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("particle2"), m_pParentTransform->Get_State(STATE::POSITION));
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt));
        m_iFXIdx_Spining = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        Start_DefaultDistortion();
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(45.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 0.6f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    m_pModelCom->Register_Event("GS_GhostLiberation_Landing", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);
        _vector rot = Decompose_Rotation(XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt));
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining, rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("GS_GhostLiberation_Landing", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger0"), m_iFXIdx_Spining);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Giant_Hunt_Land"), XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        // 이미시브 데칼
        Spawn_EmissiveDecal(false);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Ghost_Dark_Shadow_Land"), XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        });

    //한계극복
        //한계극복 차징
    m_pModelCom->Register_Event("GS_StrongAtk01_Charge_Unlock_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust9"), m_pParentTransform->Get_State(STATE::POSITION)); //빨강!
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(40.f);
        tMod.iPriority = 1.f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });

    m_pModelCom->Register_Event("GS_Apocalypse_Land", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Trail_V"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        Start_LongRadialBlur();
        Start_DefaultVignette();
        });

    m_pModelCom->Register_Event("GS_Apocalypse_Land", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("SpiningCharger_Smoke_Red"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("GS_Apocalypse_Land", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Manifest_Strength_Land"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(2.f, 1.f);
        Spawn_LinearBloodDecal();
        });
    

    m_pModelCom->Register_Event("GS_StrongAtk02_Charge_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("FerociousMomentum0"), rot, m_pParentTransform->Get_State(STATE::POSITION));
        Spawn_EmissiveDecal(true);
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    // Trail
    m_pModelCom->Register_Event("WeakAtk01_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("WeakAtk02_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("WeakAtk03_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("StrongAtk_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("GhostSlashAtk_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("GhostSlashCharge_Turn_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("ChargeCrash_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });

    m_pModelCom->Register_Event("DodgeAtk_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_BrightTrail(); });
    m_pModelCom->Register_Event("DodgeAtk_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("DodgeAtk_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    m_pModelCom->Register_Event("WeakAtk01_ChargeAtk_Trail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { Set_BrightTrail(); });
    m_pModelCom->Register_Event("WeakAtk01_ChargeAtk_Trail", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() { FX_Trail(); });
    m_pModelCom->Register_Event("WeakAtk01_ChargeAtk_Trail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { Set_BaseTrail(); });

    m_pModelCom->Register_Event("Grapple_ChargeArm_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
        _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
        EffectID_SpiralSpear = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), world, world.r[3]);
        BrutalAtk_ScreenEffect0();
        });
    m_pModelCom->Register_Event("Grapple_ChargeArm_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
        _matrix mat_arm = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrix("Muscle_R_ForeTwist1"));
        _matrix world = mat_arm * XMLoadFloat4x4(m_pParentMatrix);
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear, world, world.r[3]);
        });
    //m_pModelCom->Register_Event("Grapple_ChargeArm_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear);
    //    });

    m_pModelCom->Register_Event("Grapple_Sting_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutalParticle"), m_pParentTransform->Get_WorldMatrix(), XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal"), XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        });

    m_pModelCom->Register_Event("Grapple_Charge0_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust12"), m_pParentTransform->Get_WorldMatrix().r[3]);
        });

    m_pModelCom->Register_Event("Grapple_BackBlust_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Brutal_Spark_GS"), m_pParentTransform->Get_WorldMatrix(), BodyCenter());
        }); 

    m_pModelCom->Register_Event("Grapple_Charge1_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Blust5"), m_pParentTransform->Get_WorldMatrix(), BodyCenter());
        });

    m_pModelCom->Register_Event("Grapple_Trail_FX", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  Set_BrightTrail(); 
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("GrappleTrail"), rot, m_pParentTransform->Get_WorldMatrix().r[3]);
        BrutalAtk_ScreenEffect1();
        });

    m_pModelCom->Register_Event("Grapple_Trail_FX", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {  FX_Trail(); });
    m_pModelCom->Register_Event("Grapple_Trail_FX", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { 
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("blust_brutal_GS"), m_pParentTransform->Get_WorldMatrix(), BodyCenter());
        _vector rot = Decompose_Rotation(m_pParentTransform->Get_WorldMatrix());
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("GS_StrongATK"), rot, XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt).r[3]);
        m_pGameInstance->Stop_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("brutal_hand"), EffectID_SpiralSpear);
        Set_BaseTrail();
        });

#pragma endregion



    // 숨통 끊기
    m_pModelCom->Register_Event("GhostSlash_Atk_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    // 숨통 끊기 : 태동
    m_pModelCom->Register_Event("GhostSlash_Charge03_Turn_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 레디얼 블러
        Start_LongRadialBlur();
        FOVModifier tMod{};
        tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
        tMod.fDuration = 1.5f;
        tMod.fFrom = XMConvertToRadians(60.f);
        tMod.fTo = XMConvertToRadians(70.f);
        tMod.iPriority = 1.f;
        tMod.fOutDuration = 0.6f;
        tMod.Ease = EaseOutQuad;
        m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);
        });
    m_pModelCom->Register_Event("GhostSlash_Charge03_Turn_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.f);
        });

    // 거인 사냥
    m_pModelCom->Register_Event("AsheFork_Atk_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 모션 트레일 시작
        Trigger_MotionTrail(TEXT("MT_Life5_RedGray"), true);
        m_isEnableAnimEvent = true;
        m_iCurAnimEventIndex = m_pModelCom->Get_CurAnimIndex();
        });
    m_pModelCom->Register_Event("AsheFork_Atk_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        // 모션 트레일 끝
        Trigger_MotionTrail(TEXT("MT_Life5_RedGray"), false);
        // 비네트
        Start_DefaultVignette();
        // 레디얼 블러
        Start_LongRadialBlur();
        m_isEnableAnimEvent = false;
        });

    // 귀신 : 어둠의 그림자
    m_pModelCom->Register_Event("GhostLiberation_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        // 모션 트레일 시작
        Trigger_MotionTrail(TEXT("MT_Life5_RedGray"), true);
        m_isEnableAnimEvent = true;
        m_iCurAnimEventIndex = m_pModelCom->Get_CurAnimIndex();
        });
    m_pModelCom->Register_Event("GhostLiberation_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        // 모션 트레일 끝
        Trigger_MotionTrail(TEXT("MT_Life5_RedGray"), false);
        // 비네트
        Start_DefaultVignette();
        // 레디얼 블러
        Start_LongRadialBlur();
        m_isEnableAnimEvent = false;
        });

    // 정면 돌파
    m_pModelCom->Register_Event("ChargeCrash_Atk_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        // 카메라 쉐이킹
        CClientInstance::GetInstance()->ActiveCamera_Shaking(1.f, 1.5f);
        });

    // 한계 극복
    m_pModelCom->Register_Event("Apocalypse_Atk_ScreenEffect", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.3f, 0.5f, 0.2f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.3f, 0.5f, 0.2f);
        m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.3f, 0.5f, 0.2f);
        });

    // 전투의 굴레
    //m_pModelCom->Register_Event("DodgeAtk_MotionTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
    //    Trigger_MotionTrail(TEXT("MT_Life5_RedGray"), true);
    //    Start_LongRadialBlur();
    //    Start_DefaultVignette();
    //    });
    //m_pModelCom->Register_Event("DodgeAtk_MotionTrail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
    //    Trigger_MotionTrail(TEXT("MT_Life5_RedGray"), false);
    //    CClientInstance::GetInstance()->ActiveCamera_Shaking(0.7f, 1.5f);
    //    });


    // 닷지
    m_pModelCom->Register_Event("Dodge_MotionTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        if(Has_Status(CKhazan_GSword::GSWORD)) Trigger_MotionTrail(TEXT("MT_Int05_RedGray"), true);
        else if (Has_Status(CKhazan_GSword::SPEAR)) Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), true);
        else  Trigger_MotionTrail(TEXT("MT_Common_WhiteDefault"), true);
        m_isEnableAnimEvent = true;
        m_iCurAnimEventIndex = m_pModelCom->Get_CurAnimIndex();
        });
    m_pModelCom->Register_Event("Dodge_MotionTrail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT(""), false);
        m_isEnableAnimEvent = false;
        //Remove_Status(CKhazan_GSword::DODGE_ENDING);
        });

    // 닷지 어택
    m_pModelCom->Register_Event("DodgeAtk_MotionTrail", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        if (Has_Status(CKhazan_GSword::GSWORD)) Trigger_MotionTrail(TEXT("MT_Int05_RedGray"), true);
        else if (Has_Status(CKhazan_GSword::SPEAR)) Trigger_MotionTrail(TEXT("MT_Common_BlueGray"), true);
        else  Trigger_MotionTrail(TEXT("MT_Common_WhiteDefault"), true);
        m_isEnableAnimEvent = true;
        m_iCurAnimEventIndex = m_pModelCom->Get_CurAnimIndex();
        });
    m_pModelCom->Register_Event("DodgeAtk_MotionTrail", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        Trigger_MotionTrail(TEXT(""), false);
        m_isEnableAnimEvent = false;
        });
#pragma endregion

#pragma region Collider  
    m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {  m_pBodyCom_Attack->Collision_Active(true); m_isNotifyAttacking = true; });
 //   m_pModelCom->Register_Event("AttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() { m_pBodyCom_Attack->Collision_Active(false); });

    m_pModelCom->Register_Event("RangeAttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { m_pBodyCom_RangeAttack->Collision_Active(true); m_isNotifyAttacking = true; });
  //  m_pModelCom->Register_Event("RangeAttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()  { m_pBodyCom_RangeAttack->Collision_Active(false); });

    m_pModelCom->Register_Event("BodyAttackTiming", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { m_pBodyCom_BodyAttack->Collision_Active(true); m_isNotifyAttacking = true; });
  //  m_pModelCom->Register_Event("BodyAttackTiming", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]()  { m_pBodyCom_BodyAttack->Collision_Active(false); });

    m_pModelCom->Register_Event("AttackCollisionOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {   m_pBodyCom_Attack->Collision_Active(false); });

    m_pModelCom->Register_Event("HEAL1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 
        m_pPlayerData->fCulHp += m_pPlayerData->fLachrymaItemRegen;
        if (m_pPlayerData->fCulHp > m_pPlayerData->fMaxHp)
            m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;
        }); //라크리마

    m_pModelCom->Register_Event("HEAL2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() { 

        if (*m_pHealIndex == 1)
            m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;

        if (*m_pHealIndex == 3)
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 3 });

        if (*m_pHealIndex == 4)
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 4 });

        if (*m_pHealIndex == 5)
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 5 });

        if (*m_pHealIndex == 6)
            m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::ITEM_ACTIVE), EVENT_ATICVE_ITEM{ 6 });
        
        }); //힐템

    m_pModelCom->Register_Event("WeaponOn", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGSword->Set_Equipped(false);
        });
    m_pModelCom->Register_Event("WeaponOn", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
        m_pGSword->Set_Equipped(true);

        if (!Has_Status(CKhazan_GSword::STATUE_MODE))
            m_pClientInstance->Set_PlayerInput(true);

        });
    m_pModelCom->Register_Event("WeaponOff", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
        m_pGSword->Set_Equipped(false);
        m_pClientInstance->Set_PlayerInput(false);
        });

#pragma endregion

    return S_OK;
}
HRESULT CBody_Khazan_GS::Ready_AnimationEvent_SFX()
{
    /* (노티파이 키 값 , 노티파이 최대 수, 진입점, 사운드 그룹 타입, 볼륨, 어떤 채널사용 ) */
    auto Register_EventGroup = [&](const string& strEventKey, _int iCount, ANIM_EVENT_TRIGGERTYPE eTrigger, SOUND_TYPE eSoundType, _float fVolume, SOUND_CHANNEL eChannelType) {

        if (iCount <= 0) return;

        for (_int i = 1; i <= iCount; ++i)
        {
            string strTempEventKey = strEventKey;

            std::stringstream ss;
            ss << std::setw(2) << std::setfill('0') << i;
            strTempEventKey += "_" + ss.str();

            m_pModelCom->Register_Event(strTempEventKey, eTrigger, [this, eSoundType, fVolume, eChannelType]() {
              if(m_isPlaySound) m_pGameInstance->PlaySoundOnce(m_pSoundHelper->Get_NextSoundKey(eSoundType, eChannelType), fVolume, Get_SoundChannel(eChannelType)); });
        }
    };


    /* Idle*/
    Register_EventGroup("SFX_Idle", 3, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::IDLE, 10.f, SOUND_CHANNEL::VOICE);
    //Register_EventGroup("SFX_Idle_Rattle", 3, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::IDLE_RATTLE, 10.f, SOUND_CHANNEL::MOVE);

    /* Move */
    Register_EventGroup("SFX_Move_Injure_R", 9, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_INJURE_L, 0.35f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Injure_L", 9, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_INJURE_R, 0.35f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Walk", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_WALK, 1.5f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Run", 12, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_RUN, 1.f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Sprint_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT_START, 0.6f, SOUND_CHANNEL::VOICE);
    Register_EventGroup("SFX_Move_Sprint", 16, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT, 0.45f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Sprint_Stop", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT_STOP, 0.5f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Sprint_Rattle", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_SPRINT_RATTLE, 0.45f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Dodge_Front", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_DODGE_FRONT, 0.45f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Dodge_Rear", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_DODGE_REAR, 0.45f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Dodge_Side", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_DODGE_SIDE, 0.45f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Fall", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_FALL, 0.7f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Randing", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_RANDING, 1.f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Climb", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_RANDING, 0.5f, SOUND_CHANNEL::MOVE);
    Register_EventGroup("SFX_Move_Climb_End", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::MOVE_RANDING, 0.5f, SOUND_CHANNEL::MOVE);

    /* Attack */
    Register_EventGroup("SFX_Attack_Gs_Weak1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK1, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak1_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK1_IMPACT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK2, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak3", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK3, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak3_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK3_IMPACT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak3_Foley", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK3_FOLEY, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak1_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK1_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak1_Charging_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK1_CHARGING_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak2_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK2_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Weak2_Charging_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_WEAK2_CHARGING_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Strong", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_STRONG, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Strong_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_STRONG_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Strong_Charging_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_STRONG_CHARGING_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Dodge_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_DODGE_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Dodge_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_DODGE_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Dodge_Impact", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_DODGE_IMPACT, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Brutal_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_BRUTAL_START, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Brutal1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_BRUTAL1, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Brutal2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_BRUTAL2, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Attack_Gs_Fall", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::ATTACK_GS_FALL, 0.8f, SOUND_CHANNEL::WEAPON);

    /* Skill */
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Start", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_START, 0.8f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Success", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_SUCCESS, 0.8f, SOUND_CHANNEL::EFFECT2);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Bloodshed_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_BLOODSHED_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Bloodshed_Charging_Success", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_BLOODSHED_CHARGING_SUCCESS, 0.8f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Bloodshed_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_BLOODSHED_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Breathtaking_Bloodshed_Success", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREATHTAKING_BLOODSHED_SUCCESS, 0.8f, SOUND_CHANNEL::EFFECT2);
    Register_EventGroup("SFX_Skill_Gs_Gianthunt_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_GIANTHUNT_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Gianthunt_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_GIANTHUNT_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Phantom_Shadowofdarkness_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_PHANTOM_SHADOWOFDARKNESS_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Phantom_Shadowofdarkness_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_PHANTOM_SHADOWOFDARKNESS_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Limit_Break_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_LIMIT_BREAK_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Limit_Break_Loop", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_LIMIT_BREAK_LOOP, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Limit_Break_Charging_FX", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_LIMIT_BREAK_CHARGING_FX, 0.8f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Skill_Gs_Limit_Break_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_LIMIT_BREAK_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Limit_Break_End", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_LIMIT_BREAK_END, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Break_Through_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREAK_THROUGH_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Break_Through_Run", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREAK_THROUGH_RUN, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Break_Through_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_BREAK_THROUGH_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Warcry_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_WARCRY_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Warcry_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_WARCRY_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Inner_Fury_Charging", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_INNER_FURY_CHARGING, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Skill_Gs_Inner_Fury_Charging_Success", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_INNER_FURY_CHARGING_SUCCESS, 0.8f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Skill_Gs_Inner_Fury_Attack", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::SKILL_GS_INNER_FURY_ATTACK, 0.8f, SOUND_CHANNEL::WEAPON);

    /* Disc */ 
    Register_EventGroup("SFX_GS_Weapon_Rustle", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GS_WEAPON_RUSTLE, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_GS_Pose_Return1", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GS_POSE_RETURN1, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_GS_Pose_Return2", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GS_POSE_RETURN2, 0.8f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_GS_Pose_Return3", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GS_POSE_RETURN3, 0.8f, SOUND_CHANNEL::WEAPON);

    /* Damaged */
    Register_EventGroup("SFX_Damaged_Normal", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::DAMAGED_NORMAL, 1.f, SOUND_CHANNEL::VOICE);
    Register_EventGroup("SFX_Damaged_Hard", 2, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::DAMAGED_HARD, 1.f, SOUND_CHANNEL::VOICE);

    /* Guard*/
    Register_EventGroup("SFX_Guard_On_Gs", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_ON_GS, 1.f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Guard_Off_Gs", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_OFF_GS, 1.f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Guard_Tunal", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_TUNAL, 1.f, SOUND_CHANNEL::EFFECT1);
    Register_EventGroup("SFX_Guard_Success", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::GUARD_SUCCESS, 1.f, SOUND_CHANNEL::WEAPON);
    Register_EventGroup("SFX_Justguard_Effect", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::JUSTGUARD_EFFECT, 1.f, SOUND_CHANNEL::EFFECT2);
    Register_EventGroup("SFX_Justguard", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::JUSTGUARD, 1.f, SOUND_CHANNEL::WEAPON);

    /* Interaction */
    Register_EventGroup("SFX_Interaction_Lacrima_Get", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_LACRIMA_GET, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Heal", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_HEAL, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Lantern_On", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_LANTERN_ON, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Lantern_Off", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_LANTERN_OFF, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Gs_Armed", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_GS_ARMED, 1.f, SOUND_CHANNEL::INTERACTION);
    Register_EventGroup("SFX_Interaction_Gs_Unarmed", 1, ANIM_EVENT_TRIGGERTYPE::ENTER, SOUND_TYPE::INTERACTION_GS_UNARMED, 1.f, SOUND_CHANNEL::INTERACTION);

    return S_OK;
}


HRESULT CBody_Khazan_GS::Ready_Equipment()
{
    const auto& equipData = m_pClientInstance->Get_PlayerEquipment();

    // 각 슬롯별 초기 장비 설정
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

void CBody_Khazan_GS::Equip_Part(EQUIPMENTTYPE eType, const _wstring& strPartName)
{
    if (eType == EQUIPMENTTYPE::SPEAR
        || eType == EQUIPMENTTYPE::GSWORD
        || eType == EQUIPMENTTYPE::NONE)
        return;

    // 기존 장착 파츠 해제
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

    // 새 파츠 장착
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

    // 빠른 렌더용 
    Update_QuickRenderCache();
}

void CBody_Khazan_GS::Update_QuickRenderCache()
{
    for (auto pModel : m_RenderParts)
        Safe_Release(pModel);
    m_RenderParts.clear();

    /* 렌더 순서 적용 가능 */
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

    /* todo !! 여기에 모션트레일컴포넌트에  랜더용 파츠모델 바꼈다고 넘겨주기. */
    m_pMotionTrailCom->Update_PartModels(m_RenderParts);

    // Part Model이 있는 경우!!

    // Part Model이 없고 Master Model만 있는 경우!! (무기)
    // m_pMotionTrailCom->Update_MasterModel(m_pModelCom);
}

_vector CBody_Khazan_GS::Decompose_Rotation(_matrix W, _vector localRot, _vector offset)
{
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

void CBody_Khazan_GS::Spawn_EmissiveDecal(_bool isUseOffset)
{
    DECAL_DESC Desc{};

    if (isUseOffset)
    {
        Desc.fLifeTime = 5.f;
        Desc.vFadeTime = _float2(0.5f, 0.5f);
        Desc.eType = DECALTYPE::EMISSIVE;
        _matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);
        _vector vRight = ParentMatrix.r[0];
        _vector vLook = ParentMatrix.r[2];
        _vector vPosition = ParentMatrix.r[3];
        vPosition += (vRight * 2.f) + (vLook * 2.f);
        XMStoreFloat3(&Desc.vPosition, vPosition);
        Desc.vScale = _float3(5.5f, 1.5f, 5.5f);
        Desc.EmissiveDesc.vBaseColor = _float3(0.547f, 0.02f, 0.f);
        Desc.EmissiveDesc.vEmissiveColor = _float3(1.f, 0.05f, 0.05f);
        Desc.EmissiveDesc.vBorderColor = _float3(0.f, 0.f, 0.f);
        Desc.EmissiveDesc.fEmissiveMaskPower = 3.f;
        Desc.EmissiveDesc.fEmissiveIntensity = 5.f;
        Desc.isRandomTexture = false;
        Desc.iTextureIndex = 2;
    }
    else
    {
        Desc.fLifeTime = 5.f;
        Desc.vFadeTime = _float2(0.5f, 0.5f);
        Desc.eType = DECALTYPE::EMISSIVE;
        _matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);
        _vector vPosition = ParentMatrix.r[3];
        _vector vLook = ParentMatrix.r[2];
        vPosition += (vLook * 1.2f);
        XMStoreFloat3(&Desc.vPosition, vPosition);
        Desc.vScale = _float3(5.5f, 1.5f, 5.5f);
        Desc.EmissiveDesc.vBaseColor = _float3(0.547f, 0.02f, 0.f);
        Desc.EmissiveDesc.vEmissiveColor = _float3(1.f, 0.05f, 0.05f);
        Desc.EmissiveDesc.vBorderColor = _float3(0.f, 0.f, 0.f);
        Desc.EmissiveDesc.fEmissiveMaskPower = 3.f;
        Desc.EmissiveDesc.fEmissiveIntensity = 5.f;
        Desc.isRandomTexture = false;
        Desc.iTextureIndex = 2;
    }

    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), TEXT("Layer_Decal"), Desc);
}

void CBody_Khazan_GS::Spawn_CrackDecal()
{
    DECAL_DESC Desc{};
    Desc.fLifeTime = 5.f;
    Desc.vFadeTime = _float2(0.5f, 0.5f);
    Desc.eType = DECALTYPE::EMISSIVE;
    _matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);
    _vector vPosition = ParentMatrix.r[3];
    XMStoreFloat3(&Desc.vPosition, vPosition);
    Desc.vScale = _float3(5.f, 1.5f, 5.f);
    Desc.EmissiveDesc.vBaseColor = _float3(0.f, 0.f, 0.f);
    Desc.EmissiveDesc.vEmissiveColor = _float3(0.f, 0.f, 0.f);
    Desc.EmissiveDesc.vBorderColor = _float3(1.f, 0.f, 0.f);
    Desc.EmissiveDesc.fEmissiveMaskPower = 1.f;
    Desc.EmissiveDesc.fEmissiveIntensity = 1.f;
    Desc.isRandomTexture = false;
    Desc.iTextureIndex = 7;

    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), TEXT("Layer_Decal"), Desc);
}

void CBody_Khazan_GS::Spawn_CircleBloodDecal()
{
    DECAL_DESC Desc{};
    Desc.fLifeTime = 5.f;
    Desc.vFadeTime = _float2(0.5f, 0.5f);
    Desc.eType = DECALTYPE::CIRCLE;
    _matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);
    _vector vPosition = ParentMatrix.r[3];
    XMStoreFloat3(&Desc.vPosition, vPosition);
    Desc.vScale = _float3(4.5f, 1.5f, 4.5f);
    Desc.vColor = _float3(0.4745f, 0.08f, 0.08f);
    Desc.isRandomTexture = false;
    Desc.iTextureIndex = 0;

    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), TEXT("Layer_Decal"), Desc);
}

void CBody_Khazan_GS::Spawn_LinearBloodDecal()
{
    DECAL_DESC Desc{};
    Desc.fLifeTime = 5.f;
    Desc.vFadeTime = _float2(0.5f, 0.5f);
    Desc.eType = DECALTYPE::LINEAR;
    _matrix ParentMatrix = XMLoadFloat4x4(m_pParentMatrix);
    _vector vPosition = ParentMatrix.r[3];
    _vector vLook = ParentMatrix.r[2];
    vPosition += (vLook * 4.25f);
    XMStoreFloat3(&Desc.vPosition, vPosition);
    //  Desc.vAngle = _float3(0.f, 180.f, 0.f);
    
    // 부모의 Look 벡터를 사용하여 XZ 평면에서의 회전 각도(라디안)을 얻음
    _float fParentRadianY = atan2f(XMVectorGetX(vLook), XMVectorGetZ(vLook));
    _float fParentDegreeY = XMConvertToDegrees(fParentRadianY);
    Desc.vAngle = _float3(0.f, fParentDegreeY + 180.f, 0.f);

    Desc.vScale = _float3(3.f, 1.5f, 5.5f);
    Desc.vColor = _float3(0.4745f, 0.08f, 0.08f);
    Desc.isRandomTexture = false;
    Desc.iTextureIndex = 3;

    m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), TEXT("Layer_Decal"), Desc);
}

void CBody_Khazan_GS::Start_DefaultVignette()
{
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

void CBody_Khazan_GS::Start_DefaultRadialBlur()
{
    RADIAL_BLUR_DESC RBDesc{};
    RBDesc.vCenterUV = _float2(0.5f, 0.5f);
    RBDesc.fSampleRadius = 0.05f;
    RBDesc.vMaskRadius = _float2(0.f, 0.3f);
    RBDesc.fExponent = 1.f;
    RBDesc.iNumSamples = 16;
    RBDesc.fAttenuation = 0.1f;
    RBDesc.fStrength = 0.5f;       // == Target Strength(0 ~ 1) -> 이 강도를 최대값으로 사용하여 보간 적용됨
    RBDesc.fDuration = 1.f;
    RBDesc.vFadeTime = _float2(0.25f, 0.5f);
    m_pGameInstance->Start_RadialBlur(RBDesc);
}

void CBody_Khazan_GS::Start_LongRadialBlur()
{
    RADIAL_BLUR_DESC RBDesc{};
    RBDesc.vCenterUV = _float2(0.5f, 0.5f);
    RBDesc.fSampleRadius = 0.05f;
    RBDesc.vMaskRadius = _float2(0.f, 0.3f);
    RBDesc.fExponent = 1.f;
    RBDesc.iNumSamples = 16;
    RBDesc.fAttenuation = 0.1f;
    RBDesc.fStrength = 0.5f;       // == Target Strength(0 ~ 1) -> 이 강도를 최대값으로 사용하여 보간 적용됨
    RBDesc.fDuration = 2.f;
    RBDesc.vFadeTime = _float2(0.25f, 0.5f);
    m_pGameInstance->Start_RadialBlur(RBDesc);
}

void CBody_Khazan_GS::Start_LongDistortion()
{
    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 0.5f;
    Desc.fPower = 0.01f;
    Desc.fDuration = 2.25f;
    Desc.vFadeTime = _float2(1.8f, 0.2f);
    Desc.fSpeed = 2.f;
    Desc.iNoiseIndex = 4;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_GS::Start_DefaultDistortion()
{
    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    _float fPosY = XMVectorGetY(vCenterPos);
    _float fOffset = 2.f;
    vCenterPos = XMVectorSetY(vCenterPos, fPosY + fOffset);
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 1.f;
    Desc.fPower = 0.07f;
    Desc.fDuration = 0.5f;
    Desc.vFadeTime = _float2(0.1f, 0.3f);
    Desc.fSpeed = 2.f;
    Desc.iNoiseIndex = 2;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_GS::Start_FullScreenDistortion()
{
    // Distortion
    DISTORTION_DESC Desc{};
    _vector vCenterPos = m_pParentTransform->Get_WorldMatrix().r[3];
    XMStoreFloat3(&Desc.vCenter, vCenterPos);
    Desc.fRange = 5.f;
    Desc.fPower = 0.025f;
    Desc.fDuration = 1.f;
    Desc.vFadeTime = _float2(0.2f, 0.5f);
    Desc.fSpeed = 2.f;
    Desc.iNoiseIndex = 15;
    m_pGameInstance->Start_Distortion(Desc);
}

void CBody_Khazan_GS::FX_Trail()
{
    _matrix SwordTipMatrix = XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt);
    _matrix SwordHandMatrix = XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt);
    _vector vTipUp = SwordTipMatrix.r[0];
    _vector vTipPos = SwordTipMatrix.r[3];
    _vector vHandPos = SwordHandMatrix.r[3];
    m_pTrail->Add_ControlPoint(vTipPos, vHandPos);


    if (m_iTrailType)
        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("TrailParticle_R"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);
    //if (m_iTrailType)
    //{
    //    m_TrailParticleTime += 1.f;
    //    if (m_TrailParticleTime > 3.f)
    //        m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("TrailParticle_R"), XMLoadFloat4x4(&m_matWorldGSwordTip_nJolt).r[3]);
    //}
    //else
    //    m_TrailParticleTime = 0.f;
}

void CBody_Khazan_GS::Set_BaseTrail()
{
    TRAIL_CONFIG Config{};
    Config.iTextureIdx = 22;
    Config.fLifeTime = 0.22f;
    Config.iDivisionCount = 10.f;
    Config.vColor = { 0.6f, 0.f, 0.f, 7.843f };
    Config.vSubColor = { 0.f, 0.f, 0.f, 3.f };
    m_pTrail->Set_TrailConfig(Config);
    m_iTrailType = 0;
}

void CBody_Khazan_GS::Set_BrightTrail()
{
    TRAIL_CONFIG Config{};
    Config.iTextureIdx = 22;
    Config.fLifeTime = 0.22f;
    Config.iDivisionCount = 10.f;
    Config.vColor = { 1.529f, 0.f, 0.f, 1.f };
    Config.vSubColor = { 0.f, 0.f, 0.f, 10.f };
    m_pTrail->Set_TrailConfig(Config);
    m_iTrailType = 1;
}

void CBody_Khazan_GS::Spawn_Guard_FX()
{ 
    _matrix mat = XMLoadFloat4x4(&m_matWorldGSwordBody_nJolt);
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

_vector CBody_Khazan_GS::BodyCenter()
{
    _vector pos = m_pParentTransform->Get_WorldMatrix().r[3];
    pos = XMVectorSetY(pos, XMVectorGetY(pos) + 1.2f);
    return pos;
}

void CBody_Khazan_GS::BrutalAtk_ScreenEffect0()
{
    // 레디얼 블러 짧게
    RADIAL_BLUR_DESC RBDesc{};
    RBDesc.vCenterUV = _float2(0.5f, 0.5f);
    RBDesc.fSampleRadius = 0.05f;
    RBDesc.vMaskRadius = _float2(0.f, 0.3f);
    RBDesc.fExponent = 1.f;
    RBDesc.iNumSamples = 16;
    RBDesc.fAttenuation = 0.1f;
    RBDesc.fStrength = 0.7f;       // == Target Strength(0 ~ 1) -> 이 강도를 최대값으로 사용하여 보간 적용됨
    RBDesc.fDuration = 2.f;
    RBDesc.vFadeTime = _float2(1.5f, 0.5f);
    m_pGameInstance->Start_RadialBlur(RBDesc);

    // Fov 좁게
    FOVModifier tMod{};
    tMod.eMode = FOVModifier::FOV_MODE::MULTIPLY;
    tMod.fDuration = 1.f;
    tMod.fFrom = XMConvertToRadians(60.f);
    tMod.fTo = XMConvertToRadians(50.f);
    tMod.iPriority = 1.f;
    tMod.Ease = EaseOutQuad;
    m_pClientInstance->ActiveCamera_PushFOVModifier(tMod);

    VIGNETTE_CONFIG Config{};
    Config.vColor = _float3(0.f, 0.f, 0.f);
    Config.fPower = 3.5f;
    Config.fMinIntensity = 0.f;
    Config.fMaxIntensity = 4.f;
    Config.fDuration = 2.f;
    Config.vFadeTime = _float2(1.5f, 0.5f);
    m_pGameInstance->Start_VignetteAnimation(Config);
}

void CBody_Khazan_GS::BrutalAtk_ScreenEffect1()
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
    Config.fMaxIntensity = 5.f;
    Config.fDuration = 0.75f;
    Config.vFadeTime = _float2(0.35f, 0.35f);
    m_pGameInstance->Start_VignetteAnimation(Config);

    // 히트스탑
    m_pGameInstance->Start_HitStop(TIME_CHANNEL::PLAYER, 0.5f, 0.5f, 0.5f);
    m_pGameInstance->Start_HitStop(TIME_CHANNEL::EFFECT, 0.5f, 0.5f, 0.5f);
    m_pGameInstance->Start_HitStop(TIME_CHANNEL::ENEMY, 0.5f, 0.5f, 0.5f);

    // 카메라쉐이킹강
}

CBody_Khazan_GS* CBody_Khazan_GS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBody_Khazan_GS* pInstance = new CBody_Khazan_GS(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Khazan_GS::Clone(void* pArg)
{
    CBody_Khazan_GS* pInstance = new CBody_Khazan_GS(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CBody_Khazan_GS"));
        Safe_Release(pInstance);
    }

    return pInstance;
}
void CBody_Khazan_GS::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pMotionTrailCom);

    Safe_Release(m_pBodyCom_BodyAttack);
    Safe_Release(m_pBodyCom_RangeAttack);
    Safe_Release(m_pBodyCom_Attack);
    Safe_Release(m_pBodyCom_Guard);
    Safe_Release(m_pBodyCom_Search);

    if (m_pBrutalAttack)
        Safe_Release(m_pBrutalAttack);
    if (m_pBrutalmonster)
        Safe_Release(m_pBrutalmonster);

    Safe_Release(m_pGSword);
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
