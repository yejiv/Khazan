#include "Khazan_GSword.h"
#include "Body_Khazan_GS.h"
#include "GSword_Khazan_GS.h"

#include "ClientInstance.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "CharacterVirtual.h"

#include "Khazan_Spear_ASMachine.h"

#include "Khazan_GS_Anim_Move.h"
#include "Khazan_GS_Anim_Attack.h"
#include "Khazan_GS_Anim_Guard.h"
#include "Khazan_GS_Anim_Interaction.h"
#include "Khazan_GS_Anim_Damaged.h"
#include "Khazan_GS_Anim_Fall.h"
#include "Lantern_Khazan_GS.h"
#include "Khazan_GS_Anim_Ladder.h"
#include "Camera_Compre.h"
#include "UI_HUD.h"
#include "Damage_Text.h"


#pragma region ?씠踰ㅽ듃 - ?씤踰ㅽ넗由?
#include "UI_Inven.h"
#include <Monster.h>
#include <Target_BrutalAttack.h>
#include "UI_Talk_Daphrona.h"
#pragma endregion

using WEA = CKhazan_Spear_ASMachine::WEAPON;
using CAT = CKhazan_Spear_ASMachine::CATEGORY;
using ATT = CKhazan_Spear_ASMachine::ATTACK;
using SKI = CKhazan_Spear_ASMachine::SKILL;
using MOV = CKhazan_Spear_ASMachine::MOVE;
using MOV_S = CKhazan_Spear_ASMachine::MOVESUB;
using CYC = CKhazan_Spear_ASMachine::CYCLE;
using GUA = CKhazan_Spear_ASMachine::GUARD;
using GRO = CKhazan_Spear_ASMachine::GROGGY;
using INTE = CKhazan_Spear_ASMachine::INTERACT;
using WEA_C = CKhazan_Spear_ASMachine::WEAPONCHANGE;
using HOL = CKhazan_Spear_ASMachine::HOLD;
using DAM = CKhazan_Spear_ASMachine::DAMAGED;
using CONTROL_BUTTON = CPlayer_Manager::CONTROL_BUTTON;
using GS_SKILL = CPlayerData_Manager::GSWORDSKILL;


CKhazan_GSword::CKhazan_GSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCreature{ pDevice, pContext }
{
}

CKhazan_GSword::CKhazan_GSword(const CKhazan_GSword& Prototype)
    : CCreature{ Prototype }
    , m_pClientInstance{ CClientInstance::GetInstance() }
{
    Safe_AddRef(m_pClientInstance);
}

HRESULT CKhazan_GSword::Initialize_Prototype()
{
    return S_OK;

}

HRESULT CKhazan_GSword::Initialize_Clone(void* pArg)
{
    CREATURE_DESC desc{};

    desc.fAttack = 10.f;
    desc.fMaxHP = 100.f;
    desc.fMaxStamina = 100.f;
    desc.fMoveSpeed = 10.f;
    desc.fRotationPerSec = XMConvertToRadians(180.f);
    desc.fSpeedPerSec = 1.f;

    if (FAILED(__super::Initialize_Clone(&desc)))
        return E_FAIL;

    /* ?뵆?젅?씠?뼱 ?뀑?똿  */
    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();  //?뵆?젅?씠?뼱 ?뜲?씠?꽣 ?뿰寃?

    if (m_pClientInstance->Is_CurrentSpear())
        Add_Status(SPEAR);
    else if (m_pClientInstance->Is_CurrentGSword()) 
        Add_Status(GSWORD);
    else 
        Add_Status(BAREHAND);


    //m_pClientInstance->Set_PlayerEquipment(EQUIPMENTTYPE::GSWORD, 4002);  //Test
    //Add_Status(GSWORD);

    if (FAILED(Ready_Components()))
        return E_FAIL;



    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    Ready_MotionTrailCallback();

    if (FAILED(Ready_Collision()))
        return E_FAIL;

    if (FAILED(Ready_AnimationStateMachine()))
        return E_FAIL;


#pragma region ?긽?샇 ?옉?슜 留? ?삤釉뚯젥?듃 ?씠踰ㅽ듃
    Subscribe_Events();
#pragma endregion

#ifdef _DEBUG
    Debug_Widget();
#endif // _DEBUG


    /* 湲곕낯 ?뀑?똿  */
    m_eDir.Add_Flag(DIRECTION_INFO::NONE);
    if (m_pClientInstance->Is_CurrentSpear())
    {
        //m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Stand");
        Add_Status(SPEAR);
    }
    else if (m_pClientInstance->Is_CurrentGSword())
    {
       // m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Stand");
        Add_Status(GSWORD);
    }
    else
    {
        //m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Stand");
        Add_Status(BAREHAND);
    }

    m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Teleport_End");
    m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);


    //m_iStopMoveIndexTable[0] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Walk_Stop_F_RF");
    m_iStopMoveIndexTable[0] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_Stop_F_RF");
    m_iStopMoveIndexTable[1] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Walk_Stop_F_RF");
    //m_iStopMoveIndexTable[2] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Walk_Stop_F_LF");
    m_iStopMoveIndexTable[2] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Walk_Stop_F_LF");
    m_iStopMoveIndexTable[3] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Walk_Stop_F_LF");
    //m_iStopMoveIndexTable[4] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_Stop_F_RF");
    m_iStopMoveIndexTable[4] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Run_Stop_F_RF");
    m_iStopMoveIndexTable[5] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Run_Stop_F_RF");
    //m_iStopMoveIndexTable[6] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Run_Stop_F_LF");
    m_iStopMoveIndexTable[6] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Run_Stop_F_LF");
    m_iStopMoveIndexTable[7] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Run_Stop_F_LF");
    m_iStopMoveIndexTable[8] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Sprint_Stop_F");
    m_iStopMoveIndexTable[9] = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Sprint_Stop_F");
    m_Offset_Matrix = XMMatrixRotationX(XMConvertToRadians(-90));

    m_pGSword->Set_Enble(true);
    m_strName = "Khazan";
    m_EffectTimeDelta = 0.f;

    static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 1.f, 0.f, 0.f));

    return S_OK;

}

void CKhazan_GSword::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

  

    /* Test*/

    //if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_P))
    //{
    //    //?삁?뒠媛?
    //    if (m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::HEINMACH))
    //    {
    //        m_pCharVirCom->Teleport(XMVectorSet(511.f, -11.9f, 260.f, 1.f), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);
    //        m_pTransformCom->LookAt(XMVectorSet(520.47f, -11.48f, 227.18f, 0.f));
    //    }

    //    //?궗?떎由? ?쟾
    //    if (m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::EMBARS))
    //        m_pCharVirCom->Teleport(XMVectorSet(43.f, -81.f, -47.f, 1.f), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);


    //    // 諛붿씠?띁
    //    if (m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::VIPER))
    //        m_pCharVirCom->Teleport(XMVectorSet(-30.103f, -29.9f, 185.861f, 1.f), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);
    //    
    //}
    //if (m_pGameInstance->Key_Down(DIK_NUMPAD0))
    //{
    //    m_isGhost = true;
    //}

    //if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
    //{
    //    m_isGhost = false;
    //}

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_T))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Teleport_End"));

    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT,fTimeDelta) && m_pGameInstance->Key_Down(DIK_4) )
    {
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed"));
    }
    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_5))
    {
        Remove_Status(LADDER_CLIMBING_END);
    }

    if (m_pGameInstance->Key_Pressing(DIK_RSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_0))
    {
        if (m_pAnimInteraction->Try_Teleport())
            Teleport_SFX();

    }

    //if (m_pGameInstance->Key_Down(DIK_O))
    //{
    //    m_pClientInstance->Add_SkillExp(1000.f);
    //}
    if (m_pGameInstance->Key_Down(DIK_P))
    {
        m_pPlayerData->fCulHp = 10.f;
    }

#ifdef _DEBUG
    // 스태틱 렌더 계산용
    m_pGameInstance->Jolt_Set_TargetPos_DebugRender(m_pTransformCom->Get_State(STATE::POSITION));
#endif // _DEBUG
}

void CKhazan_GSword::Update(_float fTimeDelta)
{
    //_vector vpos = m_pTransformCom->Get_State(STATE::POSITION);
    //vpos.m128_f32[1] += g_fGravity* fTimeDelta;
    //    //m_pCharVirCom->Set_Gravity(0.f);
    //if (m_pGameInstance->Key_Down(DIK_J))
    //    m_vGravity = XMVectorSet(0.f, g_fGravity, 0.f, 0.f);
    //if (m_pGameInstance->Key_Pressing(DIK_H, fTimeDelta))
    //    m_pTransformCom->Set_State(STATE::POSITION, vpos);
    //    //m_pCharVirCom->Set_Gravity(g_fGravity);
    //    //m_vGravity = XMVectorSet(0.f, 0.F, 0.f, 0.f);
    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_M))
    {
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_Grapple_Atk_01"));

    }
    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_N))
    {
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Com_Grapple_Atk_02"));

    }

    if (m_isEnableControl)
    {
        m_fTimeAcc += fTimeDelta;
        ++m_iFpsCount;
        if (m_fTimeAcc >= 1.f)
        {
            m_iFps = m_iFpsCount;
            m_iFpsCount = 0;
            m_fTimeAcc = 0.f;
        }

        if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
        {
            _float3     vPickedPos{};
            _bool isPicked = m_pGameInstance->isPicked(&vPickedPos);
            if (true == isPicked)
            {
                m_pCharVirCom->Set_Position(XMVectorSet(vPickedPos.x, vPickedPos.y, vPickedPos.z, 1.f));
                m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 1.f));
            }
        }

        Check_IsInAir(fTimeDelta);

        Check_Statue();

        m_pBody->Search_BrutalTarget(fTimeDelta);
        m_pBody->Check_BrutalAttack(fTimeDelta);

        Update_State(fTimeDelta);

        Update_Stats(fTimeDelta);


        if (Has_Status(DODGE_ENDING)) {
            if (!m_pAnimMove->IsCurrentAnimationDodge()) {
                Remove_Status(DODGE_ENDING);
              //  m_isGhost = false;
            }
        }
    }

#pragma region ?긽?샇 ?옉?슜 留? ?삤釉뚯젥?듃 ?씠踰ㅽ듃
    Event_Interact_Object(fTimeDelta);
#pragma endregion

    if (m_pCharVirCom->Get_isGround())
    {
        m_vGravity = XMVectorSet(0.f, g_fGravity, 0.f, 0.f);
    }
    else {
        m_vGravity = XMVectorSet(0.f, g_fGravity * 2.f, 0.f, 0.f);
    }

    __super::Update(fTimeDelta);

    if ((m_pGameInstance->Get_CurrentLevelID() == ENUM_CLASS(LEVEL::HEINMACH)) 
        && m_EventInteract.isInCave() == false)
    {
        m_EffectTimeDelta += fTimeDelta;
        if (m_EffectTimeDelta > 2.f)
        {
            m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Snow_Once"), m_pTransformCom->Get_State(STATE::POSITION));
            m_EffectTimeDelta = 0.f;
        }
    }
    m_pGameInstance->ListenerPosSet(m_pTransformCom->Get_State(STATE::POSITION), XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)), XMVector3Normalize(m_pTransformCom->Get_State(STATE::UP)));


	if (m_pAnimInteraction->Is_Lachryma())
	{
		m_pBody->Start_HealRimLight(8.f, _float2(5.f, 1.f), 1.5f);
		m_pGSword->Start_HealRimLight(8.f, _float2(5.f, 1.f), 1.5f);
	}
	else if (m_pAnimInteraction->Is_Heal())
	{
		m_pBody->Start_HealRimLight(1.6f, _float2(0.5f, 0.2f), 1.f);
		m_pGSword->Start_HealRimLight(1.6f, _float2(0.5f, 0.2f), 1.f);
	}
	else
	{
		m_pBody->Reset_HealRimLightFlag();
		m_pGSword->Reset_HealRimLightFlag();
	}
}

void CKhazan_GSword::Late_Update(_float fTimeDelta)
{
   if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CKhazan_GSword::Render()
{
    return S_OK;
}

void CKhazan_GSword::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::MONSTERATTACK))
        Get_HitReaction(vContactPoint);
}

void CKhazan_GSword::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CKhazan_GSword::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

void CKhazan_GSword::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
    if (Has_State(CAT::M_DIE))
        return;
    if (eHitreaction != HITREACTION::GRAB)
    {
        /* Just Guard ?슦?꽑 泥섎━*/
        if (Has_Status(JUST_GUARD))
        {
            Clear_Step3();
            m_pAnimGuard->Try_JustGuard(m_eHitNormalDir.iDirFlag);
            Remove_Status(JUST_GUARD);
            return;
        }


        /* 媛??뱶 以? 媛뺥븳?꼮諛깃났寃⑹씠 ?삤硫? ?꽦怨듬え?뀡 痍⑦븯湲? */
        if (m_pAnimGuard->Is_Guarding())
        {
            if (eHitreaction == HITREACTION::KNOCKBACK_STRONG) {
                Clear_Step3();
                m_pAnimGuard->Try_SuccessGuard(m_eHitNormalDir.iDirFlag);
            }

            return;
        }
    }

    m_pPlayerData->fCulHp -= fDamage;

    /* ?뵆?젅?씠?뼱 二쎌뿀?쓣 ?븣 ?꽭?똿?븯?뒗 踰?  */
    if ( m_pPlayerData->fCulHp <= 0.f)
    {
        /* ?엯?젰 留됯린 */
        m_pClientInstance->Set_PlayerInput(false);

        /* ?긽?깭 珥덇린?솕 */
        //Clear_CycleState();
        //Clear_SubState();
        //Clear_State();
        //m_eDir.iDirFlag = 0;
        ////m_eWorldDir.iDirFlag = 0;
        //m_iStatus = 0;
        Clear_Step0();

        /* ?긽?깭 DIE濡? ?옱?젙鍮? */
        if (Has_Status(BAREHAND)) m_iStatus = BAREHAND;
        else if (Has_Status(SPEAR)) m_iStatus = SPEAR;
        else if (Has_Status(GSWORD)) m_iStatus = GSWORD;


        Add_State(CAT::M_DIE);
        m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Die_F");
        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);

        /* UI */
        m_pGameInstance->Emit_Event< EVENT_ANNOUNCE_RESULT>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_OVER), {});
        return;
    }

    if (Has_Status(DODGE_ENDING)) {
        if (m_pAnimMove->IsCurrentAnimationDodge())
            return;
        else {
            Remove_Status(DODGE_ENDING);
          //  m_isGhost = false;
        }
    }
    if (eHitreaction != HITREACTION::GRAB)
    {
        /* Damage UI font */
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
            vPos.m128_f32[1] += 1.4f;
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::PLAYER, vPos, fDamage, { 0.f, 5.f });
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }

        /*  Decal */
        DECAL_DESC Desc{};
        Desc.fLifeTime = 8.f;
        Desc.vFadeTime = _float2(0.2f, 0.2f);
        Desc.eType = static_cast<DECALTYPE>(m_pGameInstance->Rand(0.f, static_cast<_float>(DECALTYPE::EMISSIVE)));
        Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
        Desc.isRandomTexture = true;
        _vector vDecalPos = m_pTransformCom->Get_State(STATE::POSITION);

        _float fRadianY{}, fDegreeY{};

        switch (Desc.eType)
        {
        case DECALTYPE::LINEAR:
            Desc.eType = DECALTYPE::LINEAR;
            _vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);
            _vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
            vPosition += (vLook * -1.5f);
            XMStoreFloat3(&Desc.vPosition, vPosition);
            fRadianY = atan2f(XMVectorGetX(vLook), XMVectorGetZ(vLook));
            fDegreeY = XMConvertToDegrees(fRadianY);
            Desc.vAngle = _float3(0.f, fDegreeY, 0.f);
            Desc.vScale = _float3(2.f, 1.f, 4.f);
            break;

        case DECALTYPE::CIRCLE:
            Desc.eType = DECALTYPE::CIRCLE;
            XMStoreFloat3(&Desc.vPosition, vDecalPos);
            Desc.vScale = _float3(
                m_pGameInstance->Rand(3.f, 5.f),
                1.f,
                m_pGameInstance->Rand(3.f, 5.f)
            );
            Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
            Desc.isRandomTexture = true;
            break;

        case DECALTYPE::CURVE:
            Desc.eType = DECALTYPE::CURVE;
            _float fOffset = 1.25f;
            _float fPosX = XMVectorGetX(vDecalPos);
            _float fPosZ = XMVectorGetZ(vDecalPos);
            vDecalPos = XMVectorSetX(vDecalPos, m_pGameInstance->Rand(fPosX - fOffset, fPosX + fOffset));
            vDecalPos = XMVectorSetZ(vDecalPos, m_pGameInstance->Rand(fPosZ - fOffset, fPosZ + fOffset));
            XMStoreFloat3(&Desc.vPosition, vDecalPos);
            Desc.vAngle = _float3(0.f, m_pGameInstance->Rand(0.f, 360.f), 0.f);
            Desc.vScale = _float3(2.f, 1.f, 4.f);
            Desc.vColor = _float3(0.2745f, 0.08f, 0.08f);
            Desc.isRandomTexture = true;
            break;
        }

        m_pGameInstance->Spawn_Decal(TEXT("Pool_Decal"), ENUM_CLASS(CClientInstance::GetInstance()->Get_CurrLevel()), TEXT("Layer_Decal"), Desc);

        // ?뵾寃? Vignette
        VIGNETTE_CONFIG Config{};
        Config.vColor = _float3(0.5f, 0.f, 0.f);
        Config.fPower = 3.5f;
        Config.fMinIntensity = 0.f;
        Config.fMaxIntensity = 2.f;
        Config.fDuration = 0.5f;
        Config.vFadeTime = _float2(0.25f, 0.25f);
        m_pGameInstance->Start_VignetteAnimation(Config);
    }

    /* Play Damaged Animation */
    switch (eHitreaction)
    {
    case Client::HITREACTION::NONE:
        break;
    case Client::HITREACTION::PARRY:
        break;
    case Client::HITREACTION::GROGGY:
        break;
    case Client::HITREACTION::GRAB_FINISHED:
        //m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_DamAir_F");
        //m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
        //m_pCharVirCom->End_Ladder();
        m_isGrabFinish2 = false;
        break;
    case Client::HITREACTION::BRUTAL_ATTACK:
        break;
    case Client::HITREACTION::GRAB:

        if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();
        Clear_Step1();
        m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Burrow_Predation_Kazan_DamageHold");
        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
        //m_pCharVirCom->Begin_Ladder();
        m_fGrabFinishTime.x = 0.f;
        m_isGrab = true;
        m_isGrabFinish2 = true;
        m_isViperGrab = true;

        break;
    case Client::HITREACTION::KNOCKBACK_WEAK:
        if (Has_State(CAT::M_ATTACK | CAT::M_SKILL))  break;
        //if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        //if (Has_State(CAT::M_SKILL))  m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();

        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_FAST_ATTACK | SPRINT_AGAIN_REQUEST | CHARGING_STRONG_ATTACK);
        m_eDir.iDirFlag = 0;
        // m_eWorldDir.iDirFlag = 0;
        cout << "        KNOCKBACK_WEAK    " << endl;

        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedNormal(Has_Status(GSWORD), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::KNOCKBACK_NORMAL:
        if (Has_State(CAT::M_ATTACK | CAT::M_SKILL))  break;
        //if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        //if (Has_State(CAT::M_SKILL))  m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();
        cout << "       KNOCKBACK_NORMAL     " << endl;

        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_FAST_ATTACK | SPRINT_AGAIN_REQUEST | CHARGING_STRONG_ATTACK);
        m_eDir.iDirFlag = 0;
        // m_eWorldDir.iDirFlag = 0;

        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedNormal(Has_Status(GSWORD), m_eHitNormalDir.iDirFlag);
        break;
    case Client::HITREACTION::KNOCKBACK_STRONG:
        if (Has_State(CAT::M_ATTACK | CAT::M_SKILL))  break;
        //if (Has_State(CAT::M_ATTACK)) m_pAnimAttack->Exit();
        //if (Has_State(CAT::M_SKILL))  m_pAnimAttack->Exit();
        if (Has_State(CAT::M_GUARD)) m_pAnimGuard->Exit();
        if (Has_State(CAT::M_MOVE)) m_pAnimMove->Exit();
        cout << "        KNOCKBACK_STRONG       " << endl;

        Clear_CycleState();
        Clear_SubState();
        Clear_State();
        Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_FAST_ATTACK | SPRINT_AGAIN_REQUEST | CHARGING_STRONG_ATTACK);
        m_eDir.iDirFlag = 0;
        // m_eWorldDir.iDirFlag = 0;

        Add_State(CAT::M_DAMAGED);
        m_pAnimDamaged->Force_DamagedStrong(Has_Status(GSWORD), m_eHitStrongDir.iDirFlag);
        break;
    }

}

void CKhazan_GSword::Set_Camera(CCamera_Compre* pCamera)
{
    m_pCamera = pCamera;
    Safe_AddRef(m_pCamera);
}
    
void CKhazan_GSword::Set_Position(_float4 vPos)
{
    m_pCharVirCom->Teleport(XMLoadFloat4(&vPos), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);
    if (m_pAnimInteraction->Try_Teleport()) {
        m_pBody->Get_Model()->QuitAnimationSet();
        m_pGSword->Set_Equipped(true);
        m_pClientInstance->Set_PlayerInput(true);
        Teleport_SFX();
    }

}

void CKhazan_GSword::Update_Stats(_float fTimeDelta)
{

    /*  ?뒪?깭誘몃굹 ?떎 ?뼥?뼱吏? */
    if (m_pPlayerData->fCulStamina < 0.1f && !m_pAnimAttack->Is_Attacking() && !m_pAnimAttack->Is_Skilling())
    {
        m_pPlayerData->fCulStamina = 0.f;
        if (!Has_Status(STAMINA_EXHAUSTION)) {
            Add_Status(STAMINA_EXHAUSTION);
            Clear_Step1();
            m_pBody->Get_Model()->Set_AnimationSet("Set_StaminaExhaustion");
            m_pClientInstance->Set_PlayerInput(false);     //?엯?젰 留됯린 

            m_pGameInstance->PlaySoundOnce(TEXT("vo_pc_dmg_01 (Korean(KR)).wav"), 1.2f);
        }

        if (Has_Status(STAMINA_EXHAUSTION) && (m_pBody->Get_Model()->IsFinished()/* || m_pBody->Get_Model()->Get_CurAnimIndex() != m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_StaminaExhaustion")*/))
        {
            Remove_Status(STAMINA_EXHAUSTION);
            m_pPlayerData->fCulStamina += m_pPlayerData->fStaminaRegen;      //?뒪?깭誘몃굹 ?빟媛? ?쉶蹂? 
            m_pClientInstance->Set_PlayerInput(true);       //?엯?젰 ???湲?
        }

        return;
    }

    //諛붾뵒?뿉?꽌 媛??뱶 ?궗?슜 ?떆 珥덇린?솕
    if (!m_isCanStaminaRecovery)
    {
        m_fWaitStaminaRecovery.x = 0.f;
        Remove_Status(STAMINA_RECOVERY);
        m_isCanStaminaRecovery = true;
    }

    if (Has_State(CAT::M_ATTACK | CAT::M_SKILL) || Has_SubState(MOV::MOVE_SPRINT))
        return;

    /* idle, run, walk, guard ?쁽?옱 ?뒪?깭誘몃굹媛? ?떝?븘 ?엳?뒗 ?긽?깭?씪 ?븣 ?쉶蹂듯븯湲? */
    if ((!Has_States()
        || (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_RUN | MOV::MOVE_WALK))
        || (Has_State(CAT::M_GUARD) && !Has_SubState(MOV::MOVE_SPRINT))
        || !Has_Status(DODGE_ENDING))
        && m_pPlayerData->fCulStamina < m_pPlayerData->fMaxStamina)
    {
        if (!Has_Status(STAMINA_RECOVERY))
        {
            m_fWaitStaminaRecovery.x += fTimeDelta;
            if (m_fWaitStaminaRecovery.x >= m_fWaitStaminaRecovery.y)
            {
                m_fIntervalStaminaRecovery.x = 1.f;
                m_fWaitStaminaRecovery.x = 0.f;
                Add_Status(STAMINA_RECOVERY);
            }
        }
        else
        {
            m_pPlayerData->fCulStamina += m_pPlayerData->fStaminaRegen * 3.f * fTimeDelta;
            if (m_pPlayerData->fCulStamina > m_pPlayerData->fMaxStamina)
                m_pPlayerData->fCulStamina = m_pPlayerData->fMaxStamina;
        }
    }
    else if (Has_Status(STAMINA_RECOVERY))
    {
        m_fWaitStaminaRecovery.x = 0.f;
        Remove_Status(STAMINA_RECOVERY);
    }


}

void CKhazan_GSword::Update_State(_float fTimeDelta)
{
    if (Has_State(CAT::M_DIE))
        return;

   /* Ladder Climb ?긽?깭 理쒖슦?꽑 泥댄겕 */
    if (Has_Status(LADDER_CLIMBING))
    {
        LadderClimb_Input(fTimeDelta);
        return;
    }
    else if (Has_Status(LADDER_SPRINT))
    {
        Remove_Status(LADDER_CLIMBING_END | LADDER_SPRINT);
    }


    /* Viper Grab ?긽?깭 理쒖슦?꽑 泥댄겕 */
    if (m_isGrab||m_isGrabFinish) {
        if (!ChangeGrabAnimation(fTimeDelta))
            return;
    }

    /* Fall ?긽?깭 理쒖슦?꽑 泥댄겕 */
   /* if (!m_isGrab && !m_isGrabFinish  && Fall_Input(fTimeDelta))
        return;*/
    if (!m_isViperGrab)
    {
        if (Fall_Input(fTimeDelta))
            return;
    }

    /* ?씠?쟾 ?긽?깭 ????옣*/
    m_iPrevMainState = m_iCurMainState;
    m_iPrevSubState = m_iCurSubState;
    m_ePrevDir = m_eDir.iDirFlag;
    m_iPrevCycle = m_iCycle;
 
    /* ???誘몄?? ?긽?깭 ?슦?꽑 泥댄겕 */
    _bool IsDamaged = m_pAnimDamaged->Is_Damaged();
    _bool IsGuarding = m_pAnimGuard->Is_Guarding();

    // 媛??뱶 以묒씠硫? ???誘몄?? 臾댁떆
    if (IsGuarding && IsDamaged)
    {
        // 媛??뱶 ?꽦怨? 泥섎━?뒗 ?뿬湲곗꽌
        m_pAnimDamaged->Clear_Damaged();
        IsDamaged = false;
    }

    // 媛??뱶以묒씠 ?븘?땲怨?  ???誘몄?? ?긽?깭媛? ?걹?궗?쓣 ?븣留? ?긽?깭 ?젣嫄?
    if (!IsGuarding && !IsDamaged && (m_iPrevMainState & CAT::M_DAMAGED))
    {
        Clear_Step0();
        Remove_State(CAT::M_DAMAGED);
    }

    // 媛??뱶以묒씠 ?븘?땲怨?  ???誘몄?? 諛쏅뒗 以묒씠硫? ?엯?젰 諛? ?떎瑜? 紐⑤뱺 泥섎━ 李⑤떒
    if (!IsGuarding && IsDamaged)
    {
        // ???誘몄?? 濡쒖쭅留? ?떎?뻾
        if (Has_State(CAT::M_DAMAGED))
        {
            _bool isEnter = (m_iCurMainState != m_iPrevMainState);
            if (isEnter) m_pAnimDamaged->Enter();
            m_pAnimDamaged->Continue(fTimeDelta);
        }
        return;  // ???誘몄?? 以묒뿉?뒗 ?떎瑜? 紐⑤뱺 泥섎━ 李⑤떒
    }

    /* ?떣吏?留? ?뵲濡? 泥댄겕 */
    if (Has_Status(DODGING) && m_pBody->Get_Model()->Check_MinAnimationTime())
    {
        Remove_Status(DODGING);
    }

    /* ?궎 ?엯?젰 */
    if (m_pClientInstance->Get_PlayerInput())
    {

        /* ?씫?삩?긽?깭 泥댄겕  */
        Update_LockOn();

        /* 諛⑺뼢 寃곗젙 */
        Check_KeyInput_Direction(fTimeDelta);

        Interaction_Input(fTimeDelta);
        /* 怨듦꺽?씠?굹 媛??뱶瑜? 留됱븘?빞?븯?뒗 ?긽?샇?옉?슜 泥섎━  */
        if (!Has_Status(BLOCK_ATK_SKILL_GUARD))
        {
            Guard_Input(fTimeDelta);
            Skill_Input(fTimeDelta);
            Attack_Input(fTimeDelta);
        }

        // 怨듦꺽 以묒씪 ?븣?뒗 Move_Input?쓣 ?셿?쟾?엳 李⑤떒
        if (!Has_State(CAT::M_ATTACK | CAT::M_GUARD | CAT::M_SKILL) && !m_pAnimAttack->Is_Attacking() && m_pAnimAttack && !m_pAnimInteraction->Is_Heal())
            Move_Input(fTimeDelta);

        if (Has_State(CAT::M_ATTACK | CAT::M_SKILL))
        {
            if (Has_State(CAT::M_MOVE))
                Clear_Step2();
            if (Has_Status(LOCKON)) 
                LockOn_Rotation(fTimeDelta);
        }

        if (Has_Status(GUARD_ROTATION_REQUEST)) Setting_Guard_Rotation();
        if (Has_Status(GUARD_ROTATION)) Guard_Rotation(fTimeDelta);

    }

    /*  ?긽?깭 ?쟾?솚 ?뿬遺?*/
    _bool isEnter = (m_iCurMainState != m_iPrevMainState) || (m_iCurSubState != m_iPrevSubState);
    _bool isContinue = (m_iCurMainState == m_iPrevMainState) && (m_iCurSubState == m_iPrevSubState);

    /* 怨듦꺽 ?긽?깭媛? ?븘?땺 ?븣 怨듦꺽 肄쒕━?뜑 ?걚湲?  */
    if (!m_pAnimAttack->Is_Attacking() && !m_pAnimFall->Is_FallAttacking())
        m_pBody->AllAttackCollisionActive_Off();


    /* (move , idle ?븷?땲硫붿씠?뀡 ?옱?깮 ?떆?룄 */
    if (!Has_State(CAT::M_FALL | CAT::M_DAMAGED) && !m_pAnimDamaged->Is_Damaged())
        Change_MoveIdle(fTimeDelta);

    /* ?떎?젣 ?씠?룞媛? 二쇨린 */
    if (Has_State(CAT::M_MOVE | CAT::M_GUARD)
         && !Has_State(CAT::M_ATTACK | CAT::M_SKILL | CAT::M_DAMAGED | CAT::M_INTERACT)
         && !Has_SubState(MOV::MOVE_DODGE) 
         && !m_pAnimMove->IsDodge()
         && !m_pAnimAttack->Is_Attacking()
         && m_pClientInstance->Get_PlayerInput())
    {
        Apply_PlayerMovement(fTimeDelta);
    }

    /* Exit ?떎?뻾 */
    if (isEnter)
        ExecuteAnimationExit();

    /* ?긽?깭蹂? 濡쒖쭅 ?떎?뻾 */
    if (Has_State(CAT::M_DIE)) {
        Update_Die(fTimeDelta);
    }
    else if (Has_State(CAT::ORDER2))
    {
        if (Has_State(CAT::M_FALL))
        {
            if (isEnter) m_pAnimFall->Enter();
            m_pAnimFall->Continue(fTimeDelta);

            if (!m_pAnimFall->Is_Falling())
            {
                Remove_State(CAT::M_FALL);
                // Clear_SubState();
            }
        }


        if (Has_State(CAT::M_GUARD))
        {
            if (isEnter) m_pAnimGuard->Enter();
            m_pAnimGuard->Continue(fTimeDelta);

            if (!m_pAnimGuard->Is_Guarding())
            {
                Remove_State(CAT::M_GUARD | CAT::M_MOVE);
                Clear_SubState();
            }
        }

        if (Has_State(CAT::M_DAMAGED))
        {
            if (isEnter) m_pAnimDamaged->Enter();
            m_pAnimDamaged->Continue(fTimeDelta);

            if (!m_pAnimDamaged->Is_Damaged())
            {
                Remove_State(CAT::M_DAMAGED);
                Clear_SubState();
            }
        }
    }
    else if (Has_State(CAT::M_SKILL))
    {
        if (isEnter) m_pAnimAttack->Enter();
        m_pAnimAttack->Continue(fTimeDelta);

        if (!m_pAnimAttack->Is_Skilling())
        {
            Remove_State(CAT::M_SKILL);
            Clear_SubState();
            m_pAnimAttack->Clear_Skill();

            if (m_eDir.iDirFlag > 0)
            {
                Clear_Step2();
                Add_State(CAT::M_MOVE);
                Add_SubState(MOV::MOVE_RUN);
                Add_CycleState(CYC::CYCLE_START);
            }
        }
    }

    else if (Has_State(CAT::ORDER5))
    {
        if (Has_State(CAT::M_ATTACK))
        {
            if (isEnter) m_pAnimAttack->Enter();
            m_pAnimAttack->Continue(fTimeDelta);

            if (!m_pAnimAttack->Is_Attacking())
            {
                Remove_State(CAT::M_ATTACK);
                Remove_Status(CHARGING_FAST_ATTACK | CHARGING_STRONG_ATTACK);
                Clear_SubState();

                if (m_eDir.iDirFlag > 0)
                {
                    Clear_Step2();
                    Add_State(CAT::M_MOVE);
                    Add_SubState(MOV::MOVE_RUN);
                    Add_CycleState(CYC::CYCLE_START);
                }
            }
        }

        if (!Has_State(CAT::M_ATTACK | CAT::M_SKILL) && Has_State(CAT::M_MOVE))
        {
            if (isEnter) m_pAnimMove->Enter();
            /*if (isEnter || isContinue) */m_pAnimMove->Continue(fTimeDelta);


        }
    }
    else if (Has_State(CAT::M_INTERACT))
    {

    }
    else if (Has_State(CAT::M_WEAPON_CHANGE))
    {

    }
    else
    {

    }
}

_bool CKhazan_GSword::Fall_Input(_float fTimeDelta)
{
    if (Has_Status(FALLING | PRE_LAND | FALLING_ATTACK))
    {
        // ?굺?븯 ?떆?옉
        if (Has_Status(FALLING) && !Has_Status(PRE_LAND))
        {
            // ?굺?븯怨듦꺽 ?엯?젰 泥댄겕 (?굺?븯 以묒뿉留?)
            if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB) &&
                !Has_Status(FALLING_ATTACK))
            {
                m_pAnimFall->Force_StartFallAttack();
                Add_Status(FALLING_ATTACK);
                Remove_Status(FALLING);
            }
            // ?씪諛? ?굺?븯 ?븷?땲硫붿씠?뀡
            else if (!m_pAnimFall->Is_Falling() && !m_pAnimFall->Is_FallAttacking())
            {
                m_pAnimFall->Force_StartFall();
                Add_State(CAT::M_FALL);
            }
        }
        // 李⑹?? 吏곸쟾
        else if (Has_Status(PRE_LAND))
        {
            if (Has_Status(FALLING_ATTACK))
            {
                m_pAnimFall->Force_AttackLanding();
            }
            else
            {
                m_pAnimFall->Force_Landing();
            }
        }

        // ?굺?븯/李⑹?? ?븷?땲硫붿씠?뀡 怨꾩냽 ?떎?뻾
        m_pAnimFall->Continue(fTimeDelta);

        // 李⑹?? ?븷?땲硫붿씠?뀡 ?셿猷? 泥댄겕
        if (Has_Status(PRE_LAND) && m_pBody->Get_Model()->Check_MinAnimationTime())
        {
            Remove_Status(FALLING | FALLING_ATTACK | PRE_LAND);
            Remove_State(CAT::M_FALL);
            m_pAnimFall->Exit();
        }

        // ?굺?븯 以묒뿉?뒗 ?떎瑜? ?엯?젰 李⑤떒
        return true;
    }
    return false;
}

void CKhazan_GSword::Move_Input(_float fTimeDelta)
{
    /*  怨듦꺽以? ?떣吏??뒗 ?삁?쇅 泥섎━ (怨듦꺽以? 鍮좊Ⅸ ????씠諛띿뿉 ?떣吏??븷 ?닔 ?엳?룄濡?) */
    if (m_pGameInstance->Key_Down(DIK_SPACE) &&/* m_pBody->Is_SpearFullExtension() &&*/ Has_State(CAT::M_ATTACK | CAT::M_SKILL))  // zzzz
    {
        Remove_State(CAT::M_ATTACK | CAT::M_SKILL);
        Remove_Status(CHARGING_FAST_ATTACK | CHARGING_STRONG_ATTACK);
        Clear_SubState();
        Add_State(CAT::M_MOVE);
        Add_SubState(MOV::MOVE_DODGE);
        Add_Status(DODGING);
    }

    // 怨듦꺽/媛??뱶 以묒씪 ?븣?뒗 ?셿?쟾?엳 由ы꽩
    if (m_pAnimAttack->Is_Attacking() || m_pAnimGuard->Is_Guarding())
    {
        // 怨듦꺽 以묒씪 ?븣?뒗 Move ?긽?깭?룄 ?젣嫄?
        if (Has_State(CAT::M_MOVE))
        {
            Remove_State(CAT::M_MOVE);
            Clear_SubState();
            AllClear_CycleState();
        }
        return;
    }

    _bool isPrevMove = Has_State(CAT::M_MOVE);
    _bool isPrevDodge = Has_SubState(MOV::MOVE_DODGE);

    //Dodge 醫낅즺 泥댄겕
    if (isPrevDodge)
    {
        Remove_SubState(MOV::MOVE_DODGE);
        Remove_Status(CHARGING_SPRINT);

        //// ?떣吏? ?긽?깭 ?쑀吏? - ?븷?땲硫붿씠?뀡?씠 吏꾪뻾 以묒씠硫? DODGING ?긽?깭 異붽??
        //if (!m_pAnimMove->IsStopMoveAnimantionFinished())
        //{
        //    Add_Status(DODGING);  // ?떣吏? 以? ?긽?깭 ?뵆?옒洹? ?꽕?젙
        //}

        // Dodge ?븷?땲硫붿씠?뀡?씠 ?걹?궗?뒗吏? ?솗?씤
        if (m_pAnimMove->IsStopMoveAnimantionFinished()&& !m_pAnimMove->IsDodge()&& !Has_Status(DODGE_ENDING))
        {
            //Remove_SubState(MOV::MOVE_DODGE);
           // Remove_Status(CHARGING_SPRINT);
            // 諛⑺뼢?궎留? ?닃?젮?엳?쑝硫? 利됱떆 Run?쑝濡? ?쟾?솚
            if (!m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta) && m_eDir.iDirFlag > 0)
            {
                Add_State(CAT::M_MOVE);
                Add_SubState(MOV::MOVE_RUN);
                Add_CycleState(CYC::CYCLE_START);

                CKhazan_GS_Anim_Move::GS_MOVEINFO info;

                if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
                else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
                else if (Has_Status(GSWORD))info.iWeapon = GSWORD;

                info.isLockOn = Has_Status(LOCKON);
                info.iState = m_iCurSubState;
                info.iCycle = m_iCycle;
                info.eDir = m_eDir;

                m_pAnimMove->Try_ChangeAnimation(info);
            }
            return;  //Dodge 醫낅즺 泥섎━ ?썑 諛붾줈 由ы꽩
        }
        else
        {
            //Dodge ?븷?땲硫붿씠?뀡 ?옱?깮 以묒씠硫? ?엯?젰 臾댁떆, ?긽?깭?뒗 ?쑀吏?
            return;
        }
    }

    //Clear_State();
    if (Has_CycleState(CYC::CYCLE_END))
        Remove_State(CAT::M_MOVE);

    AllClear_CycleState();


    if (Has_Status(BACK_DODGE))
    {
        Remove_Status(BACK_DODGE);
        m_eDir.Delete_Flag(DIR::B);
    }


    /* 諛⑺뼢?씠 ?뱾?뼱?삤硫? Move On */
    if (m_eDir.iDirFlag) {
        Add_State(CAT::M_MOVE);
        if (isPrevMove)    Add_CycleState(CYC::CYCLE_LOOP);
        else Add_CycleState(CYC::CYCLE_START);
    }
    else {
        if (isPrevMove) {
            Add_CycleState(CYC::CYCLE_END);
        }
    }


    if (Has_State(CAT::M_MOVE))
    {
        Clear_SubState();
        _bool isSpaceHandled = false;

        /*  Sprint , Dodge */
        if (m_pGameInstance->Key_Down(DIK_SPACE))
        {
            ///* ?떣吏? 以묐났 諛⑹??*/
            if (m_pAnimMove->IsDodge())
                return;

            m_fSprintTime = 0.f;
            Add_Status(CHARGING_SPRINT | DODGING );
            Add_SubState(MOV::MOVE_DODGE);               
            /* 諛? ?떣吏??씪硫? */
            if (m_eDir.Check_Flag(DIR::B))
            {
                Add_CycleState(CYC::CYCLE_END);
                Add_Status(BACK_DODGE );
            }
            isSpaceHandled = true;

        }
        /* ?뒪?럹?씠?뒪 ?뼹怨? 諛⑺뼢?궎瑜? ?늻瑜닿퀬 ?엳?떎?뒗 ?삁?빟?씠 嫄몃젮?엳?쓣 ?븣 ?떎?떆 ?뒪?럹?씠?뒪瑜? ?늻瑜? 寃쎌슦*/
        else if (Has_State(SPRINT_AGAIN_REQUEST) && m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta))
        {
            Add_SubState(MOV::MOVE_SPRINT);
            Remove_SubState(MOV::MOVE_RUN);
            Remove_Status(SPRINT_AGAIN_REQUEST);
            isSpaceHandled = true;
        }
        else if (m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta, INPUT_TYPE::GAMEPLAY, &m_fSprintTime))
        {
            if (Has_Status(CHARGING_SPRINT))
            {

                // Dodge ?븷?땲硫붿씠?뀡 吏꾪뻾?룄 泥댄겕
                _float trackPos = *m_pBody->Get_Model()->Get_CurTrackPosition();
                _float duration = m_pBody->Get_Model()->Get_CurDuration();
                if (m_pBody->Get_Model()->Check_MinAnimationTime() || (trackPos / duration) >= 0.3f)
                {
                    Add_SubState(MOV::MOVE_SPRINT);
                    Remove_Status(CHARGING_SPRINT);  // Sprint ?쟾?솚 ?셿猷?
                }
                else
                {
                    Add_SubState(MOV::MOVE_DODGE);  // ?븘吏? Dodge ?쑀吏?
                    isSpaceHandled = true;
                }
            }
            else
            {
                // ?씠誘? Sprint ?긽?깭 - ?쑀吏?
                Add_SubState(MOV::MOVE_SPRINT);
                isSpaceHandled = true;
            }
        }
        // Space瑜? ?뿄
        else if (m_pGameInstance->Key_Up(DIK_SPACE))
        {
            m_fSprintTime = 0.f;
            Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

            if (m_eDir.iDirFlag > 0)
            {
                Remove_SubState(MOV::MOVE_SPRINT);
                Add_SubState(MOV::MOVE_RUN);
                Add_Status(SPRINT_AGAIN_REQUEST);
            }
            // Sprint 醫낅즺 ?떊?샇 (?떎?쓬 ?봽?젅?엫?뿉 END ?븷?땲硫붿씠?뀡 ?옱?깮)
            if (Has_SubState(MOV::MOVE_SPRINT))
            {
                Add_CycleState(CYC::CYCLE_END);
                Add_SubState(MOV::MOVE_SPRINT);
                isSpaceHandled = true;
            }
        }

        /* Walk,  Run */
        if (!isSpaceHandled && !Has_State(CAT::M_ATTACK | CAT::M_SKILL))
        {
            if (m_pGameInstance->Key_Pressing(DIK_LALT, fTimeDelta))  Add_SubState(MOV::MOVE_WALK);
            else
                Add_SubState(MOV::MOVE_RUN);

        }

    }
    else
    {
        // ?뮘濡? ?쉶?뵾
        if (m_pGameInstance->Key_Down(DIK_SPACE))
        {
            /* ?떣吏? 以묐났 諛⑹??*/
            if (isPrevDodge)
                return;

            Add_State(CAT::M_MOVE);
            Add_SubState(MOV::MOVE_DODGE);
            Add_CycleState(CYC::CYCLE_END);
            m_eDir.Clear_Flag();
            m_eDir.Add_Flag(DIR::B);  // ?뮘濡? ?쉶?뵾
            m_fSprintTime = 0.f;
            Add_Status(BACK_DODGE );
            Remove_Status(SPRINT_AGAIN_REQUEST | CHARGING_SPRINT);
        }
        else
        {
            // ?씠?룞?븯吏? ?븡?쓣 ?븣?뒗 SubState 珥덇린?솕
            Clear_SubState();
            m_fSprintTime = 0.f;
            Remove_Status(CHARGING_SPRINT);
        }
    }

}

_bool CKhazan_GSword::Skill_Input(_float fTimeDelta)
{
    // ?뒪?궗?씠 ?걹?궗?뒗吏? 泥댄겕
    if (Has_State(CAT::M_SKILL) && m_pAnimAttack && !m_pAnimAttack->Is_Skilling())
    {
       // m_pClientInstance->Set_UsedSkill(m_iCurSkillIndex, false); //?궡遺??뿉?꽌 ?빐寃? 
        Clear_Step1();
        return false;
    }

    ///* ?닾吏?, ?뒪?깭誘몃굹  ?뒪?꺈 ?솗?씤 - attack ?븞?뿉?꽌 寃??궗 */

    if (m_pGameInstance->Key_Down(DIK_Q))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::Q);
        if (m_iCurSkillIndex == 0) return false;

        if (m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        else if (m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        return false;

    }
    if (m_pGameInstance->Key_Down(DIK_E))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::E);
        if (m_iCurSkillIndex == 0) return false;


        if (m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        else if (m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        return false;

    }
    if (m_pGameInstance->Key_Down(DIK_R))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::R);
        if (m_iCurSkillIndex == 0) return false;

        if (m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        else if (m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        return false;

    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_F))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_F);
        if (m_iCurSkillIndex == 0) return false;


        if (m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        else if (m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        return false;

    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_LB);
        if (m_iCurSkillIndex == 0) return false;


        if (m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        else if (m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        return false;

    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    {
        m_iCurSkillIndex = m_pClientInstance->Get_ButtonSkill(CONTROL_BUTTON::CTRL_RB);
        if (m_iCurSkillIndex == 0) return false;


        if (m_pAnimAttack->Try_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        else if (m_pAnimAttack->Reserve_SkillAttack(m_iCurSkillIndex)) {
            Add_State(CAT::M_SKILL);
            return true;
        }
        return false;
    }

    return false;
}

_bool CKhazan_GSword::Attack_Input(_float fTimeDelta)
{

    /* ?뒪?깭誘몃굹 ?솗?씤 */
    if (m_pPlayerData->fCulStamina <= 0.1f) {
        //Clear_Step0();
        return false;
    }

    /* ?빞留? 遺?瑜댄깉 */
    if (m_pGameInstance->Key_Down(DIK_Y))
    {
        Add_Status(BRUTAL_BEGIN | BRUTAL_READY);
        if (m_pAnimAttack->Try_GrappleAttack())
        {
            Clear_Step2();
            Add_State(CAT::M_ATTACK);
            Add_SubState(ATT::ATK_GRAPPLE);
            Add_Status(BRUTAL_SUCCESS);
            m_eHitReaction = ENUM_CLASS(HITREACTION::BRUTAL_ATTACK);
            return true;
        }
    }


    /* 媛뺥븳 怨듦꺽 1???  + 李⑥쭠 + ?뒪?궗: ?닲?넻?걡湲곕뱾 ?뙋蹂? */
    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    {
        m_fChargingStrongAttackTime = 0.f;
        Remove_State(CAT::M_MOVE);
        Remove_Status(CHARGING_STRONG_ATTACK);
    }
    else if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB))
    {
        if (!Has_Status(CHARGING_STRONG_ATTACK))
        {
            m_fChargingStrongAttackTime += fTimeDelta;

            if (m_fChargingStrongAttackTime >= m_fChargingStartIntervalTime)
            {

                /* ?삁?빟以묒씤 怨듦꺽 ???湲곌?? ?엳?쑝硫? true */
                if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Reserve())
                    return true;

                /* ?뒪?궗 : ?닲?넻?걡湲? : ?꽑?삁 */
                if (m_pAnimAttack->Is_PossibleBreathtaking_BloodShed())
                {
                    _bool isTryAttack = false; 
                    if (m_pAnimAttack->Try_SkillAttack(GS_SKILL::BREATHTAKING_BLOODSHED)) isTryAttack = true;
                    else if (m_pAnimAttack->Reserve_SkillAttack(GS_SKILL::BREATHTAKING_BLOODSHED)) isTryAttack = true;
                    
                    if (isTryAttack)
                    {
                        Remove_State(CAT::M_MOVE);
                        AllClear_CycleState();
                        Clear_SubState();
                        Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

                        Add_Status(CHARGING_STRONG_ATTACK);
                       // Add_SubState(ATT::ATK_CHARGE);
                        Add_State(CAT::M_ATTACK);

                        m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
                        //Add_State(CAT::M_SKILL);
                        return true;
                    }
                }

     
                // 怨듦꺽?씠 ?걹?궗?뒗吏? 泥댄겕
                if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Is_Attacking())
                {
                    Clear_Step0();
                    return false;
                }
                /* 肄ㅻ낫 怨듦꺽 以묒씤吏? 泥댄겕  */
                if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Can_NextCombo() && !Has_Status(CHARGING_FAST_ATTACK))
                    return false;

                /* 媛뺥븳 李⑥쭠 怨듦꺽 */
                if (m_pAnimAttack->Try_ChageStrongAttack()) {
                    Remove_State(CAT::M_MOVE);
                    AllClear_CycleState();
                    Clear_SubState();
                    Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

                    Add_Status(CHARGING_STRONG_ATTACK);
                    Add_SubState(ATT::ATK_CHARGE);
                    Add_State(CAT::M_ATTACK);

                    m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
                    return true;
                }
            }
        }

    }
    else if (!Has_Status(CHARGING_STRONG_ATTACK) && m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::RB))
    {
        _bool wasCharging = Has_Status(CHARGING_STRONG_ATTACK);
        Remove_Status(CHARGING_STRONG_ATTACK);

        // 李⑥쭠 ?븞?뻽?쑝硫? ?씪諛? 媛뺢났寃?
        if (!wasCharging && m_fChargingFastAttackTime < m_fChargingStartIntervalTime)
        {

            /* ?삁?빟以묒씤 怨듦꺽 ???湲곌?? ?엳?쑝硫? true */
            if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Reserve())
                return true;

            _bool isTryAttack = false;
            /* ?뒪?궗 : ?닲?넻?걡湲? */
            if (m_pAnimAttack->Is_PossibleBreathtaking())
            {
                if (m_pAnimAttack->Try_SkillAttack(GS_SKILL::BREATHTAKING)) isTryAttack = true;
                else if (m_pAnimAttack->Reserve_SkillAttack(GS_SKILL::BREATHTAKING)) isTryAttack = true;
            }
            /* ?뒪?궗 : ?닲?넻?걡湲? : ?깭?룞 */
            if (m_pAnimAttack->Is_PossibleBreathtaking_Embryonic())
            {
                if (m_pAnimAttack->Try_SkillAttack(GS_SKILL::BREATHTAKING_EMBRYONIC)) isTryAttack = true;
                else if (m_pAnimAttack->Reserve_SkillAttack(GS_SKILL::BREATHTAKING_EMBRYONIC))isTryAttack = true;
            }

            if (isTryAttack)
            {
                Remove_State(CAT::M_MOVE);
                Clear_SubState();
                AllClear_CycleState();
                Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

                Add_State(CAT::M_ATTACK);
                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_NORMAL);
                //Add_State(CAT::M_SKILL);
                return true;
            }

            // 怨듦꺽?씠 ?걹?궗?뒗吏? 泥댄겕
            if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Is_Attacking())
            {
                Clear_Step0();
                return false;
            }
            /* 肄ㅻ낫 怨듦꺽 以묒씤吏? 泥댄겕  */
            if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Can_NextCombo() && !Has_Status(CHARGING_FAST_ATTACK))
                return false;

            /* ?씪諛? 媛뺢났寃?  */
            if (!Has_Status(CHARGING_STRONG_ATTACK) && m_pAnimAttack->Try_StrongAttack())
            {

                Remove_State(CAT::M_MOVE);
                Clear_SubState();
                AllClear_CycleState();
                Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

                Add_SubState(ATT::ATK_STRONG);
                Add_State(CAT::M_ATTACK);

                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_NORMAL);
                return true;
            }
        }
        else if (wasCharging)
        {
            // 怨듦꺽 ?긽?깭 ?빐?젣
            if (m_pAnimAttack)
            {
                m_pAnimAttack->Exit();
            }
            Remove_State(CAT::M_ATTACK);
            Clear_SubState();
        }
    }


    /* ?삁?빟以묒씤 怨듦꺽 ???湲곌?? ?엳?쑝硫? true */
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Reserve())
        return true;
    // 怨듦꺽?씠 ?걹?궗?뒗吏? 泥댄겕
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Is_Attacking())
    {
        Clear_Step0();
        return false;
    }
    /* 肄ㅻ낫 怨듦꺽 以묒씤吏? 泥댄겕  */
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack && !m_pAnimAttack->Can_NextCombo() && !Has_Status(CHARGING_FAST_ATTACK))
        return false;




    /*  釉뚮（?깉 怨듦꺽.*/
    if (Has_Status(BRUTAL_READY) && m_pGameInstance->Key_Down(DIK_T))
    {
        if (m_pAnimAttack->Try_GrappleAttack())
        {
            Clear_Step2();
            Add_State(CAT::M_ATTACK);
            Add_SubState(ATT::ATK_GRAPPLE);
            Add_Status(BRUTAL_SUCCESS);
            m_eHitReaction = ENUM_CLASS(HITREACTION::BRUTAL_ATTACK);
        }
    }


    /* dodge 怨듦꺽 */
   // if ((m_iPrevMainState & CAT::M_MOVE) && (m_iPrevSubState & MOV::MOVE_DODGE) && (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB)))
    if (Has_Status(DODGING) && (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB)))
    {
        _uint dodgeDir = m_eDir.iDirFlag > 0 ? m_eDir.iDirFlag : m_ePrevDir;

        if (m_pAnimAttack->Try_DodgeAttack(dodgeDir))
        {
            Clear_Step2();
            Add_State(CAT::M_ATTACK);
            Add_SubState(ATT::ATK_DODGEATK);
            m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_NORMAL);

            Remove_Status(DODGING);

            return true;
        }
    }

    /* Sprint 怨듦꺽  - ?븷?땲硫붿씠?뀡 ?옄泥닿?? ?씠?긽?븿. ?븳 ?봽?젅?엫 ?닚媛꾩뿉 ?닚媛꾩씠?룞?븿 - ?궗?슜 xxx */
    //if ((m_iPrevMainState & CAT::M_MOVE) && (m_iPrevSubState & MOV::MOVE_SPRINT) && (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB) || m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB)))
    //{
    //    if (m_pAnimAttack->Try_SprintAttack())
    //    {
    //        Clear_Step2();
    //        Add_State(CAT::M_ATTACK);
    //        Add_SubState(ATT::ATK_SPRINTATK);
    //        m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
    //        return true;
    //    }
    //}

    /* 鍮좊Ⅸ 怨듦꺽 3?뿰??? + 李⑥쭠 */
    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
    {
        m_fChargingFastAttackTime = 0.f;
        Remove_State(CAT::M_MOVE);
        Remove_Status(CHARGING_FAST_ATTACK);
    }
    else if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
    {
        if (!Has_Status(CHARGING_FAST_ATTACK))
        {
            m_fChargingFastAttackTime += fTimeDelta;

            if (m_fChargingFastAttackTime >= m_fChargingStartIntervalTime)
            {
                /* 鍮좊Ⅸ 李⑥쭠 怨듦꺽 */
                if (m_pAnimAttack->Try_ChageFastAttack()) {
                    Remove_State(CAT::M_MOVE);
                    AllClear_CycleState();
                    Clear_SubState();
                    Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

                    Add_Status(CHARGING_FAST_ATTACK);
                    Add_SubState(ATT::ATK_CHARGE);
                    Add_State(CAT::M_ATTACK);

                    m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG);
                    return true;
                }
            }
        }

    }
    else if (!Has_Status(CHARGING_FAST_ATTACK) && m_pGameInstance->Mouse_Up(MOUSEKEYSTATE::LB))
    {
        _bool wasCharging = Has_Status(CHARGING_FAST_ATTACK);
        Remove_Status(CHARGING_FAST_ATTACK);

        // 李⑥쭠 ?븞?뻽?쑝硫? ?씪諛? 怨듦꺽
        if (!wasCharging && m_fChargingFastAttackTime < m_fChargingStartIntervalTime)
        {
            if (m_pAnimAttack->Try_FastAttack())
            {
                Remove_State(CAT::M_MOVE);
                Clear_SubState();
                AllClear_CycleState();
                Remove_Status(CHARGING_SPRINT | SPRINT_AGAIN_REQUEST);

                Add_SubState(ATT::ATK_FAST);
                Add_State(CAT::M_ATTACK);

                m_eHitReaction = ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK);
                return true;
            }
        }
        else if (wasCharging)
        {
            // 怨듦꺽 ?긽?깭 ?빐?젣
            if (m_pAnimAttack)
            {
                m_pAnimAttack->Exit();
            }
            Remove_State(CAT::M_ATTACK);
            Clear_SubState();
        }
    }

    return false;
}

_bool CKhazan_GSword::Guard_Input(_float fTimeDelta)
{

    /* 媛??뱶 醫낅즺*/
    if (/*Has_State(CAT::M_GUARD) && */m_pGameInstance->Key_Up(DIK_LSHIFT))
    {
        m_pAnimGuard->Play_FinishGuard();
        Clear_Step1();

        return true;
    }

    if (!Has_State(CAT::M_GUARD) && m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta))
    {
        _float fAddDesceaseTime = 0.f;
        if (Has_State(CAT::M_ATTACK)) fAddDesceaseTime = 3.f;
        if (Has_State(CAT::M_ATTACK)) fAddDesceaseTime = 5.f;
        if (Has_State(CAT::M_DAMAGED)) fAddDesceaseTime = 11.f;
        if (Has_Status(DODGE_ENDING)) fAddDesceaseTime = 4.f;

        /* 洹몃깷 媛??뱶 */
        if (m_pAnimGuard->Try_Guard(fAddDesceaseTime))
        {
            Clear_Step3();
            Add_State(CAT::M_GUARD);
            return true;
        }

    }

    //   /* 媛??뱶 ?꽦怨?  */
       //else if (Has_State(CAT::M_GUARD)&&) //todo 議곌굔 二쇨린
       //{
       //	if (m_pAnimGuard->Try_SuccessGuard(HITDIR))
       //	{
       //		return true;
       //	}
       //}

       /* 媛??뱶以? ?씠?룞 - 諛⑺뼢 ?엯?젰?씠 ?엳?쓣 ?븣 */
    if (Has_State(CAT::M_GUARD) && m_eDir.iDirFlag > 0)
    {
        // 諛⑺뼢?씠 諛붾?뚯뿀嫄곕굹, ?썙?궧媛??뱶媛? ?븘?땺 ?븣
        if (!m_pAnimGuard->Is_WalkGuarding() || m_eDir.iDirFlag != m_ePrevDir)
        {
            if (m_pAnimGuard->Try_WalkGuard(m_eDir.iDirFlag))
            {
                return true;
            }
        }
    }
    /* ?썙?궧媛??뱶以묒뿉 諛⑺뼢?궎 ?뼹硫?  */
    else if (Has_State(CAT::M_GUARD) && m_eDir.iDirFlag == 0 && m_pAnimGuard->Is_WalkGuarding())
    {
        //Remove_State(CAT::M_MOVE );
        //Remove_SubState(MOV::MOVE_WALK);
        m_pAnimGuard->Try_Guard(0.f);  // ?젙吏? 媛??뱶濡? ?쟾?솚
        return true;
    }




    return false;
}

_bool CKhazan_GSword::Interaction_Input(_float fTimeDelta)
{
    _matrix mat_arm = XMLoadFloat4x4(m_pBody->Get_BoneMatrix("Muscle_L_ForeTwist1"));
    _matrix mat_hand = XMLoadFloat4x4(m_pBody->Get_BoneMatrix("FX_L_Hand_02"));

    //?씪?겕由щ쭏 
    if (m_pGameInstance->Key_Down(DIK_1))
    {
        if (m_pPlayerData->fCulHp < m_pPlayerData->fMaxHp)
            if (m_pAnimInteraction->Try_Heal())
            {
                m_FXIdx[FX_LACRIMA] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma_Arm"), mat_arm, (m_Offset_Matrix * mat_arm * m_pTransformCom->Get_WorldMatrix()).r[3]);
                m_FXIdx[FX_LACRIMA_HAND] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma"), (m_Offset_Matrix * mat_hand * m_pTransformCom->Get_WorldMatrix()).r[3]);
                m_iHealIndex = 1;
            }
    }

    //?옖?꽩
    if (m_pGameInstance->Key_Down(DIK_2)) {
        _bool isEquip = !m_pLantern->Get_Equipped();
        if (m_pAnimInteraction->Try_Lantern(isEquip))
            m_pLantern->Set_Equipped(isEquip);
    }

    if (m_pGameInstance->Key_Down(DIK_3))
    {
        if (m_pAnimInteraction->Try_Heal())
        {
            m_FXIdx[FX_LACRIMA] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma_Arm"), mat_arm, (m_Offset_Matrix * mat_arm * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_FXIdx[FX_LACRIMA_HAND] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma"), (m_Offset_Matrix * mat_hand * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_iHealIndex = 3;
        }
    }
    if (m_pGameInstance->Key_Down(DIK_4))
    {
        if (m_pAnimInteraction->Try_Heal())
        {
            m_FXIdx[FX_LACRIMA] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma_Arm"), mat_arm, (m_Offset_Matrix * mat_arm * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_FXIdx[FX_LACRIMA_HAND] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma"), (m_Offset_Matrix * mat_hand * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_iHealIndex = 4;
        }
    }
    if (m_pGameInstance->Key_Down(DIK_5))
    {
        if (m_pAnimInteraction->Try_Heal())
        {
            m_FXIdx[FX_LACRIMA] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma_Arm"), mat_arm, (m_Offset_Matrix * mat_arm * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_FXIdx[FX_LACRIMA_HAND] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma"), (m_Offset_Matrix * mat_hand * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_iHealIndex = 5;
        }
    }
    if (m_pGameInstance->Key_Down(DIK_6))
    {
        if (m_pAnimInteraction->Try_Heal())
        {
            m_FXIdx[FX_LACRIMA] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma_Arm"), mat_arm, (m_Offset_Matrix * mat_arm * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_FXIdx[FX_LACRIMA_HAND] = m_pGameInstance->Spawn_Effect(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma"), (m_Offset_Matrix * mat_hand * m_pTransformCom->Get_WorldMatrix()).r[3]);
            m_iHealIndex = 6;
        }
    }
    if (m_pAnimInteraction->Is_Heal())
    {
        m_pGameInstance->Update_Effect_World(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma_Arm"), m_FXIdx[FX_LACRIMA], mat_arm, (m_Offset_Matrix * mat_arm * m_pTransformCom->Get_WorldMatrix()).r[3]);
        m_pGameInstance->Update_Effect_Position(m_pGameInstance->Get_CurrentLevelID(), TEXT("Lachryma"), m_FXIdx[FX_LACRIMA_HAND], (m_Offset_Matrix * mat_hand * m_pTransformCom->Get_WorldMatrix()).r[3]);
        Clear_Step2();
    }

    return false;
}

_bool CKhazan_GSword::LadderClimb_Input(_float fTimeDelta)
{
    /* ?궗?떎由? ???湲? ?걹?굹怨? 臾닿린 爰쇰궪?븣 源뚯?? */
    if (Has_Status(LADDER_CLIMBING_END))
    {
        if (m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed")) && m_pBody->Get_Model()->Check_MinAnimationTime()) {
            Remove_Status(LADDER_CLIMBING_END | LADDER_CLIMBING | LADDER_SPRINT);
            m_pClientInstance->Set_PlayerInput(true);
        }
        return true;
    }

    /* ?궗?떎由? ???湲? ?떆?옉?떆  ?쉶?쟾?떆?궎湲?  */
    if (Has_Status(LADDER_CLIMBING_ROTATION) && m_pAnimLadder->Is_Climbing()) {
        m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
        Remove_Status(LADDER_CLIMBING_ROTATION);
    }

    CKhazan_GS_Anim_Ladder::GS_LADDERINFO info;  

    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
    {
        if (m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] >= m_EventInteract.LadderEvent.vEndActionPos.x + 1.6f)
        {
            m_EventInteract.isEvent = true;
            m_EventInteract.LadderEvent.eLadderState = EventLadder::LADDER_ACTION::UPEND;
            return true;
        }

        info.eDir.Add_Flag(DIR::U);
    }
    else if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
    {
        if (m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] <= m_EventInteract.LadderEvent.vEndActionPos.y - 1.6f)
        {
            m_EventInteract.isEvent = true;
            m_EventInteract.LadderEvent.eLadderState = EventLadder::LADDER_ACTION::DOWNEND;
            return true;
        }

        info.eDir.Add_Flag(DIR::D);
    }

    if (m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta)) {
        //info.isSprint = true;
        Add_Status(LADDER_SPRINT);
    }
    else if (m_pGameInstance->Key_Up(DIK_SPACE)) {
       // info.isSprint = false;
        Remove_Status(LADDER_SPRINT);
    }

    m_pAnimLadder->Try_PlayLadder(info);

    return true;
}

void CKhazan_GSword::Change_MoveIdle(_float fTimeDelt)
{
    // ?굺?븯 以묒뿉?뒗 ?븷?땲硫붿씠?뀡 蹂?寃? 湲덉??
    if (Has_Status(FALLING | FALLING_ATTACK | PRE_LAND))
        return;

    /* ?떣吏??븯湲? */
    if (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_DODGE)&& !m_pAnimMove->IsDodge() )
    {
        /* ?떣吏? : ?뒪?깭誘몃굹 ?냼紐?*/
        if (m_pPlayerData->fCulStamina != 0.f && !Has_Status(DODGE_ENDING))
        {
            //m_isGhost = true;
            Add_Status(DODGE_ENDING);
            CKhazan_GS_Anim_Move::GS_MOVEINFO info;

            if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
            else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
            else if (Has_Status(GSWORD))info.iWeapon = GSWORD;

            info.isLockOn = Has_Status(LOCKON);
            info.iState = m_iCurSubState;
            info.iCycle = m_iCycle;
            info.eDir = m_eDir;

     //       m_pAnimMove->Try_ChangeAnimation(info);

            m_pPlayerData->fCulStamina = max(0.f, m_pPlayerData->fCulStamina - m_pPlayerData->fUsedStamina);
        }
        else
        {
            Remove_SubState(MOV::MOVE_DODGE);
            Add_SubState(MOV::MOVE_RUN);
        }
    }

    // 怨듦꺽 以묒씪 ?븣?뒗 Move ?븷?땲硫붿씠?뀡 蹂?寃? 湲덉??
    if (Has_State(CAT::M_ATTACK) && m_pAnimAttack->Is_Attacking())
        return;

    // Guard 以묒씪 ?븣?룄 泥댄겕
    if (Has_State(CAT::M_GUARD) && m_pAnimGuard->Is_Guarding())
        return;

    /*  ?씫?삩 泥댄겕 + 諛? ?떣吏? + ?떣吏?媛? ?븘?땲硫? */
    if (Has_Status(LOCKON) && !Has_Status(BACK_DODGE) && !Has_SubState(MOV::MOVE_DODGE)) {
        m_eDir = Calculate_LockOnDirection(fTimeDelt);
    }

    /* ?씠?룞以? ?뒪?럹?씠?뒪諛? ?늻瑜대뒗寃? ?븣臾몄뿉 ?떎?떆 ?슂泥??븯湲?  */
    if (Has_Status(SPRINT_AGAIN_REQUEST))
    {
        CKhazan_GS_Anim_Move::GS_MOVEINFO info;
        if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
        else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
        else if (Has_Status(GSWORD))info.iWeapon = GSWORD;
        info.isLockOn = Has_Status(LOCKON);
        info.iState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;

        m_pAnimMove->Reserve_Animation(info);

        Remove_Status(SPRINT_AGAIN_REQUEST);
    }

    /* ?씫?삩?긽?깭?뿉?꽌 ?떣吏?以묒씪 ?븣?뒗 諛⑺뼢 蹂?寃? 嫄대꼫?슌?뀭湲?*/
    if (Has_Status(LOCKON) && m_eDir.iDirFlag != m_ePrevDir && !Has_SubState(MOV::MOVE_DODGE))
    {

        CKhazan_GS_Anim_Move::GS_MOVEINFO info;
        if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
        else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
        else if (Has_Status(GSWORD))info.iWeapon = GSWORD;
        info.isLockOn = Has_Status(LOCKON);
        info.iState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Try_ChangeAnimation(info);
        return;
    }

    _bool isNothingState = m_iCurMainState == m_iPrevMainState && m_iCurSubState == m_iPrevSubState&& m_iCycle == m_iPrevCycle;
    _bool isIdleState = !Has_State(CAT::M_END - 2) ;
    /* Idle */
    if ((isIdleState && Has_Status(LOCKON) && isNothingState) || (isIdleState && m_pBody->Get_Model()->IsFinished()))
    {
        if ((Has_Status(STAMINA_EXHAUSTION)))
            return;

        if (m_pAnimInteraction->Is_Heal())
            return;

        if (m_pBody->Get_Model()->IsCurSetAnimation())
            return;

        if (m_isLadderEndEvent)
            return;

        _uint iGSwordStand = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Stand");
        _uint iSpearStand = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Stand");
        _uint iBareHandStand = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_BareHands_Stand");
        _uint iCurAnimIndex = m_pBody->Get_Model()->Get_CurAnimIndex();
        if (iCurAnimIndex != iGSwordStand && iCurAnimIndex != iSpearStand && iCurAnimIndex != iBareHandStand)
            m_pBody->Get_Model()->Set_Animation( Has_Status(GSWORD) ? iGSwordStand : (Has_Status(SPEAR) ? iSpearStand : iBareHandStand) );

    }

    if (isNothingState)
    {
        return;
    }

    /* Move  */
    if (((Has_Status(LOCKON) && m_eDir.iDirFlag != m_ePrevDir && m_eDir.iDirFlag > 0)) || Has_State(CAT::M_MOVE) && !Has_State(CAT::M_ATTACK | CAT::M_GUARD)|| !Has_SubState(MOV::MOVE_DODGE))
    {
        CKhazan_GS_Anim_Move::GS_MOVEINFO info;
        if (Has_Status(BAREHAND))info.iWeapon = BAREHAND;
        else if (Has_Status(SPEAR))info.iWeapon = SPEAR;
        else if (Has_Status(GSWORD))info.iWeapon = GSWORD;
        info.isLockOn = Has_Status(LOCKON);
        info.iState = m_iCurSubState;
        info.iCycle = m_iCycle;
        info.eDir = m_eDir;
        m_pAnimMove->Try_ChangeAnimation(info);

        //Remove_State(CAT::M_IDLE);
    }

}

void CKhazan_GSword::ExecuteAnimationExit()
{
    //if(m_iPrevMainState & CAT::M_DIE)
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_FALL) m_pAnimFall->Exit();
    //if(m_iPrevMainState &   CAT::M_GROGGY           )
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_DAMAGED) m_pAnimDamaged->Exit();
    //if(m_iPrevMainState &   CAT::M_CLIMB            )
    if (((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_SKILL) && m_pPlayerData->fCulStamina >= m_pPlayerData->fUsedStamina*2.f) m_pAnimAttack->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_GUARD) m_pAnimGuard->Exit();
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_ATTACK) {
        m_pAnimAttack->Exit();
        m_pBody->AllAttackCollisionActive_Off();
    }
    if ((m_iCurMainState != m_iPrevMainState) && m_iPrevMainState & CAT::M_MOVE) m_pAnimMove->Exit();
    //if(m_iPrevMainState &   CAT::M_LOCKON           )
    //if(m_iPrevMainState &   CAT::M_INTERACT         )
    //if(m_iPrevMainState &   CAT::M_WEAPON_CHANGE    )
    //if(m_iPrevMainState &   CAT::M_IDLE             )
    //if(m_iPrevMainState &   CAT::M_END              )

}

_bool CKhazan_GSword::ChangeGrabAnimation(_float fTimeDelta)
{
    if (m_isGrab && !m_isGrab2)
    {
        m_fGrabStartTime.x += fTimeDelta;

        if (m_fGrabStartTime.y <= m_fGrabStartTime.x)
        {
            m_isGrab2 = true;
            m_fGrabStartTime.x = 0.f;
        }
        return false;
    }

    if (m_isGrab && m_isGrab2 && m_pCharVirCom->Get_isGround())
    {
        m_iCurAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_DamAir_F");
        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);

        m_isGrabFinish = true;
        m_isGrab = m_isGrab2 = false;

    }

    if (m_isGrabFinish)
    {
        m_fGrabFinishTime.x += fTimeDelta;

        if (m_fGrabFinishTime.y <= m_fGrabFinishTime.x)
        {
            m_pClientInstance->Set_PlayerInput(true);
            m_isGrabFinish = m_isGrabFinish = false;
            return true;
        }
    }

    return false;
}

void CKhazan_GSword::Apply_PlayerMovement(_float fTimeDelta)
{
    //if (m_pAnimMove->IsDodge() || m_pAnimAttack->Is_Attacking() || !m_pAnimMove->IsMoving())
    //    return;

    // ?굺?븯 以묒뿉?뒗 ?씠?룞 湲덉??
    if (Has_Status(FALLING | FALLING_ATTACK | PRE_LAND))
        return;

    // 怨듦꺽 以묒씪 ?븣?뒗 ?씠?룞?븯吏? ?븡?쓬
    if (m_pAnimAttack->Is_Attacking()) {
        //?뒪?봽由고듃 泥댄겕 媛뺥솕
        if (Has_SubState(ATT::ATK_SPRINTATK))
            return;

        //?븷?땲硫붿씠?뀡 理쒖냼蹂댁옣?떆媛?
        if (!m_pBody->Get_Model()->Check_MinAnimationTime())
            return;
    }
    /* ?떣吏? ?씪?븣 ?씠?룞?븯吏? ?븡?쓬*/
    if (Has_State(CAT::M_MOVE) && Has_SubState(MOV::MOVE_DODGE))
        return;

    /* stop ?븷?땲硫붿씠?뀡?룄 ?씠?룞 x  */
    for (size_t i = 0; i < 10; i++)
        if (m_iStopMoveIndexTable[i] == m_pBody->Get_Model()->Get_CurAnimIndex()) {
            Remove_State(CAT::M_MOVE);
            return;
        }

    _float4x4 CamWorldMatrix = *m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
    _vector vCamLook = XMLoadFloat3((_float3*)&CamWorldMatrix._31);
    _vector vRight = XMVector3Normalize(XMVectorSetW(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook), 0.f));
    _vector vLook = XMVector3Normalize(XMVectorSetW(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)), 0.f));

    _vector vPlayerPosition = m_pTransformCom->Get_State(STATE::POSITION);

    /* ?냽?룄 ?꽕?젙 */
    _float fSpeed = 0.f;
    if (m_pAnimGuard->Is_WalkGuarding())fSpeed = m_fWalkSpeed;
    else if (Has_SubState(MOV::MOVE_SPRINT)) fSpeed = m_fSprintSpeed;
    else if (Has_SubState(MOV::MOVE_WALK)) fSpeed = m_fWalkSpeed;
    else if (Has_SubState(MOV::MOVE_RUN)) fSpeed = m_fRunSpeed;
    else if (Has_SubState(MOV::MOVE_INJURED)) fSpeed = m_fInjuredSpeed;

    /*  移대찓?씪 湲곗?? ?씠?룞 諛⑺뼢 踰≫꽣 怨꾩궛  */
    _vector vMoveDirection = XMVectorSet(0.f, 0.f, 0.f, 0.f);
    _bool isMoving = false;

    // m_eWorldDir ?궗?슜 (移대찓?씪 湲곗?? ?엯?젰)
    if (m_eWorldDir.Check_Flag(DIR::F))
    {
        vMoveDirection += vLook;
        isMoving = true;
    }
    if (m_eWorldDir.Check_Flag(DIR::B))
    {
        vMoveDirection -= vLook;
        isMoving = true;
    }
    if (m_eWorldDir.Check_Flag(DIR::L))
    {
        vMoveDirection -= vRight;
        isMoving = true;
    }
    if (m_eWorldDir.Check_Flag(DIR::R))
    {
        vMoveDirection += vRight;
        isMoving = true;
    }

    /*  ?씠?룞 ?쟻?슜*/
    if (isMoving)
    {
        vMoveDirection = XMVector3Normalize(vMoveDirection);
        vPlayerPosition += vMoveDirection * fSpeed * fTimeDelta;
        m_pTransformCom->Set_State(STATE::POSITION, vPlayerPosition);
    }

    // ?뮘 諛⑺뼢 ?떣吏??씤吏? 泥댄겕 (B, BR, BL 紐⑤몢 ?룷?븿)
    _bool isBackwardDodge = Has_Status(BACK_DODGE) || (m_eDir.Check_Flag(DIR::B) && Has_SubState(MOV::MOVE_DODGE));

    // ?쉶?쟾 泥섎━
    if (!isBackwardDodge) {
        if (!Has_SubState(MOV::MOVE_SPRINT) && Has_Status(LOCKON) && m_pCamera && m_pCamera->Get_IsLockOn())
        {
            LockOn_Rotation(fTimeDelta);
        }
        else
        {
            /* ?씫?삩?씠 ?븘?땲硫? ?룊?긽?떆???濡? */
            if (Has_State(CAT::M_MOVE) && isMoving)
            {
                _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
                // ???寃? 猷? = ?씠?룞 諛⑺뼢 (移대찓?씪 湲곗??!!!)
                _vector vTargetLook = vMoveDirection;

                _float fDotProduct = XMVectorGetX(XMVector3Dot(vPlayerLook, vTargetLook));

                // ?쉶?쟾 ?떆?옉 議곌굔 (?빟 5?룄 ?씠?긽 李⑥씠)
                if (!Has_Status(ROTATION) && fDotProduct < 0.996f)
                {
                    Add_Status(ROTATION);
                    m_fRotateTime[0] = 0.f;
                    m_vRotateStart = vPlayerLook;
                }

                if (Has_Status(ROTATION))
                {
                    m_fRotateTime[0] += fTimeDelta;
                    _float t = min(m_fRotateTime[0] / m_fRotateTime[1], 1.0f);

                    if (t >= 1.0f)
                    {
                        // ?쉶?쟾 ?셿猷?
                        _float yaw = atan2f(XMVectorGetX(vTargetLook), XMVectorGetZ(vTargetLook));
                        _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
                        m_pTransformCom->Set_Quaternion(q);
                        Remove_Status(ROTATION);
                    }
                    else
                    {
                        // ?쉶?쟾 以? - Slerp 蹂닿컙
                        _vector vInterpolated = XMVector3Normalize(XMVectorLerp(m_vRotateStart, vTargetLook, t));
                        _float yaw = atan2f(XMVectorGetX(vInterpolated), XMVectorGetZ(vInterpolated));
                        _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
                        m_pTransformCom->Set_Quaternion(q);
                    }
                }
                else
                {
                    // 媛곷룄 李⑥씠媛? ?옉?쓣 ?븣 利됱떆 ?쉶?쟾
                    _float yaw = atan2f(XMVectorGetX(vTargetLook), XMVectorGetZ(vTargetLook));
                    _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
                    m_pTransformCom->Set_Quaternion(q);
                }
            }
        }
    }
}



void CKhazan_GSword::Check_KeyInput_Direction(_float fTimeDelta)
{
    // 移대찓?씪 諛⑺뼢 怨꾩궛
    _float4x4 CamWorldMatrix = *m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
    _vector vCamLook = XMLoadFloat3((_float3*)&CamWorldMatrix._31);
    vCamLook = XMVector3Normalize(XMVectorSetW(vCamLook, 0.f));

    _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    _vector vCamRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook));

    _float dotLook = XMVectorGetX(XMVector3Dot(vPlayerLook, vCamLook));
    _float dotRight = XMVectorGetX(XMVector3Dot(vPlayerLook, vCamRight));

    PLAYER_CAMERA_DIR playerCamDir = PC_FRONT;
    _float angle = atan2f(dotRight, dotLook);

    if (angle >= -XM_PI / 8.f && angle < XM_PI / 8.f)                      playerCamDir = PC_FRONT;
    else if (angle >= XM_PI / 8.f && angle < 3.f * XM_PI / 8.f)           playerCamDir = PC_FRONT_RIGHT;
    else if (angle >= 3.f * XM_PI / 8.f && angle < 5.f * XM_PI / 8.f)     playerCamDir = PC_RIGHT;
    else if (angle >= 5.f * XM_PI / 8.f && angle < 7.f * XM_PI / 8.f)     playerCamDir = PC_BACK_RIGHT;
    else if (angle >= 7.f * XM_PI / 8.f || angle < -7.f * XM_PI / 8.f)    playerCamDir = PC_BACK;
    else if (angle >= -7.f * XM_PI / 8.f && angle < -5.f * XM_PI / 8.f)   playerCamDir = PC_BACK_LEFT;
    else if (angle >= -5.f * XM_PI / 8.f && angle < -3.f * XM_PI / 8.f)   playerCamDir = PC_LEFT;
    else if (angle >= -3.f * XM_PI / 8.f && angle < -XM_PI / 8.f)         playerCamDir = PC_FRONT_LEFT;

    // ?궎 ?엯?젰
    _bool isW = m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta);
    _bool isS = m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta);
    _bool isA = m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta);
    _bool isD = m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta);

    // 移대찓?씪 湲곗?? ?썡?뱶 諛⑺뼢 (?씠?룞/?쉶?쟾?슜)
    m_eWorldDir.Clear_Flag();
    if (isW && !isS && !isA && !isD)      m_eWorldDir.Add_Flag(DIR::F);
    else if (!isW && isS && !isA && !isD) m_eWorldDir.Add_Flag(DIR::B);
    else if (!isW && !isS && isA && !isD) m_eWorldDir.Add_Flag(DIR::L);
    else if (!isW && !isS && !isA && isD) m_eWorldDir.Add_Flag(DIR::R);
    else if (isW && !isS && isA && !isD)  m_eWorldDir.Add_Flag(DIR::F | DIR::L);
    else if (isW && !isS && !isA && isD)  m_eWorldDir.Add_Flag(DIR::F | DIR::R);
    else if (!isW && isS && isA && !isD)  m_eWorldDir.Add_Flag(DIR::B | DIR::L);
    else if (!isW && isS && !isA && isD)  m_eWorldDir.Add_Flag(DIR::B | DIR::R);

    // ?뵆?젅?씠?뼱 濡쒖뺄 諛⑺뼢 (?븷?땲硫붿씠?뀡 ?꽑?깮?슜)
    m_eDir.Clear_Flag();
    if (m_eWorldDir.Check_Flag(DIR::F | DIR::B | DIR::L | DIR::R))
    {
        m_eDir.Add_Flag(ConvertCameraToPlayerDir(playerCamDir));
    }
}

DIRECTION_INFO CKhazan_GSword::Calculate_LockOnDirection(_float fTimeDelta)
{
    DIRECTION_INFO lockOnDir;
    lockOnDir.Clear_Flag();

    if (!m_pCamera || !m_pCamera->Get_IsLockOn())
        return lockOnDir;

    // ?씫?삩 ???寃? ?쐞移? 媛??졇?삤湲?
    _float4* pLockOnPos = m_pCamera->Get_LockOnPosition();
    if (!pLockOnPos)
        return lockOnDir;

    _vector vTargetPos = XMLoadFloat4(pLockOnPos);
    _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

    // ?뵆?젅?씠?뼱?뿉?꽌 ???寃잛쑝濡쒖쓽 諛⑺뼢 (?뵆?젅?씠?뼱?쓽 forward)
    _vector vToTarget = XMVector3Normalize(vTargetPos - vPlayerPos);
    vToTarget = XMVectorSetY(vToTarget, 0.f); // Y異? ?젣嫄?

    // ?뵆?젅?씠?뼱?쓽 ?쁽?옱 Look 諛⑺뼢
    _vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);

    // ?뵆?젅?씠?뼱?쓽 Right 諛⑺뼢
    _vector vPlayerRight = XMVector3Normalize(m_pTransformCom->Get_State(STATE::RIGHT));

    // 移대찓?씪 湲곗?? ?씠?룞 諛⑺뼢 怨꾩궛
    _float4x4 CamWorldMatrix = *m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW);
    _vector vCamLook = XMLoadFloat3((_float3*)&CamWorldMatrix._31);
    _vector vCamRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook));
    _vector vCamForward = XMVector3Normalize(XMVector3Cross(vCamRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    // ?엯?젰 諛⑺뼢?쓣 ?썡?뱶 怨듦컙?뿉?꽌?쓽 ?씠?룞 踰≫꽣濡? 蹂??솚
    _vector vMoveDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

    _bool isW = m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta);
    _bool isS = m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta);
    _bool isA = m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta);
    _bool isD = m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta);

    if (isW) vMoveDir += vCamForward;
    if (isS) vMoveDir -= vCamForward;
    if (isA) vMoveDir -= vCamRight;
    if (isD) vMoveDir += vCamRight;

    if (XMVectorGetX(XMVector3Length(vMoveDir)) < 0.01f)
        return lockOnDir;

    vMoveDir = XMVector3Normalize(vMoveDir);

    // ?씠?룞 諛⑺뼢?쓣 ???寃? 湲곗?? 醫뚰몴怨꾨줈 蹂??솚
    // Forward = ???寃? 諛⑺뼢, Right = ???寃잛쓽 ?삤瑜몄そ
    _float fDotForward = XMVectorGetX(XMVector3Dot(vMoveDir, vToTarget));
    _float fDotRight = XMVectorGetX(XMVector3Dot(vMoveDir, XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vToTarget)));

    // 8諛⑺뼢 寃곗젙 (45?룄?뵫 援щ텇)
    _float angle = atan2f(fDotRight, fDotForward);

    if (angle >= -XM_PI / 8.f && angle < XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::F);
    else if (angle >= XM_PI / 8.f && angle < 3.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::F | DIRECTION_INFO::R);
    else if (angle >= 3.f * XM_PI / 8.f && angle < 5.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::R);
    else if (angle >= 5.f * XM_PI / 8.f && angle < 7.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::B | DIRECTION_INFO::R);
    else if (angle >= 7.f * XM_PI / 8.f || angle < -7.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::B);
    else if (angle >= -7.f * XM_PI / 8.f && angle < -5.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::B | DIRECTION_INFO::L);
    else if (angle >= -5.f * XM_PI / 8.f && angle < -3.f * XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::L);
    else if (angle >= -3.f * XM_PI / 8.f && angle < -XM_PI / 8.f)
        lockOnDir.Add_Flag(DIRECTION_INFO::F | DIRECTION_INFO::L);

    return lockOnDir;
}

void CKhazan_GSword::LockOn_Rotation(_float fTimeDelta)
{
    // ?씫?삩 ?긽?깭?뿉?꽌?뒗 ?빆?긽 ???寃잛쓣 諛붾씪蹂닿쾶
    _float4* pLockOnPos = m_pCamera->Get_LockOnPosition();
    if (pLockOnPos)
    {
        _vector vTargetPos = XMLoadFloat4(pLockOnPos);
        _vector vPlayerPos = m_pTransformCom->Get_State(STATE::POSITION);

        _vector vToTarget = vTargetPos - vPlayerPos;
        vToTarget = XMVectorSetY(vToTarget, 0.f);
        vToTarget = XMVector3Normalize(vToTarget);

        // 遺??뱶?윭?슫 ?쉶?쟾
        _vector vCurrentLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
        _vector vNewLook = XMVector3Normalize(XMVectorLerp(vCurrentLook, vToTarget, 8.f * fTimeDelta));

        _float yaw = atan2f(XMVectorGetX(vNewLook), XMVectorGetZ(vNewLook));
        _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
        m_pTransformCom->Set_Quaternion(q);
    }
}
void CKhazan_GSword::Setting_Guard_Rotation()
{
    m_fGuardRotationTime.x = 0.f;

    _vector vCurrentLook = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    vCurrentLook = XMVectorSetY(vCurrentLook, 0.f);
    m_vGuardRotationStart = vCurrentLook;

    m_vGuardRotationEnd = XMLoadFloat4(&m_vGuardRotationTarget);

    Remove_Status(GUARD_ROTATION_REQUEST);
    Add_Status(GUARD_ROTATION);

}
void CKhazan_GSword::Guard_Rotation(_float fTimeDelta)
{
    m_fGuardRotationTime.x += fTimeDelta;

    // Ease-Out Quadratic
    _float fRatio = m_fGuardRotationTime.x / m_fGuardRotationTime.y;
    fRatio = min(fRatio, 1.0f);
    fRatio = 1.f - (1.f - fRatio) * (1.f - fRatio);

    _vector vCurrentLook = XMVector3Normalize(
        XMVectorLerp(m_vGuardRotationStart, m_vGuardRotationEnd, fRatio)
    );

    // ?쉶?쟾 ?셿猷? 泥댄겕
    if (fRatio >= 1.0f)
    {
        vCurrentLook = m_vGuardRotationEnd;
        Remove_Status(GUARD_ROTATION);
    }

    _float yaw = atan2f(XMVectorGetX(vCurrentLook), XMVectorGetZ(vCurrentLook));
    _vector q = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), yaw);
    m_pTransformCom->Set_Quaternion(q);
}
void CKhazan_GSword::Update_LockOn()
{
    // 移대찓?씪?쓽 ?씫?삩 ?긽?깭??? ?룞湲고솕
    if (m_pCamera)
    {
        _bool   isLockOn = m_pCamera->Get_IsLockOn();
        if (isLockOn != Has_Status(LOCKON))
        {
            if (isLockOn) {
                Add_Status(LOCKON);
            }
            else {
                Remove_Status(LOCKON);
            }
        }
    }
}

void CKhazan_GSword::Update_Die(_float fTimeDelta)
{
    if (m_pBody->Get_Model()->Get_CurAnimIndex() == m_iCurAnimIndex && m_pBody->Get_Model()->IsFinished())
    {
        /* ?븳踰덈쭔 ?뱾?뼱媛?.  */
        m_iCurAnimIndex = Has_Status(GSWORD)
            ? m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_Down_Loop_F")
            : m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_DownDie_F");

        m_pBody->Get_Model()->Set_Animation(m_iCurAnimIndex);
    }
}

void CKhazan_GSword::Teleport_SFX()
{

}

void CKhazan_GSword::Clear_Injured()
{
    Clear_State();
    Clear_SubState();
    Clear_CycleState();

    //Remove_Status(INJURED);
}

void CKhazan_GSword::EnterStatuePuzzle()
{
    Clear_Step2();
    m_pClientInstance->Set_PlayerInput(false);

    if (Has_Status(SPEAR))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed"));
    if (Has_Status(GSWORD))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed"));

    Add_Status(BLOCK_ATK_SKILL_GUARD | STATUE_MODE | BAREHAND);
    Remove_Status(GSWORD);
}

void CKhazan_GSword::ExitStatuePuzzle()
{
    Remove_Status( BLOCK_ATK_SKILL_GUARD | BAREHAND);
    Add_Status(GSWORD);

    if (Has_Status(SPEAR))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Armed"));
    if (Has_Status(GSWORD))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed"));  

  Clear_Step2();
    m_pClientInstance->Set_PlayerInput(false);
  

}


void CKhazan_GSword::Get_HitReaction(_float3 vContactPoint)
{
    // 1. 罹먮┃?꽣?쓽 ?쐞移? 援ы븯湲?
    _vector vCharacterPos = m_pTransformCom->Get_State(STATE::POSITION);

    // 2. 罹먮┃?꽣 -> ?젒珥됱젏 諛⑺뼢 踰≫꽣 怨꾩궛
    _vector vHitDir = XMLoadFloat3(&vContactPoint) - vCharacterPos;

    // 3. ?넂?씠 李⑥씠 怨꾩궛 (Y異?)
    _float fHeightDiff = XMVectorGetY(vHitDir);

    // 4. XZ ?룊硫? 諛⑺뼢 怨꾩궛
    _vector vHitDirXZ = XMVectorSetY(vHitDir, 0.f);

    _float fLengthSq = XMVectorGetX(XMVector3LengthSq(vHitDirXZ));
    if (fLengthSq < 0.0001f) // 嫄곗쓽 0?씤 寃쎌슦
    {
        // 湲곕낯媛?: 罹먮┃?꽣?쓽 ?젙硫? 諛⑺뼢?쓣 ?궗?슜
        vHitDirXZ = m_pTransformCom->Get_State(STATE::LOOK);
        vHitDirXZ = XMVectorSetY(vHitDirXZ, 0.f);
    }

    vHitDirXZ = XMVector3Normalize(vHitDirXZ);

    // 5. 罹먮┃?꽣?쓽 Forward??? Right 踰≫꽣 援ы븯湲?
    _vector vCharacterForward = m_pTransformCom->Get_State(STATE::LOOK);
    vCharacterForward = XMVector3Normalize(vCharacterForward);

    _vector vCharacterRight = m_pTransformCom->Get_State(STATE::RIGHT);
    vCharacterRight = XMVector3Normalize(vCharacterRight);

    // 6. ?궡?쟻?쑝濡? 諛⑺뼢 怨꾩궛
    _float fDotForward = XMVectorGetX(XMVector3Dot(vCharacterForward, vHitDirXZ));
    _float fDotRight = XMVectorGetX(XMVector3Dot(vCharacterRight, vHitDirXZ));

    // 7. 媛곷룄 怨꾩궛
    _float fAngle = atan2f(fDotRight, fDotForward);
    fAngle = XMConvertToDegrees(fAngle);

    // 8. ?넂?씠 ?뙋?떒
    _float fHeightThreshold = 0.5f;
    bool bIsUp = (fHeightDiff > fHeightThreshold);


    m_eHitStrongDir.Clear_Flag();

    if (bIsUp) {
        m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::U);
        m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::F);
    }
    else {
        m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::D);

        if (fabs(fDotRight) > fabs(fDotForward) && fDotRight > 0.f) {
            m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::R);
            m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
        else {
            m_eHitStrongDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
    }


    m_eHitNormalDir.Clear_Flag();

    if (bIsUp) {
        m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::U);

        if (fDotForward < 0.f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::B);
        }
        else {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
    }
    else {
        m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::D);

        if (fAngle >= -22.5f && fAngle < 22.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
        else if (fAngle >= 22.5f && fAngle < 157.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::R);
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
        else if (fAngle >= 157.5f || fAngle < -157.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::B);
        }
        else if (fAngle >= -157.5f && fAngle < -22.5f) {
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::L);
            m_eHitNormalDir.Add_Flag(DIRECTION_INFO::DIR::F);
        }
    }

}


void CKhazan_GSword::Check_IsInAir(_float fTimeDelta)
{

    if (m_pCharVirCom->Get_isGround())
        return;

    if (m_isGrab || m_isGrabFinish)
    {
        Remove_Status(FALLING | FALLING_ATTACK | PRE_LAND);
        return;
    }

    _vector vRayStart = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vRayEnd = vRayStart + XMVectorSet(0.f, -m_fRayLength, 0.f, 0.f);

    _float fFraction;
    _float4 vPosition;
    _float3 outNormal;

    if (m_pGameInstance->RayCast(
        _float3(XMVectorGetX(vRayStart), XMVectorGetY(vRayStart), XMVectorGetZ(vRayStart)),
        _float3(XMVectorGetX(vRayEnd), XMVectorGetY(vRayEnd), XMVectorGetZ(vRayEnd)),
        fFraction,
        vPosition,
        &outNormal
    ))
    {
        /* 吏?硫댁뿉 ?룄?떖 - 李⑹?? */
        if (fFraction <= 0.2f)
        {
            // ?굺?븯 以묒씠?뿀?떎硫? 李⑹?? ?셿猷?
            if (Has_Status(FALLING | FALLING_ATTACK | PRE_LAND))
            {
                // PRE_LAND ?긽?깭媛? ?븘?땲硫? 諛붾줈 PRE_LAND濡? ?쟾?솚
                if (!Has_Status(PRE_LAND))
                {
                    Remove_Status(FALLING);
                    Add_Status(PRE_LAND);
                   // cout << " === LANDING !!! ===" << endl;
                }
            }
        }
        /* 李⑹?? 吏곸쟾 - 李⑹?? ?븷?땲硫붿씠?뀡 以?鍮? */
        else if ((Has_Status(FALLING) || Has_Status(FALLING_ATTACK)) && !Has_Status(PRE_LAND) && fFraction <= 0.4f)
        {
            Add_Status(PRE_LAND);
           // cout << " === PRE_LAND !!! ===" << endl;
        }
        /* 怨듭쨷 - ?굺?븯 ?떆?옉 */
        else if (!Has_Status(FALLING | FALLING_ATTACK | PRE_LAND) && fFraction > 0.2f)
        {
            Add_Status(FALLING);
            Add_State(CAT::M_FALL);
            //cout << " === FALLING START !!! ===" << endl;
        }


    }
}

void CKhazan_GSword::Check_Statue()
{
    if (!Has_Status(STATUE_MODE)) return;

    if (m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed")) && m_pBody->Get_Model()->IsFinished())
       // || m_pBody->Get_Model()->Get_CurAnimIndex() != m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed"))
    {
        m_pClientInstance->Set_PlayerInput(true);
    }

    if (m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed")) && m_pBody->Get_Model()->IsFinished())
       // || m_pBody->Get_Model()->Get_CurAnimIndex() != m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed"))
    {
        m_pClientInstance->Set_PlayerInput(true);
        Remove_Status(STATUE_MODE);
    }

}

HRESULT CKhazan_GSword::Ready_Components()
{
    return S_OK;
}

HRESULT CKhazan_GSword::Ready_AnimationStateMachine()
{

    m_pAnimMove = CKhazan_GS_Anim_Move::Create();
    if (m_pAnimMove == nullptr)
        return E_FAIL;
    m_pAnimMove->Set_Model(m_pBody->Get_Model());
    m_pAnimMove->Initialize();

    m_pAnimAttack = CKhazan_GS_Anim_Attack::Create();
    if (m_pAnimAttack == nullptr)
        return E_FAIL;
    m_pAnimAttack->Set_Model(m_pBody->Get_Model());
    m_pAnimAttack->Initialize();

    m_pAnimGuard = CKhazan_GS_Anim_Guard::Create();
    if (m_pAnimGuard == nullptr)
        return E_FAIL;
    m_pAnimGuard->Set_Model(m_pBody->Get_Model());
    m_pBody->Set_IsGuarding(m_pAnimGuard->Get_IsGuarding());
    m_pBody->Set_IsLadderRotationEvent(&m_isLadderRotationEvent);

    m_pAnimInteraction = CKhazan_GS_Anim_Interaction::Create();
    if (m_pAnimInteraction == nullptr)
        return E_FAIL;
    m_pAnimInteraction->Set_Model(m_pBody->Get_Model());
    m_pAnimInteraction->Initialize();

    m_pAnimDamaged = CKhazan_GS_Anim_Damaged::Create();
    if (m_pAnimDamaged == nullptr)
        return E_FAIL;
    m_pAnimDamaged->Set_Model(m_pBody->Get_Model());

    m_pAnimFall = CKhazan_GS_Anim_Fall::Create();
    if (m_pAnimFall == nullptr)
        return E_FAIL;
    m_pAnimFall->Set_Model(m_pBody->Get_Model());


    m_pAnimLadder = CKhazan_GS_Anim_Ladder::Create();
    if (m_pAnimLadder == nullptr)
        return E_FAIL;
    m_pAnimLadder->Set_Model(m_pBody->Get_Model()); 
    m_pAnimLadder->Initialize();

    return S_OK;
}

void CKhazan_GSword::Ready_MotionTrailCallback()
{
    m_pBody->Set_MotionTrailCallBack([this](const _wstring& strKey, _bool isActive) {
        m_pBody->On_MotionTrail(strKey, isActive);
        m_pGSword->On_MotionTrail(strKey, isActive);
        });
}


void CKhazan_GSword::Clear_Step0()
{
    Clear_CycleState();
    Clear_SubState();
    Clear_State();
    Remove_Status(STATUS_CLEARS);

    m_eDir.iDirFlag = 0;
    m_eWorldDir.iDirFlag = 0;
    //m_ePrevDir = m_iPrevCycle = m_iPrevMainState = m_iPrevSubState = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;
    m_fChargingFastAttackTime = 0.f;
    m_iCurSkillIndex = 0;

    // m_f180TurnTime = { 0.f, 0.f };

    if (m_pAnimMove)
    {
        m_pAnimMove->Clear();
        // m_pAnimMove->Clear_Turn180();
    }

    if (m_pAnimAttack) m_pAnimAttack->Clear_All();
    if (m_pAnimDamaged) m_pAnimDamaged->Clear_Damaged();
    if (m_pAnimGuard) m_pAnimGuard->Clear();
    if (m_pAnimFall) m_pAnimFall->Clear();
}

void CKhazan_GSword::Clear_Step1()
{
    Clear_CycleState();
    Clear_SubState();
    Clear_State();
    Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_FAST_ATTACK | CHARGING_STRONG_ATTACK | SPRINT_AGAIN_REQUEST  | GUARD | GUARD_SUCCESS | JUST_GUARD | GUARD_ROTATION_REQUEST);
    //m_eDir.iDirFlag = 0;
    //m_eWorldDir.iDirFlag = 0;
    //m_ePrevDir = m_iPrevCycle = m_iPrevMainState = m_iPrevSubState = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;
    m_fChargingFastAttackTime = 0.f;
    m_iCurSkillIndex = 0;

    if (m_pAnimMove) m_pAnimMove->Clear();
    if (m_pAnimAttack) m_pAnimAttack->Clear_All();
    if (m_pAnimDamaged) m_pAnimDamaged->Clear_Damaged();
    if (m_pAnimGuard) m_pAnimGuard->Clear();
}

void CKhazan_GSword::Clear_Step2()
{
    if (Has_State(CAT::M_MOVE))
    {
        Clear_CycleState();
        Clear_SubState();
        Remove_State(CAT::M_MOVE);
    }
    Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | ROTATION | SPRINT_AGAIN_REQUEST | DODGING );

    m_eDir.iDirFlag = 0;
    m_eWorldDir.iDirFlag = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;

    if (m_pAnimMove)
    {
        m_pAnimMove->Clear();
        //  m_pAnimMove->Clear_Turn180();
    }
}

void CKhazan_GSword::Clear_Step3()
{
    Clear_CycleState();
    Clear_SubState();
    _bool isGuard = Has_State(CAT::M_GUARD);
    Clear_State();
    if (isGuard)
        Add_State(CAT::M_GUARD);

    Remove_Status(RESERVED | CHARGING_SPRINT | BACK_DODGE | CHARGING_FAST_ATTACK | SPRINT_AGAIN_REQUEST );
    m_eDir.iDirFlag = 0;
    m_eWorldDir.iDirFlag = 0;
    //m_ePrevDir = m_iPrevCycle = m_iPrevMainState = m_iPrevSubState = 0;
    m_fRotateTime[0] = 0.f;
    m_fSprintTime = 0.f;
    m_fChargingFastAttackTime = 0.f;
    m_iCurSkillIndex = 0;

    if (m_pAnimMove) m_pAnimMove->Clear();
    if (m_pAnimAttack) m_pAnimAttack->Clear_All();
    if (m_pAnimDamaged) m_pAnimDamaged->Clear_Damaged();

}

HRESULT CKhazan_GSword::Ready_PartObjects()
{
    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();

    CBody_Khazan_GS::BODY_KHAZAN_GS_DESC         BodyDesc{};
    BodyDesc.pState = &m_iCurMainState;
    BodyDesc.pStatus = &m_iStatus;
    //BodyDesc.pIsGuarding = m_pAnimGuard->Get_IsGuarding();
    BodyDesc.pHitReation = &m_eHitReaction;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pGuardRotationTarget = &m_vGuardRotationTarget;
    BodyDesc.pParentTransform = m_pTransformCom;
    BodyDesc.pParentIsCanStaminaRecovery = &m_isCanStaminaRecovery;
    BodyDesc.pHealIndex = &m_iHealIndex;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Body_Khazan_GS"), &BodyDesc)))
        return E_FAIL;
    m_pBody = static_cast<CBody_Khazan_GS*>(Find_PartObject(TEXT("Part_Body")));
    m_pWeaponR_Matrix = m_pBody->Get_BoneMatrix("Weapon_R");
    m_GSwordBackPack_Matrix = m_pBody->Get_BoneMatrix("Weapon_R_BackPack_GSword");
    m_SpearBackPack_Matrix = m_pBody->Get_BoneMatrix("Weapon_C_BackPack_Spear");
    m_LanternSocket_Matrix = m_pBody->Get_BoneMatrix("Lantern_Socket_L");
    m_LHandSocket_Matrix = m_pBody->Get_BoneMatrix("L_Hand_Socket");

    CGSword_Khazan_GS::GSWORD_KHAZAN_GS_DESC         GSwordDesc{};
    GSwordDesc.pStatus = &m_iStatus;
    GSwordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    GSwordDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_GSword_Khazan_GS"), &GSwordDesc)))
        return E_FAIL;
    m_pGSword = static_cast<CGSword_Khazan_GS*>(Find_PartObject(TEXT("Part_Weapon")));
    //m_pSpearFX_Matrix = m_pGSword->Get_BoneMatrix("FX");
    //m_SpearOffset_Matrix = m_pGSword->Get_OffestMatrix();

    CLantern_Khazan_GS::LANTERN_KHAZAN_SPEAR_DESC         LanternDesc{};
    LanternDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    LanternDesc.pParentTransform = m_pTransformCom;
    LanternDesc.pLHandSocket_Matrix = m_LHandSocket_Matrix;
    LanternDesc.pLanternSocket_Matrix = m_LanternSocket_Matrix;
    if (FAILED(__super::Add_PartObject(TEXT("Part_Lantern"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Lantern_Khazan_GS"), &LanternDesc)))
        return E_FAIL;
    m_pLantern = static_cast<CLantern_Khazan_GS*>(Find_PartObject(TEXT("Part_Lantern")));
                       
                 
    /* ?꽆寃⑥＜湲?  */         
    m_pGSword->Set_GSwordBackPack(m_GSwordBackPack_Matrix);
    m_pGSword->Set_SpearBackPack(m_SpearBackPack_Matrix);
    m_pGSword->Set_matWeaponR(m_pWeaponR_Matrix);
    m_pBody->Set_matGSword(m_pGSword_Matrix);
    m_pBody->Set_GSword(m_pGSword);
    //m_pBody->Set_IsLantern(m_pLantern->Get_isEquip());
    return S_OK;

}

HRESULT CKhazan_GSword::Ready_Collision()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.9f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    tCharVirDesc.fRadius = 0.6f;
    tCharVirDesc.fHeight = 0.8f;
    tCharVirDesc.fMaxSlopeAngle = 60.f;
    tCharVirDesc.fMass = 60.f;
    tCharVirDesc.fMaxStrength = 0.f;
    tCharVirDesc.fPredictiveContactDistance = 0.2f;
    tCharVirDesc.iMaxConstraintIterations = 20;
    tCharVirDesc.fCollisionTolerance = 0.03f;
    tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;
    m_tPlayerCollisionDesc.pGameObject = this;
    m_tPlayerCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    m_tPlayerCollisionDesc.strName = TEXT("Khazan_Body");
    tCharVirDesc.pCollisionDesc = &m_tPlayerCollisionDesc;
    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, -0.5f, 0);
    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.3f, 0.f);
    tCharVirDesc.fWalkStairsMinStepForward = 0.1f;
    tCharVirDesc.fWalkStairsStepForwardTest = 0.4f;    
    tCharVirDesc.vWalkStairsStepDownExtra = _float3(0.f, -0.25f, 0.f);
    tCharVirDesc.fWalkStairsCosAngleForwardContact = cosf(XMConvertToRadians(60.f));

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

_uint CKhazan_GSword::ConvertCameraToPlayerDir(PLAYER_CAMERA_DIR playerCamDir)
{
    // 8諛⑺뼢 蹂??솚 ?뀒?씠釉?
    // [?뵆?젅?씠?뼱媛? 移대찓?씪 湲곗???쑝濡? 蹂대뒗 諛⑺뼢][?엯?젰 諛⑺뼢] = ?뵆?젅?씠?뼱 湲곗?? 諛⑺뼢
    static const _uint conversionTable[8][8] = {
        // 移대찓?씪 ?엯?젰:    F,  R,  B,  L,  FR, BR, BL, FL
        {DIR::F, DIR::R, DIR::B, DIR::L, (DIR::F | DIR::R), (DIR::B | DIR::R), (DIR::B | DIR::L), (DIR::F | DIR::L)},               /* PC_FRONT */
        {(DIR::F | DIR::L), DIR::F, (DIR::F | DIR::R), DIR::L, DIR::F, (DIR::F | DIR::R), (DIR::B | DIR::R), (DIR::B | DIR::L)},    /* PC_FRONT_RIGHT */
        {DIR::L, DIR::F, DIR::R, DIR::B, (DIR::F | DIR::L), (DIR::F | DIR::R), (DIR::B | DIR::R), (DIR::B | DIR::L)},               /* PC_RIGHT */
        {(DIR::B | DIR::L), (DIR::F | DIR::L), DIR::F, (DIR::B | DIR::R), (DIR::F | DIR::L), DIR::F, (DIR::F | DIR::R), DIR::B},    /* PC_BACK_RIGHT */
        {DIR::B, DIR::L, DIR::F, DIR::R, (DIR::B | DIR::L), (DIR::F | DIR::L), (DIR::F | DIR::R), (DIR::B | DIR::R)},               /* PC_BACK */
        {(DIR::B | DIR::R), DIR::B, (DIR::F | DIR::L), (DIR::F | DIR::R), DIR::B, (DIR::B | DIR::L), DIR::F, (DIR::F | DIR::R)},    /* PC_BACK_LEFT */
        {DIR::R, DIR::B, DIR::L, DIR::F, (DIR::B | DIR::R), (DIR::B | DIR::L), (DIR::F | DIR::L), (DIR::F | DIR::R)},               /* PC_LEFT */
        {(DIR::F | DIR::R), (DIR::B | DIR::R), DIR::B, DIR::F, (DIR::B | DIR::R), DIR::B, (DIR::B | DIR::L), (DIR::F | DIR::L)}     /* PC_FRONT_LEFT */
    };
    // ?엯?젰 諛⑺뼢?쓣 ?씤?뜳?뒪濡? 蹂??솚
    int inputIdx = 0;

    if (m_eWorldDir.AllCheck_Flag((DIR::F | DIR::R))) inputIdx = 4;
    else if (m_eWorldDir.AllCheck_Flag((DIR::B | DIR::R))) inputIdx = 5;
    else if (m_eWorldDir.AllCheck_Flag((DIR::B | DIR::L))) inputIdx = 6;
    else if (m_eWorldDir.AllCheck_Flag((DIR::F | DIR::L))) inputIdx = 7;
    else if (m_eWorldDir.AllCheck_Flag(DIR::F)) inputIdx = 0;
    else if (m_eWorldDir.AllCheck_Flag(DIR::R)) inputIdx = 1;
    else if (m_eWorldDir.AllCheck_Flag(DIR::B)) inputIdx = 2;
    else if (m_eWorldDir.AllCheck_Flag(DIR::L)) inputIdx = 3;


    return conversionTable[playerCamDir][inputIdx];
}

#pragma region ?긽?샇 ?옉?슜 留? ?삤釉뚯젥?듃 ?씠踰ㅽ듃
void CKhazan_GSword::Subscribe_Events()
{

#pragma region ?긽?샇 ?옉?슜 留? ?삤釉뚯젥?듃 ?씠踰ㅽ듃
    m_iInteractTypeEventID = m_pGameInstance->Subscribe_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), [&](const EventInteractType& e) { m_EventInteract = e; });

    m_iObjectInteractEventID = m_pGameInstance->Subscribe_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), [&](const EventObject& e) {
        if (e.isOff())
        {
            if (m_EventInteract.isNPC())
            {
                if(Has_Status(SPEAR))
                    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Armed"));
                if (Has_Status(GSWORD))
                    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed"));

                static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
            }
            else if(INTERACTIVE_TYPE::CHECKPOINT == m_EventInteract.eInteractType)
            {
                m_pBody->Get_Model()->AnimationSetIndexIncrease();
                static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
            }
            else
            {
                static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
            }
        }  });

#pragma endregion
}


void CKhazan_GSword::Event_Interact_Object(_float fTimeDelta)
{
    // ?긽?샇 ?옉?슜 ?삤釉뚯젥?듃 履쎌뿉?꽌 BEGIN STATE ?궡蹂대궡硫? ?뵆?젅?씠?뼱?뿉?꽌 ?뻾?룞 ?썑, ?뻾?룞 ?셿猷? ?떆 ?씠踰ㅽ듃 諛쒖깮?쑝濡? ?긽?샇 ?옉?슜 ?삤釉뚯젥?듃 ?룞?옉
    if (EventInteractType::EVENT_STATE::BEGIN == m_EventInteract.eState)
    {
        if (false == m_isInteractEventSetting)
        {
            m_isInteractEventSetting = true;
            /*  UnArmed ?븷?땲硫붿씠?뀡 ?옱?깮  (議곌컖?긽?븣?뒗 ?븞?븿)*/
            if (!Has_Status(BLOCK_ATK_SKILL_GUARD) && INTERACTIVE_TYPE::TOMBSTONE != m_EventInteract.eInteractType)
            {
                cout << "@@@@@@@@@@@@@@  PARK !!!!!!!!!!!!!!!!!!! @@@@@@@@@@" << endl;
                if (Has_Status(SPEAR))
                    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed"));
                if (Has_Status(GSWORD))
                    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed"));
            }
            XMStoreFloat4(&m_vStartPos_Event, m_pTransformCom->Get_State(STATE::POSITION));
            m_fLerpTime_Event = 0.f;

        }
        // ?뵆?젅?씠?뼱 ?씠?룞, LOOK 蹂닿컙?? | ?셿猷뚰븯硫? ?씠踰ㅽ듃 諛섎??濡? ?뜕?졇二쇨린
        _bool isDone = { true };

        switch (m_EventInteract.eInteractType)
        {
        case INTERACTIVE_TYPE::CHEST:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsFinished())  isDone = true;   /* ?쁽?옱 ?옱?깮?릺?뒗 ?븷?땲硫붿씠?뀡?씠 UnArmed?씠怨? ?걹?궗?쑝硫? true濡? */
            break;
        }
        case INTERACTIVE_TYPE::CHECKPOINT:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsFinished())  isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::TOMBSTONE:
        {
            //isDone = false;
            //if (m_pBody->Get_Model()->IsFinished())  isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::LEVER:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsFinished())  isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::STATUE:
        {
            //isDone = false;
            //if (Has_Status(GSWORD) && m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed")) && m_pBody->Get_Model()->IsFinished())
            //{
            //    isDone = true;
            //    m_pClientInstance->Set_PlayerInput(true);
            //}
            //else if (Has_Status(SPEAR) && m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed")) && m_pBody->Get_Model()->IsFinished())
            //{
            //    isDone = true;
            //    m_pClientInstance->Set_PlayerInput(true);
            //}
            break;
        }
        case INTERACTIVE_TYPE::IRONGATE:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsFinished())  isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::UNLOCKGEAR:
        {
            isDone = false;
            string str = m_pBody->Get_Model()->Get_CurAnimName();
            cout << str << endl;
            if (m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed")) && *m_pBody->Get_Model()->Get_CurTrackPosition() >= 25.f)
                isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::GIANTGATE:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed")) && *m_pBody->Get_Model()->Get_CurTrackPosition() >= 25.f)  isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::DANJIN:
        case INTERACTIVE_TYPE::DUIMUK:
        case INTERACTIVE_TYPE::DAPHRONA:
        case INTERACTIVE_TYPE::GACHANPC:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsFinished()) {
                static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(false);
                isDone = true;
            }
            break;
        }
        case INTERACTIVE_TYPE::LADDER:
        {
            isDone = false;
            if (m_pBody->Get_Model()->IsFinished())  isDone = true;
            break;
        }
        case INTERACTIVE_TYPE::DESTINYSTONE:
        {
            break;
        }
        default:
            break;
        }

        /* ?뵆?젅?씠?뼱媛? ?삤釉뚯젥?듃?븳?뀒 "?굹 ?씠?젣 以?鍮꾨릱?뼱"瑜? 蹂대궦?떎. */
        if (isDone)               // ?듅?젙 議곌굔 ?셿?꽦?븯硫? ?씠踰ㅽ듃 諛쒖깮
        {
            // ?씠踰ㅽ듃?뿉 ?븘?슂?븳 ?꽭?똿?쓣 ?떎?쓬?뿉 ?삉 諛쒖깮?떆 蹂?寃? 媛??뒫?븯寃? false濡? 蹂?寃?
            m_isInteractEventSetting = false;

            // ?긽?샇?옉?슜 ?솢?꽦?솕?떆 留? ?삤釉뚯젥?듃?븳?뀒 EVENT_STATE瑜? ON ?쑝濡? ?뜕?졇以??떎
            _float4 vPosition = {};
            XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));
            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OnEvent_Player(vPosition) });
            // ?궡 ?긽?깭瑜? STATE::NONE ?쑝濡? 蹂?寃쏀빐以??떎.
            m_EventInteract.eState = EventInteractType::EVENT_STATE::NONE;
        }
    }

    // ?긽?샇 ?옉?슜 ?삤釉뚯젥?듃 履쎌뿉?꽌 END STATE ?궡蹂대궪 ?떆
    if (EventInteractType::EVENT_STATE::END == m_EventInteract.eState)
    {
        if (true)               // ?듅?젙 議곌굔 ?셿?꽦?븯硫? ?씠踰ㅽ듃 諛쒖깮
        {
            // ?긽?샇?옉?슜 鍮꾪솢?꽦?솕?떆 留? ?삤釉뚯젥?듃?븳?뀒 EVENT_STATE瑜? OFF 濡? ?뜕?졇以??떎
            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OffEvent() });
            // ?궡 ?긽?깭瑜? STATE::NONE ?쑝濡? 蹂?寃쏀빐以??떎.
            m_EventInteract.eState = EventInteractType::EVENT_STATE::NONE;
        }
    }

    // ?씠踰ㅽ듃媛? 諛쒖깮 ?뻽?쓣 ?븣
    if (true == m_EventInteract.isEvent)
    {
        // 洹?寃??씪?븣
        if (INTERACTIVE_TYPE::CHECKPOINT == m_EventInteract.eInteractType)
        {
            BladeNexus_Event(fTimeDelta);
        }
        // ?긽?옄?씪?븣 ( ?굹以묒뿉 李쎄퀬, ?뙆諛? ?긽?옄 ?굹?닃 ?삁?젙 )
        if (INTERACTIVE_TYPE::CHEST == m_EventInteract.eInteractType)
        {
            Chest_Event(fTimeDelta);
        }
        // 鍮꾨??諛? ?뿞諛붿뒪 ?댘?뒪?넠?씪?븣
        if (INTERACTIVE_TYPE::TOMBSTONE == m_EventInteract.eInteractType)
        {
            TombStone_Event(fTimeDelta);
        }
        // ?뿞諛붿뒪 ?젅踰꾩씪 ?븣
        if (INTERACTIVE_TYPE::LEVER == m_EventInteract.eInteractType)
        {
            Lever_Event(fTimeDelta);
        }
        // 議곌컖?긽 湲곕?뱀씪?븣
        if (INTERACTIVE_TYPE::STATUE == m_EventInteract.eInteractType)
        {
            Statue_Event(fTimeDelta);
        }
        // ?뿞諛붿뒪 ?쐞履? ?옞寃⑥엳?뒗 泥좊Ц?쓣 ?뿴?븣
        if (INTERACTIVE_TYPE::IRONGATE == m_EventInteract.eInteractType)
        {
            IronGate_Event(fTimeDelta);
        }
        // ?뿕由щ쿋?씠?꽣 媛??룞 ?쐞?븳 ?옞湲? ?옣移? 媛??룞 ?떆  ?닔吏? ?젅踰?
        if (INTERACTIVE_TYPE::UNLOCKGEAR == m_EventInteract.eInteractType)
        {
            UnLockGear_Event(fTimeDelta);
        }
        // ?뿕由щ쿋?씠?꽣 媛??룞 ?쐞?븳 ?옞湲? ?옣移? 媛??룞 ?떆
        if (INTERACTIVE_TYPE::GIANTGATE == m_EventInteract.eInteractType)
        {
            GiantGate_Event(fTimeDelta);
        }
        // NPC ?옉 ?긽?샇 ?옉?슜 ?떆
        if (true == m_EventInteract.isNPC())
        {
            NPC_Event(fTimeDelta);
        }
        // ?궗?떎由щ옉 ?긽?샇 ?옉?슜 ?떆
        if (INTERACTIVE_TYPE::LADDER == m_EventInteract.eInteractType)
        {
            Ladder_Event(fTimeDelta);
        }
        // 洹??꽍?씠?옉 ?긽?샇 ?옉?슜 ?떆
        if (INTERACTIVE_TYPE::DESTINYSTONE == m_EventInteract.eInteractType)
        {
            m_EventInteract.End_Event();
        }
    }

    /* ?닔吏? ?젅踰? ?룞湲고솕 ?떆 */
    if (m_isInteractEventStart)
    {
        _uint iCurUnArmedAnimIndex{};
        if (Has_Status(SPEAR)) iCurUnArmedAnimIndex = m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_UnArmed");
        else  iCurUnArmedAnimIndex =m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_UnArmed");

        if (m_pBody->Get_Model()->IsAnimationStart(iCurUnArmedAnimIndex) && *m_pBody->Get_Model()->Get_CurTrackPosition() >= 25.f) {
            m_pBody->Get_Model()->AnimationSetIndexIncrease();
            m_isInteractEventStart = false;
        }

    }

    /* ?궗?떎由? ?걹?궓 */
    if (m_isLadderEndEvent)
    {
        if (!m_isLadderRotationEvent && m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_ClimbDn_D_End_Start")) && m_pBody->Get_Model()->Check_MinAnimationTime())
        {
            cout << "=============="<< * m_pBody->Get_Model()->Get_CurTrackPosition() << endl;
            m_isLadderRotationEvent = true;
            m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(0.f));
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed"));
            cout << "  =============  Set_AnimationBlend  false " << endl;
        }

        //if (!m_isLadderRotationEvent && m_pBody->Get_Model()->Get_CurAnimIndex() == m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Stand"))
        //{
        //    m_pBody->Get_Model()->Set_AnimationBlend(false);
        //    m_isLadderRotationEvent = true;
        //    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
        //    cout << "  =============  Set_AnimationBlend  false " << endl;
        //}

        if (m_isLadderRotationEvent && m_pBody->Get_Model()->IsAnimationStart(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed")) &&/* m_pBody->Get_Model()->Check_MinAnimationTime()*/ *m_pBody->Get_Model()->Get_CurTrackPosition() >16.f)
        {
 
            m_isLadderEndEvent = false;
            m_isLadderRotationEvent = false;

        }
        
    }
}

void CKhazan_GSword::BladeNexus_Event(_float fTimeDelta)
{
    EventBladeNexus BNEvent = m_EventInteract.BNEvent;

    // 洹?寃??뿉 ?젒珥? ?썑 ?긽?샇 ?옉?슜 ( 洹?寃? 媛??룞 )
    if (false == BNEvent.isBNOpened)
    {

        // 洹?寃? 泥? ?빐湲? ?떆
        if (true == BNEvent.isUnLock)
        {
            // 泥? ?빐湲? ?뵆?젅?씠?뼱    ?븷?땲硫붿씠?뀡 ?옱?깮 
            if (m_pAnimInteraction->Try_DamagedTS_Before(Has_Status(GSWORD | SPEAR)))
            {
                //if (Has_Status(GSWORD | SPEAR))
                  //  m_pBody->Get_Model()->AnimationSetIndexIncrease();
                Clear_State();
                Clear_SubState();
                Clear_CycleState();
                //m_isInteractEventStart = true;

                m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;
            }
        }
        // ?씠誘? ?빐湲덈맂 洹?寃?
        else if (false == BNEvent.isUnLock)
        {
            // ?빐湲덈맂 洹?寃? ?뵆?젅?씠?뼱 ?븷?땲硫붿씠?뀡 ?옱?깮
            if (m_pAnimInteraction->Try_DamagedTS_After(Has_Status(GSWORD | SPEAR)))
            {
               // if (Has_Status(GSWORD | SPEAR))
                  //  m_pBody->Get_Model()->AnimationSetIndexIncrease();
                Clear_State();
                Clear_SubState();
                Clear_CycleState();
               // m_isInteractEventStart = true;

                m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;
            }
        }

        // ?뵆?젅?씠?뼱 Look -> 洹?寃? ( 湲곗슦?뒗嫄? 蹂댁젙?븯?젮怨? ?씠?젃寃? 肄붾뱶 ?꽔?뼱?넧?뒿?땲?떎. )
        BNEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&BNEvent.vPosition));
        m_pClientInstance->Start_ForceOrbit(CAMERA_FORCE_DIR::FRONT);
    }
    // 洹?寃? 媛??룞 ?걹?굹怨? UI ?뙘?뾽 ( 洹?寃? UI 李? ?솢?꽦?솕 ) ( ?뵆?젅?씠?뼱?뒗 LOOP ?븷?땲硫붿씠?뀡 )
    else if (true == BNEvent.isBNOpened)
    {
        // 洹?寃? 泥? ?빐湲? ?떆
        if (true == BNEvent.isUnLock)
        {
            // 泥? ?빐湲? ?뵆?젅?씠?뼱 洹?寃? LOOP Animation?
        }
        // ?씠誘? ?빐湲덈맂 洹?寃?
        else if (false == BNEvent.isUnLock)
        {
            // ?빐湲? ?맂 ?뵆?젅?씠?뼱 洹?寃? LOOP Animation?
        }
    }

    m_EventInteract.End_Event();
}

void CKhazan_GSword::Chest_Event(_float fTimeDelta)
{
    EventChest ChestEvent = m_EventInteract.ChestEvent;

    // ?긽?옄?뿉 ?젒珥? ?썑 ?긽?샇 ?옉?슜 ( ?떕?엺 ?긽?깭 )
    if (false == ChestEvent.isChestOpened)
    {
        /* ?븷?땲硫붿씠?뀡 ?옱?깮 */
        if (m_pAnimInteraction->Try_BoxOpen(Has_Status(GSWORD | SPEAR)))
        {
            //m_pBody->Get_Model()->AnimationSetIndexIncrease();
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
        }

        ChestEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // ?뵆?젅?씠?뼱 Look -> ?긽?옄, Position ?긽?옄 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&ChestEvent.vPlayerPosition));
        ChestEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&ChestEvent.vPosition));

        m_EventInteract.End_Event();
    }
    // ?긽?옄 ?뿴由щ뒗 ?븷?땲硫붿씠?뀡 醫낅즺?릺硫? ( ?뿴由? ?긽?깭 )
    else if (true == ChestEvent.isChestOpened)
    {
        m_fEventTimeAcc += fTimeDelta;

        if (0.2f <= m_fEventTimeAcc)
        {
            switch (m_sNextItem)
            {
            case 0:
                if (0 != ChestEvent.Items.iItem_0)
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(ChestEvent.Items.iItem_0);
                m_sNextItem = 1;
                break;
            case 1:
                if (0 != ChestEvent.Items.iItem_1)
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(ChestEvent.Items.iItem_1);
                m_sNextItem = 2;
                break;
            case 2:
                if (0 != ChestEvent.Items.iItem_2)
                    static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(ChestEvent.Items.iItem_2);
                m_sNextItem = 0;

                ZeroMemory(&ChestEvent.Items, sizeof(BOX_ITEMS));
                m_EventInteract.End_Event();
                break;
            }

            m_fEventTimeAcc = 0.f;
        }

    }
}

void CKhazan_GSword::TombStone_Event(_float fTimeDelta)
{
    EventTombStone TSEvent = m_EventInteract.TSEvent;

    // ?댘?뒪?넠?뿉 ?젒珥? ?썑 ?긽?샇 ?옉?슜 ( ?댘?뒪?넠 媛??룞 )
    if (false == TSEvent.isTSOpened)
    {
        TSEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // ?뵆?젅?씠?뼱 Look -> ?댘?뒪?넠 ( 湲곗슦?뒗嫄? 蹂댁젙?븯?젮怨? ?씠?젃寃? 肄붾뱶 ?꽔?뼱?넧?뒿?땲?떎. )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&TSEvent.vPlayerPosition));
        TSEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&TSEvent.vPosition));
    }
    // ?댘?뒪?넠 媛??룞 ?걹?굹怨? 媛??룞 LOOP 吏꾩엯
    else if (true == TSEvent.isTSOpened)
    {
        m_pCharVirCom->Teleport(XMLoadFloat4(&TSEvent.vPlayerTPPos), m_pTransformCom->Get_Rotation_Quat(), m_pTransformCom);
    }

    m_EventInteract.End_Event();
}
void CKhazan_GSword::Lever_Event(_float fTimeDelta)
{
    EventLever LeverEvent = m_EventInteract.LeverEvent;

    // ?젅踰꾧?? Active 濡? ?쟾?솚 以묒씪 ?븣
    if (EventLever::ACTIVE == LeverEvent.eState)
    {
        /* ?븷?땲硫붿씠?뀡 ?옱?깮 */
        if (m_pAnimInteraction->Try_Lever(Has_Status(GSWORD | SPEAR)))
        {
            //m_pBody->Get_Model()->AnimationSetIndexIncrease();
            Clear_State();
            Clear_SubState();
            Clear_CycleState();
            //m_isInteractEventStart = true;
        }

        LeverEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&LeverEvent.vPlayerPosition));
        LeverEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&LeverEvent.vPosition));
    }
    // ?젅踰꾧?? DeActive 濡? ?쟾?솚 以묒씪 ?븣
    else if (EventLever::DEACTIVE == LeverEvent.eState)
    {
        // ?뵆?젅?씠?뼱媛? ?젅踰꾨?? DeActive ?떆?궎?뒗 ?븷?땲硫붿씠?뀡 ?옱?깮

        LeverEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&LeverEvent.vPlayerPosition));
        LeverEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&LeverEvent.vPosition));
    }

    m_EventInteract.End_Event();
}
void CKhazan_GSword::Statue_Event(_float fTimeDelta)
{
    EventStatue StatueEvent = m_EventInteract.StatueEvent;

    /* ?븷?땲硫붿씠?뀡 ?옱?깮 */
    if (m_pAnimInteraction->Try_Statue(false))
    {
        Clear_State();
        Clear_SubState();
        Clear_CycleState();
    }


    StatueEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&StatueEvent.vPlayerPosition));
    StatueEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    m_pTransformCom->LookAt(XMLoadFloat4(&StatueEvent.vPosition));

    m_EventInteract.End_Event();
}
void CKhazan_GSword::IronGate_Event(_float fTimeDelta)
{
    EventIronGate IronGateEvent = m_EventInteract.IronGateEvent;

    /* ?븷?땲硫붿씠?뀡 ?옱?깮 */
    if (m_pAnimInteraction->Try_IronGate(Has_Status(GSWORD | SPEAR)))
    {
       // m_pBody->Get_Model()->AnimationSetIndexIncrease();
        Clear_State();
        Clear_SubState();
        Clear_CycleState();
    }


    IronGateEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&IronGateEvent.vPlayerPosition));
    IronGateEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    m_pTransformCom->LookAt(XMLoadFloat4(&IronGateEvent.vPosition));

    m_EventInteract.End_Event();
}
void CKhazan_GSword::UnLockGear_Event(_float fTimeDelta)
{
    EventUnLockGear ULGearEvent = m_EventInteract.UnLockGearEvent;

    /* ?븷?땲硫붿씠?뀡 ?옱?깮 */
    if (m_pAnimInteraction->Try_UnLockGear(Has_Status(GSWORD | SPEAR)))
    {
        //m_pBody->Get_Model()->AnimationSetIndexIncrease();
        Clear_State();
        Clear_SubState();
        Clear_CycleState();
       // m_isInteractEventStart = true;
    }


    ULGearEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&ULGearEvent.vPlayerPosition));
    ULGearEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    m_pTransformCom->LookAt(XMLoadFloat4(&ULGearEvent.vPosition));

    m_EventInteract.End_Event();
}
void CKhazan_GSword::GiantGate_Event(_float fTimeDelta)
{
    EventGiantGate GateEvent = m_EventInteract.GiantGateEvent;

    /* ?븷?땲硫붿씠?뀡 ?옱?깮 */
    if (m_pAnimInteraction->Try_GiantGate(Has_Status(GSWORD | SPEAR)))
    {
        //m_pBody->Get_Model()->AnimationSetIndexIncrease();
        Clear_State();
        Clear_SubState();
        Clear_CycleState();
       // m_isInteractEventStart = true;
    }


    GateEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&GateEvent.vPlayerPosition));
    GateEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    m_pTransformCom->LookAt(XMLoadFloat4(&GateEvent.vPosition));

    m_EventInteract.End_Event();
}
void CKhazan_GSword::NPC_Event(_float fTimeDelta)
{
    EventNPC NPCEvent = m_EventInteract.NPCEvent;

    if (Has_Status(SPEAR))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Armed"));
    if (Has_Status(GSWORD))
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_GSword_Armed"));

    NPCEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
    m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&NPCEvent.vPlayerPosition));
    NPCEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
    m_pTransformCom->LookAt(XMLoadFloat4(&NPCEvent.vPosition));

    m_EventInteract.End_Event();
}
void CKhazan_GSword::Ladder_Event(_float fTimeDelta)
{
    EventLadder LadderEvent = m_EventInteract.LadderEvent;

    switch (LadderEvent.eLadderState)
    {
    case EventLadder::LADDER_ACTION::UPTODOWN:
    {
        m_pCharVirCom-> Begin_Ladder();
        m_pAnimLadder->Try_Start_Down_Ladder(3);
        //if (m_pAnimLadder->Try_Start_Down_Ladder(3))
        //    m_isInteractEventStart = true;
        Clear_Step0();
        Add_Status(BLOCK_ATK_SKILL_GUARD | LADDER_CLIMBING | LADDER_CLIMBING_ROTATION);

        break;
    }
    case EventLadder::LADDER_ACTION::DOWNTOUP:
    {
        m_pCharVirCom->Begin_Ladder();
        m_pAnimLadder->Try_Start_Down_Ladder(3);
        //if (m_pAnimLadder->Try_Start_Down_Ladder(3))
        //    m_isInteractEventStart = true;
        //m_pAnimLadder->Try_Start_Up_Ladder();
        Clear_Step0();
        Add_Status(BLOCK_ATK_SKILL_GUARD | LADDER_CLIMBING| LADDER_CLIMBING_ROTATION);

        break;
    }
    case EventLadder::LADDER_ACTION::UPEND:
    {
        m_pCharVirCom->End_Ladder();
        m_pAnimLadder->Force_End_Down_Ladder();
        Clear_Step0();
        Remove_Status(BLOCK_ATK_SKILL_GUARD | LADDER_CLIMBING_ROTATION);
        Add_Status(LADDER_CLIMBING_END);
        m_pClientInstance->Set_PlayerInput(false);
        m_isLadderEndEvent = true;
        m_isLadderRotationEvent = false;

        //m_pCharVirCom->End_Ladder();
        //m_pAnimLadder->Force_End_Up_Ladder();
        //Clear_Step0();
        //Remove_Status(BLOCK_ATK_SKILL_GUARD  | LADDER_CLIMBING_ROTATION);
        //Add_Status(LADDER_CLIMBING_END);
        //m_pClientInstance->Set_PlayerInput(false);
        //m_isLadderEndEvent = true;
        break;

    }
    case EventLadder::LADDER_ACTION::DOWNEND:
    {
        m_pCharVirCom->End_Ladder();
        m_pAnimLadder->Force_End_Down_Ladder();
        Clear_Step0();
        Remove_Status(BLOCK_ATK_SKILL_GUARD | LADDER_CLIMBING_ROTATION);
        Add_Status(LADDER_CLIMBING_END);
        m_pClientInstance->Set_PlayerInput(false);
        m_isLadderEndEvent = true;
        m_isLadderRotationEvent = false;

        break;
    }
    default:
        MSG_BOX(TEXT("?씘 ?궗?떎由? ?씠?윴嫄? ?뾾?뒗?뵒"));
        break;
    }

    if (true == LadderEvent.isStartAction())
    {
        LadderEvent.vPlayerPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1] + 0.1f ; //?뵆?젅?씠?뼱媛? ?씠?룞?븷 ?쐞移? y 

        // ?뵆?젅?씠?뼱 Look -> ?젅踰?, Position ?젅踰? 蹂? ?쐞移섎줈 ?씠?룞 ( 湲곗슦?뒗嫄? 蹂댁젙 )
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&LadderEvent.vPlayerPosition));  //?닚媛꾩씠?룞
        LadderEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1]; // ?젅?뜑 ?쐞移? 
        m_pTransformCom->LookAt(XMLoadFloat4(&LadderEvent.vPosition)); //瑗щ씪蹂닿린 
    }

    m_EventInteract.End_Event();
}
void CKhazan_GSword::Lerp_Position_ByInteractEvent(_float4 vTargetPos, _float4 vStartPos, _float fDuration, _float fTimeDelta, _bool& isDone)
{
    _float4 vPos = vTargetPos;

    // y媛? 蹂댁젙
    vPos.y = vStartPos.y;

    m_fLerpTime_Event += fTimeDelta;

    _float fLerpTime = m_fLerpTime_Event / fDuration;

    _float4 vLerpPos = Lerp(vStartPos, vPos, fLerpTime);

    _float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vPos) - XMLoadFloat4(&vLerpPos)));

    if (0.1f > fDistance)
    {
        isDone = true;
    }
    else
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMLoadFloat4(&vLerpPos));
        vPos.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMLoadFloat4(&vPos));
    }
}
#pragma endregion

#ifdef _DEBUG

void CKhazan_GSword::Debug_Widget()
{
    m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

        ImGui::Begin("Khazan GSWORD Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::BeginTabBar("DebugTabs"))
        {
            if (ImGui::BeginTabItem("States"))
            {
                Debug_Widget_States();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Combat"))
            {
                Debug_Widget_Combat();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Animation"))
            {
                Debug_Widget_Animation();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Movement"))
            {
                Debug_Widget_Movement();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Guard Debug"))
            {
                Debug_Widget_Guard();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
        });
}

void CKhazan_GSword::Debug_Widget_States()
{
    ImGui::SeparatorText("State Flags");

    // === Main State ===
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Main State (0x%08X)", m_iCurMainState);
    ImGui::Indent();

    auto StateFlag = [this](const char* label, _uint flag, ImVec4 color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f)) {
        bool isActive = (m_iCurMainState & flag) != 0;
        if (isActive)
            ImGui::TextColored(color, "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    StateFlag("DIE", CAT::M_DIE, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    StateFlag("FALL", CAT::M_FALL);
    StateFlag("GROGGY", CAT::M_GROGGY);
    StateFlag("DAMAGED", CAT::M_DAMAGED, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
    StateFlag("CLIMB", CAT::M_CLIMB);
    StateFlag("SKILL", CAT::M_SKILL, ImVec4(0.5f, 0.0f, 1.0f, 1.0f));
    StateFlag("GUARD", CAT::M_GUARD, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
    StateFlag("ATTACK", CAT::M_ATTACK, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    StateFlag("MOVE", CAT::M_MOVE, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    StateFlag("LOCKON", CAT::M_LOCKON);
    StateFlag("INTERACT", CAT::M_INTERACT);
    StateFlag("WEAPON_CHANGE", CAT::M_WEAPON_CHANGE);
    StateFlag("IDLE", CAT::M_IDLE);

    ImGui::Unindent();
    ImGui::Separator();

    // === Sub State ===
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Sub State (0x%08X)", m_iCurSubState);
    ImGui::Indent();

    auto SubStateFlag = [this](const char* label, _uint flag) {
        bool isActive = (m_iCurSubState & flag) != 0;
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    // Movement
    ImGui::Text("Movement:");
    ImGui::Indent();
    SubStateFlag("Walk", MOV::MOVE_WALK);
    SubStateFlag("Run", MOV::MOVE_RUN);
    SubStateFlag("Sprint", MOV::MOVE_SPRINT);
    SubStateFlag("Climb", MOV::MOVE_CLIMB);
    SubStateFlag("Mirage Step", MOV::MOVE_MIRAGE_STEP);
    SubStateFlag("GetUp", MOV::MOVE_GETUP);
    SubStateFlag("Fall", MOV::MOVE_FALL);
    SubStateFlag("Dodge", MOV::MOVE_DODGE);
    SubStateFlag("Injured", MOV::MOVE_INJURED);
    ImGui::Unindent();

    // Attack
    ImGui::Text("Attack:");
    ImGui::Indent();
    SubStateFlag("Fall", ATT::ATK_FALL);
    SubStateFlag("Fast", ATT::ATK_FAST);
    SubStateFlag("Grapple", ATT::ATK_GRAPPLE);
    SubStateFlag("Skill", ATT::ATK_SKILL);
    SubStateFlag("Counter", ATT::ATK_COUNTER);
    SubStateFlag("Dodge Attack", ATT::ATK_DODGEATK);
    SubStateFlag("Reflection", ATT::ATK_REFLECTION);
    SubStateFlag("Sprint Attack", ATT::ATK_SPRINTATK);
    SubStateFlag("Strong", ATT::ATK_STRONG);
    SubStateFlag("Javelin", ATT::ATK_JAVELIN);
    SubStateFlag("Charge", ATT::ATK_CHARGE);

    ImGui::Unindent();

    // Skill
    ImGui::Text("Skill:");
    ImGui::Indent();
    SubStateFlag("Assault", SKI::ASSAULT);
    SubStateFlag("Full Moon", SKI::FULL_MOON);
    SubStateFlag("Moonlight Slash", SKI::MOONLIGHT_SLASH);
    SubStateFlag("Spiral Thrust", SKI::SPIRAL_THRUST);
    ImGui::Unindent();

    ImGui::Unindent();
    ImGui::Separator();

    // === Cycle State ===
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Cycle (0x%08X)", m_iCycle);
    ImGui::Indent();

    auto CycleFlag = [this](const char* label, _uint flag) {
        bool isActive = (m_iCycle & flag) != 0;
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    CycleFlag("Start", CYC::CYCLE_START);
    CycleFlag("Loop", CYC::CYCLE_LOOP);
    CycleFlag("End", CYC::CYCLE_END);
    CycleFlag("Break", CYC::CYCLE_BREAK);
    CycleFlag("Break", CYC::CYCLE_BREAK);
    if (m_isGhost)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", "Ghost");
    else
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", "Ghost");
    ImGui::Unindent();
    ImGui::Separator();

    // === Status ===
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Status (0x%08X)", m_iStatus);
    ImGui::Indent();

    auto StatusFlag = [this](const char* label, _uint flag) {
        bool isActive = (m_iStatus & flag) != 0;
        if (isActive)
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] %s", label);
        else
            ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] %s", label);
        };

    StatusFlag("Barehand", BAREHAND);
    StatusFlag("Spear", SPEAR);
    StatusFlag("GSWORD", GSWORD);
    StatusFlag("Reserved", RESERVED);
    StatusFlag("Charging Sprint", CHARGING_SPRINT);
    StatusFlag("Back Dodge", BACK_DODGE);
    StatusFlag("Rotation", ROTATION);
    StatusFlag("Charging Fast Attack", CHARGING_FAST_ATTACK);
    StatusFlag("Sprint Again Request", SPRINT_AGAIN_REQUEST);
    StatusFlag("LockOn", LOCKON);
    StatusFlag("Charging Strong Attack", CHARGING_STRONG_ATTACK);
    StatusFlag("Dodging", DODGING);
    StatusFlag("Guard", GUARD);
    StatusFlag("Guard Success", GUARD_SUCCESS);
    StatusFlag("Just Guard", JUST_GUARD);
    StatusFlag("Guard Rotation", GUARD_ROTATION);
    StatusFlag("Guard Rotation Request", GUARD_ROTATION_REQUEST);
    StatusFlag("Falling", FALLING);
    StatusFlag("Pre Land", PRE_LAND);
    StatusFlag("Falling Attack", FALLING_ATTACK);
    StatusFlag("Stamina Recovery", STAMINA_RECOVERY);
    StatusFlag("Brutal Begin", BRUTAL_BEGIN);
    StatusFlag("Brutal Ready", BRUTAL_READY);
    StatusFlag("Brutal Success", BRUTAL_SUCCESS);
    StatusFlag("Statue Mode", STATUE_MODE);
    StatusFlag("Block Atk Skill Guard", BLOCK_ATK_SKILL_GUARD);
    StatusFlag("Stamina Exhaustion", STAMINA_EXHAUSTION);
    StatusFlag("DODGE ENDING", DODGE_ENDING);
    StatusFlag("Ladder Climbing", LADDER_CLIMBING);
    StatusFlag("Ladder Climbing Rotation", LADDER_CLIMBING_ROTATION);
    StatusFlag("Ladder Climbing End", LADDER_CLIMBING_END);
    StatusFlag("Ladder Sprint", LADDER_SPRINT);


    //StatusFlag("TURN180", TURN180);
    //StatusFlag("TURN180_REQUESTED", TURN180_REQUESTED);
    //StatusFlag("TURN180_COMPLETE", TURN180_COMPLETE);
    //StatusFlag("MOVE_AFTER_TURN", MOVE_AFTER_TURN);

    ImGui::Unindent();
    ImGui::Separator();

    // === Direction ===
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Direction");

    ImGui::Text("Player Dir: %s", GetDirectionString(m_eDir).c_str());
    ImGui::Text("World Dir: %s", GetDirectionString(m_eWorldDir).c_str());

    // Direction Visualization
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(120, 120);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(canvas_pos.x + canvas_size.x * 0.5f, canvas_pos.y + canvas_size.y * 0.5f);
    float radius = 50.0f;

    draw_list->AddCircle(center, radius, IM_COL32(100, 100, 100, 255), 32, 2.0f);

    using DIR = DIRECTION_INFO::DIR;
    if (m_eDir.Check_Flag(DIR::F))
        draw_list->AddLine(center, ImVec2(center.x, center.y - radius), IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::B))
        draw_list->AddLine(center, ImVec2(center.x, center.y + radius), IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::L))
        draw_list->AddLine(center, ImVec2(center.x - radius, center.y), IM_COL32(0, 255, 0, 255), 3.0f);
    if (m_eDir.Check_Flag(DIR::R))
        draw_list->AddLine(center, ImVec2(center.x + radius, center.y), IM_COL32(0, 255, 0, 255), 3.0f);

    draw_list->AddCircleFilled(center, 5.0f, IM_COL32(255, 255, 0, 255));

    ImGui::Dummy(canvas_size);
}

void CKhazan_GSword::Debug_Widget_Combat()
{
    ImGui::SeparatorText("Combat Stats");

    // HP
    ImGui::Text("Health");
    ImGui::ProgressBar(m_pPlayerData->fCulHp / max(m_pPlayerData->fMaxHp, 0.001f), ImVec2(-1, 0),
        (std::to_string((_int)m_pPlayerData->fCulHp) + " / " + std::to_string((_int)m_pPlayerData->fMaxHp)).c_str());

    ImGui::SliderFloat("Current HP", &m_pPlayerData->fCulHp, 0.0f, m_pPlayerData->fMaxHp, "%.1f");
    ImGui::InputFloat("Max HP", &m_pPlayerData->fMaxHp, 0, 0, "%.0f");

    ImGui::Separator();

    // Stamina
    ImGui::Text("Stamina");
    ImGui::ProgressBar(m_pPlayerData->fCulStamina / max(m_pPlayerData->fMaxStamina, 0.001f), ImVec2(-1, 0),
        (std::to_string((_int)m_pPlayerData->fCulStamina) + " / " + std::to_string((_int)m_pPlayerData->fMaxStamina)).c_str());

    ImGui::SliderFloat("Current Stamina", &m_pPlayerData->fCulStamina, 0.0f, m_pPlayerData->fMaxStamina, "%.1f");
    ImGui::InputFloat("Max Stamina", &m_pPlayerData->fMaxStamina, 0, 0, "%.0f");

    ImGui::Separator();

    // Attack
    ImGui::InputFloat("Attack Power", &m_pPlayerData->fBonusDamage, 0, 0, "%.1f");

    ImGui::Separator();

    // Hit Reaction
    ImGui::Text("Hit Reaction: %s", GetHitReactionString().c_str());

    ImGui::Text("Hit Normal Dir: %s", GetDirectionString(m_eHitNormalDir).c_str());
    ImGui::Text("Hit Strong Dir: %s", GetDirectionString(m_eHitStrongDir).c_str());

    ImGui::Separator();

    // Quick Actions
    if (ImGui::Button("Restore HP", ImVec2(-1, 0)))
        m_pPlayerData->fCulHp = m_pPlayerData->fMaxHp;

    if (ImGui::Button("Restore Stamina", ImVec2(-1, 0)))
        m_pPlayerData->fCulStamina = m_pPlayerData->fMaxStamina;
}

void CKhazan_GSword::Debug_Widget_Animation()
{
    ImGui::SeparatorText("Animation Info");

    if (!m_pBody || !m_pBody->Get_Model())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Model not available");
        return;
    }

    CModel* pModel = m_pBody->Get_Model();

    // Current Animation
    ImGui::Text("Current Anim Index: %d", pModel->Get_CurAnimIndex());
    ImGui::Text("Reserved Anim Index: %d", m_iReserveAnimIndex);

    ImGui::Separator();

    // Animation Progress
    float trackPos = *pModel->Get_CurTrackPosition();
    float duration = pModel->Get_CurDuration();

    ImGui::Text("Track Position");
    ImGui::ProgressBar(trackPos / max(duration, 0.001f), ImVec2(-1, 0),
        (std::to_string((_int)trackPos) + " / " + std::to_string((_int)duration)).c_str());

    ImGui::Separator();

    // Animation State
    ImGui::Text("Is Finished: %s", m_pBody->Get_FinishedAnimation() ? "YES" : "NO");
    ImGui::Text("Min Time Passed: %s", pModel->Check_MinAnimationTime() ? "YES" : "NO");

    ImGui::Separator();

    // Animation System States
    if (m_pAnimMove)
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Move Animation");
        ImGui::Indent();
        ImGui::Text("Selected Index: %d", m_pAnimMove->Get_AnimationIndex());
        ImGui::Text("Is Finished: %s", m_pAnimMove->Is_Finished() ? "YES" : "NO");
        ImGui::Text("Is Dodging: %s", m_pAnimMove->IsDodge() ? "YES" : "NO");
        ImGui::Text("Is Moving: %s", m_pAnimMove->IsMoving() ? "YES" : "NO");
        ImGui::Unindent();
    }

    if (m_pAnimAttack)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Attack Animation");
        ImGui::Indent();
        ImGui::Text("Is Attacking: %s", m_pAnimAttack->Is_Attacking() ? "YES" : "NO");
        ImGui::Text("Can Next Combo: %s", m_pAnimAttack->Can_NextCombo() ? "YES" : "NO");
        ImGui::Text("Current Combo: %d", m_pAnimAttack->Get_CurrentCombo());
        ImGui::Text("Is Reserved: %s", m_pAnimAttack->Is_Reserve() ? "YES" : "NO");
        ImGui::Unindent();
    }

    if (m_pAnimGuard)
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Guard Animation");
        ImGui::Indent();
        ImGui::Text("Is Guarding: %s", m_pAnimGuard->Is_Guarding() ? "YES" : "NO");
        ImGui::Text("Is Walk Guarding: %s", m_pAnimGuard->Is_WalkGuarding() ? "YES" : "NO");
        ImGui::Text("Is Just Guarding: %s", m_pAnimGuard->Is_JustGuarding() ? "YES" : "NO");
        ImGui::Unindent();
    }

    if (m_pAnimDamaged)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Damaged Animation");
        ImGui::Indent();
        ImGui::Text("Is Damaged: %s", m_pAnimDamaged->Is_Damaged() ? "YES" : "NO");
        ImGui::Unindent();
    }
}

void CKhazan_GSword::Debug_Widget_Movement()
{
    ImGui::SeparatorText("Movement Control");

    // Enable/Disable Control
    if (ImGui::Button(m_isEnableControl ? "Disable Control" : "Enable Control", ImVec2(-1, 30)))
    {
        m_isEnableControl = !m_isEnableControl;
    }
    if (m_pClientInstance->Get_PlayerInput())
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[ok] Player Input");
    else
        ImGui::TextColored(ImVec4(0.4f, 0.4f, 0.4f, 1.0f), "[ ] Player Input");

    ImGui::Separator();

    //// Speed Settings
    ImGui::DragFloat("Walk Speed", &m_fWalkSpeed, 0.1f, 0.f, 20.f);
    ImGui::DragFloat("Run Speed", &m_fRunSpeed, 0.1f, 0.f, 30.f);
    ImGui::DragFloat("Sprint Speed", &m_fSprintSpeed, 0.1f, 0.f, 50.f);
    ImGui::DragFloat("Injured Speed", &m_fInjuredSpeed, 0.1f, 0.f, 10.f);

    //ImGui::Separator();

    // Rotation Settings
    ImGui::Text("Rotation Time: %.2f / %.2f", m_fRotateTime[0], m_fRotateTime[1]);
    ImGui::DragFloat("Rotation Duration", &m_fRotateTime[1], 0.01f, 0.01f, 1.0f);

    ImGui::Separator();

    // Teleport
    static _float teleportPos[3] = { 516.f, -11.f, 264.f };
    ImGui::DragFloat3("Teleport Position YETUGA", teleportPos, 0.1f);

    if (ImGui::Button("Teleport", ImVec2(-1, 0)))
    {
        m_pTransformCom->Set_State(STATE::POSITION,
            XMVectorSet(teleportPos[0], teleportPos[1], teleportPos[2], 1.f));
    }

    static _float teleportPos2[3] = { 114.64f, 5.2f, 99.f };
    ImGui::DragFloat3("Teleport Position HeinMach Low Cliff", teleportPos2, 0.1f);

    if (ImGui::Button("Teleport", ImVec2(-1, 0)))
    {
        m_pTransformCom->Set_State(STATE::POSITION,
            XMVectorSet(teleportPos2[0], teleportPos2[1], teleportPos2[2], 1.f));
    }

    ImGui::Separator();

    // Key Bindings
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Key Bindings");
    ImGui::Columns(2);
    ImGui::Text("Movement"); ImGui::NextColumn(); ImGui::Text("W/A/S/D"); ImGui::NextColumn();
    ImGui::Text("Walk"); ImGui::NextColumn(); ImGui::Text("LALT + WASD"); ImGui::NextColumn();
    ImGui::Text("Dodge"); ImGui::NextColumn(); ImGui::Text("SPACE (Tap)"); ImGui::NextColumn();
    ImGui::Text("Sprint"); ImGui::NextColumn(); ImGui::Text("SPACE (Hold)"); ImGui::NextColumn();
    ImGui::Text("Guard"); ImGui::NextColumn(); ImGui::Text("LSHIFT"); ImGui::NextColumn();
    ImGui::Text("Attack"); ImGui::NextColumn(); ImGui::Text("LMB/RMB"); ImGui::NextColumn();
    ImGui::Columns(1);
}

void CKhazan_GSword::Debug_Widget_Guard()
{
    ImGui::SeparatorText("Guard Debug Info");

    if (!m_pAnimGuard)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Guard Animation not available");
        return;
    }

    if (!m_pBody)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Body not available");
        return;
    }

    // Guard States
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Guard States");
    ImGui::Separator();

    bool isGuarding = m_pAnimGuard->Is_Guarding();
    bool isWalkGuarding = m_pAnimGuard->Is_WalkGuarding();
    bool isJustGuarding = m_pAnimGuard->Is_JustGuarding();

    ImGui::Text("Is Guarding: ");
    ImGui::SameLine();
    if (isGuarding)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "TRUE");
    else
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "FALSE");

    ImGui::Text("Is Walk Guarding: ");
    ImGui::SameLine();
    if (isWalkGuarding)
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "TRUE");
    else
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "FALSE");

    ImGui::Text("Is Just Guarding: ");
    ImGui::SameLine();
    if (isJustGuarding)
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "TRUE");
    else
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "FALSE");

    ImGui::Separator();

    // Body Guard Info
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Body Collision States");
    ImGui::Separator();

    // ?뿬湲곗꽌 Body?쓽 ?젙蹂대?? 媛??졇?삱 ?닔 ?엳?떎硫? ?몴?떆
    ImGui::Text("Note: Add Get functions to Body for collision states");

    ImGui::Separator();

    // Guard Rotation Info
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Guard Rotation (Body)");
    ImGui::Separator();

    ImGui::Text("Note: Guard rotation is handled in Body");
    ImGui::Text("Add Get functions to access rotation states");

    ImGui::Separator();

    // Test Buttons
    if (ImGui::Button("Test Start Guard", ImVec2(-1, 0)))
    {
        if (!Has_State(CAT::M_GUARD))
        {
            m_pAnimGuard->Try_Guard(0.f);
            Add_State(CAT::M_GUARD);
        }
    }

    if (ImGui::Button("Test End Guard", ImVec2(-1, 0)))
    {
        if (Has_State(CAT::M_GUARD))
        {
            m_pAnimGuard->Play_FinishGuard();
            Remove_State(CAT::M_GUARD);
        }
    }
}

std::string CKhazan_GSword::GetDirectionString(DIRECTION_INFO dir)
{
    if (dir.iDirFlag == 0) return "NONE";

    std::string result;
    using DIR = DIRECTION_INFO::DIR;

    if (dir.Check_Flag(DIR::F)) result += "F";
    if (dir.Check_Flag(DIR::B)) result += "B";
    if (dir.Check_Flag(DIR::L)) result += "L";
    if (dir.Check_Flag(DIR::R)) result += "R";
    if (dir.Check_Flag(DIR::U)) result += "U";
    if (dir.Check_Flag(DIR::D)) result += "D";

    return result.empty() ? "NONE" : result;
}

std::string CKhazan_GSword::GetHitReactionString()
{
    switch (m_eHitReaction)
    {
    case ENUM_CLASS(HITREACTION::NONE): return "NONE";
    case ENUM_CLASS(HITREACTION::KNOCKBACK_WEAK): return "KNOCKBACK_WEAK";
    case ENUM_CLASS(HITREACTION::KNOCKBACK_NORMAL): return "KNOCKBACK_NORMAL";
    case ENUM_CLASS(HITREACTION::KNOCKBACK_STRONG): return "KNOCKBACK_STRONG";
    case ENUM_CLASS(HITREACTION::PARRY): return "PARRY";
    case ENUM_CLASS(HITREACTION::GRAB): return "GRAB";
    default: return "UNKNOWN";
    }
}


#endif // _DEBUG

void CKhazan_GSword::Set_Idle()
{
    CModel* pModel = m_pBody->Get_Model();

    pModel->Set_Animation(pModel->Get_AnimIndexByName("CA_P_Kazan_GSword_Stand"));
}

CKhazan_GSword* CKhazan_GSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKhazan_GSword* pInstance = new CKhazan_GSword(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_GSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKhazan_GSword::Clone(void* pArg)
{
    CKhazan_GSword* pInstance = new CKhazan_GSword(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CKhazan_GSword"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_GSword::Free()
{
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), m_iInteractTypeEventID);
    m_pGameInstance->Unsubscribe_Event(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), m_iObjectInteractEventID);
    __super::Free();

    Safe_Release(m_pClientInstance);
    Safe_Release(m_pCamera);
    Safe_Release(m_pAnimMove);
    Safe_Release(m_pAnimAttack);
    Safe_Release(m_pAnimGuard);
    Safe_Release(m_pAnimInteraction);
    Safe_Release(m_pAnimDamaged);
    Safe_Release(m_pAnimFall);
    Safe_Release(m_pAnimLadder);

    // Safe_Release(m_pCharVirCom);

     //Safe_Release(m_pASMachine);
    // Safe_Release(m_pASManager);
}











#pragma region Try Later

//#include "Khazan_GSword.h"
//#include "GameInstance.h"
//#include "ClientInstance.h"
//
//#include "RigidBody.h"
//#include "CharacterVirtual.h"
//
//#include "Body_Khazan_GS.h"
//#include "Khazan_GS_InputHandler.h"
//#include "Khazan_GS_AnimationController.h"
//
//#include "Camera_Compre.h"
//#include "UI_HUD.h"
//#include "Damage_Text.h"
//
//
//CKhazan_GSword::CKhazan_GSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//    : CCreature{ pDevice, pContext }
//{
//}
//
//CKhazan_GSword::CKhazan_GSword(const CKhazan_GSword& Prototype)
//    : CCreature{ Prototype }
//    , m_pClientInstance{ CClientInstance::GetInstance() }
//{
//    Safe_AddRef(m_pClientInstance);
//}
//
//HRESULT CKhazan_GSword::Initialize_Prototype()
//{
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Initialize_Clone(void* pArg)
//{
//    CREATURE_DESC desc{};
//
//    desc.fAttack = 10.f;
//    desc.fMaxHP = 100.f;
//    desc.fMaxStamina = 100.f;
//    desc.fMoveSpeed = 10.f;
//    desc.fRotationPerSec = XMConvertToRadians(180.f);
//    desc.fSpeedPerSec = 1.f;
//
//
//    if (FAILED(__super::Initialize_Clone(&desc)))
//        return E_FAIL;
//
//    if (FAILED(Ready_Components()))
//        return E_FAIL;
//
//    if (FAILED(Ready_PartObjects()))
//        return E_FAIL;
//
//    if (FAILED(Ready_Collision()))
//        return E_FAIL;
//
//    if (FAILED(Ready_Modules()))
//        return E_FAIL;
//
//    m_pPlayerData = m_pClientInstance->Get_pInitailizePlayerData();
//
//    m_strName = "Khazan";
//
//    return S_OK;
//}
//
//void CKhazan_GSword::Priority_Update(_float fTimeDelta)
//{
//
//    __super::Priority_Update(fTimeDelta);
//
//}
//
//void CKhazan_GSword::Update(_float fTimeDelta)
//{
//    if (!m_isEnableControl || m_isDead)
//        return;
//
//    Update_LockOn();
//
//    // ?엯?젰 ?넂 Command ?깮?꽦
//    Process_Input(fTimeDelta);
//
//    // ?깮?꽑?맂 commands濡? ?뼱?뼡 ?븷?땲硫붿씠?뀡?쓣 ?옱?깮?븷吏? ?젙?븯湲? 
//    Update_State(fTimeDelta);
//
//    // ?븷?땲硫붿씠?뀡 ?떎?뻾 諛? ?긽?깭蹂? 濡쒖쭅 ?떎?뻾?떆?궎湲? (理쒖냼 蹂댁옣 ?떆媛꾩쑝濡? ?쐞?뿉?꽌 ?젙?븳 ?븷?땲硫붿씠?뀡 臾댁떆?븯湲? )
//    Update_Animation(fTimeDelta);
//
//    // ?씠?룞/?쉶?쟾 ?쟻?슜
//    Update_Movement(fTimeDelta);
//
//    // ?쟾?닾 濡쒖쭅( hp / stamina / status 愿?由? / ?꼮諛?..?  )
//    Update_Combat(fTimeDelta);
//
//
//
//
//    __super::Update(fTimeDelta);
//
//}
//
//void CKhazan_GSword::Late_Update(_float fTimeDelta)
//{
//
//
//    __super::Late_Update(fTimeDelta);
//}
//
//HRESULT CKhazan_GSword::Render()
//{
//    return S_OK;
//}
//
//void CKhazan_GSword::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
//{
//}
//
//void CKhazan_GSword::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
//{
//}
//
//void CKhazan_GSword::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
//{
//}
//
//void CKhazan_GSword::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
//{
//}
//
//void CKhazan_GSword::Set_Camera(CCamera_Compre* pCamera)
//{
//    m_pCamera = pCamera;
//    Safe_AddRef(m_pCamera);
//}
//
//void CKhazan_GSword::Set_Position(_float4 vPos)
//{
//}
//
//
//void CKhazan_GSword::Process_Input(_float fTimeDelta)
//{
//    if (!m_pClientInstance->Get_PlayerInput())
//        return;
//
//    m_pInputHandler->Update(fTimeDelta);
//
//}
//
//void CKhazan_GSword::Update_State(_float fTimeDelta)
//{
//
//    m_pStateMachine->Update(fTimeDelta);
//
//    m_pStateMachine->Request_StateChange(m_pInputHandler->Get_CommandsThisFrame());
//
//}
//
//void CKhazan_GSword::Update_Animation(_float fTimeDelta)
//{
//    /* ?꽑蹂꾨맂 而ㅻ㎤?뱶 ?꽆寃⑥＜湲?  */
//    m_pAnimController->Set_Command(m_pStateMachine->Get_SelectedCommand());
//    m_pAnimController->Update(fTimeDelta);
//}
//
//void CKhazan_GSword::Update_Movement(_float fTimeDelta)
//{
//    
//}
//
//void CKhazan_GSword::Update_Combat(_float fTimeDelta)
//{
//}
//
//void CKhazan_GSword::Update_LockOn()
//{
//    // 移대찓?씪?쓽 ?씫?삩 ?긽?깭??? ?룞湲고솕
//    if (m_pCamera)
//        m_isLockOn = m_pCamera->Get_IsLockOn();
//}
//
//void CKhazan_GSword::Handle_StateChanged()
//{
//}
//
//void CKhazan_GSword::Handle_Death()
//{
//}
//
//HRESULT CKhazan_GSword::Ready_Components()
//{
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Ready_PartObjects()
//{
//    LEVEL eCurrentLevel = CClientInstance::GetInstance()->Get_CurrLevel();
//
//    CBody_Khazan_GS::BODY_KHAZAN_GS_DESC         BodyDesc{};
//    //BodyDesc.pState = &m_iCurMainState;
//    //BodyDesc.pStatus = &m_iStatus;
//    //BodyDesc.pIsGuarding = m_pAnimGuard->Get_IsGuarding();
//    //BodyDesc.pHitReation = &m_eHitReaction;
//    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//    //BodyDesc.pGuardRotationTarget = &m_vGuardRotationTarget;
//    BodyDesc.pParentTransform = m_pTransformCom;
//    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(eCurrentLevel), TEXT("Prototype_GameObject_Body_Khazan_GS"), &BodyDesc)))
//        return E_FAIL;
//    m_pBody = static_cast<CBody_Khazan_GS*>(Find_PartObject(TEXT("Part_Body")));
//
//
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Ready_Collision()
//{
//    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
//    _float3 vPos{};
//    _float4 vQuat{};
//    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
//    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
//    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
//    tCharVirDesc.vPos = vPos;
//    tCharVirDesc.vQuat = vQuat;
//    tCharVirDesc.vShapeOffset = _float3(0.f, 0.75f, 0.f);
//    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
//    tCharVirDesc.fRadius = 0.3f;
//    tCharVirDesc.fHeight = 1.f;
//    tCharVirDesc.fMaxSlopeAngle = 45.f;
//    tCharVirDesc.fMass = 60.f;
//    tCharVirDesc.fMaxStrength = 0.f;
//    tCharVirDesc.fPredictiveContactDistance = 0.3f;
//    tCharVirDesc.iMaxConstraintIterations = 20;
//    tCharVirDesc.fCollisionTolerance = 0.03f;
//    tCharVirDesc.fPenetrationRecoverySpeed = 1.7f;
//    m_tPlayerCollisionDesc.pGameObject = this;
//    m_tPlayerCollisionDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
//    m_tPlayerCollisionDesc.strName = TEXT("Khazan_Body");
//    tCharVirDesc.pCollisionDesc = &m_tPlayerCollisionDesc;
//    tCharVirDesc.vStickToFloorStepDown = _float3(0.f, -0.5f, 0);
//    tCharVirDesc.vWalkStairsStepUp = _float3(0.f, 0.5f, 0.f);
//    tCharVirDesc.fWalkStairsMinStepForward = 0.06f;
//    tCharVirDesc.fWalkStairsStepForwardTest = 0.15f;
//
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
//        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CKhazan_GSword::Ready_Modules()
//{
//
//    m_pStateMachine = CKhazan_GS_StateMachine::Create();
//    if (m_pStateMachine == nullptr) return E_FAIL;
//
//    m_pInputHandler = CKhazan_GS_InputHandler::Create();
//    if (m_pInputHandler == nullptr) return E_FAIL;
//
//    m_pAnimController = CKhazan_GS_AnimationController::Create(m_pBody->Get_Model(), &m_iCurWeapon, &m_isLockOn);
//    if (m_pAnimController == nullptr) return E_FAIL;
//
//    //m_pMovementController = CKhazan_GS_MovementController::Create();
//    //if (m_pMovementController == nullptr) return E_FAIL;
//
//    //m_pCombatModule = CKhazan_GS_CombatModule::Create();
//    //if (m_pCombatModule == nullptr) return E_FAIL;
//
//    return S_OK;
//}
//
//
//CKhazan_GSword* CKhazan_GSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//    CKhazan_GSword* pInstance = new CKhazan_GSword(pDevice, pContext);
//
//    if (FAILED(pInstance->Initialize_Prototype()))
//    {
//        MSG_BOX(TEXT("Failed to Created : CKhazan_GSword"));
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//CGameObject* CKhazan_GSword::Clone(void* pArg)
//{
//    CKhazan_GSword* pInstance = new CKhazan_GSword(*this);
//
//    if (FAILED(pInstance->Initialize_Clone(pArg)))
//    {
//        MSG_BOX(TEXT("Failed to Clone : CKhazan_GSword"));
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//void CKhazan_GSword::Free()
//{
//    __super::Free();
//
//    Safe_Release(m_pClientInstance);
//    Safe_Release(m_pCamera);
//
//}

#pragma endregion
