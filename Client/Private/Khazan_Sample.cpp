#include "Khazan_Sample.h"
#include "Body_Khazan_Sample.h"
#include "Spear_Khazan_Sample.h"
#include "GameInstance.h"

#include "RigidBody.h"
#include "CharacterVirtual.h"

#include "Damage_Text.h"
#include "ClientInstance.h"

#pragma region 인벤토리 삽입 테스트
#include "UI_Inven.h"
#pragma endregion

#pragma region 락온, 브루탈어택 테스트
#include "Target_LockOn.h"
#include "Target_BrutalAttack.h"
#pragma endregion

CKhazan_Sample::CKhazan_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCreature{ pDevice, pContext }
{
}

CKhazan_Sample::CKhazan_Sample(const CKhazan_Sample& Prototype)
    : CCreature{ Prototype }
{
}

HRESULT CKhazan_Sample::Initialize_Prototype()
{
    return S_OK;

}

HRESULT CKhazan_Sample::Initialize_Clone(void* pArg)
{
    CREATURE_DESC desc{};

    desc.fAttack = 10.f;
    desc.fMaxHP = 100.f;
    desc.fMaxStamina = 100.f;
    desc.fMoveSpeed = 10.f;
    desc.fRotationPerSec = XMConvertToRadians(180.f);
    desc.fSpeedPerSec = 1.f;

    m_fCurrentHP = 100.f;
    m_fCurrentStamina = 100.f;
    if (FAILED(__super::Initialize_Clone(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    /*if (FAILED(Ready_Collision()))
        return E_FAIL;*/




#ifdef _DEBUG
    Debug_Widget();
#endif // _DEBUG

#pragma region 상호 작용 맵 오브젝트 임시 테스트용
    m_pGameInstance->Subscribe_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), [&](const EventInteractType& e)
        {
            m_EventInteract = e;
        });
#pragma endregion


    // 손잡이와 창날 거리 비율 구하기
    //_vector vSpearMid = XMVectorSet(m_pWeaponR_Matrix->_41, m_pWeaponR_Matrix->_42, m_pWeaponR_Matrix->_43, 0.f);
    //_vector vSpearBlade = XMVectorSet(m_pSpearFX_Matrix->_41, m_pSpearFX_Matrix->_42, m_pSpearFX_Matrix->_43, 0.f);
    //m_fLocalDistBaseRatio = XMVectorGetX(XMVector3Length(vSpearBlade - vSpearMid));

    return S_OK;

}

void CKhazan_Sample::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    if (m_pGameInstance->Key_Down(DIK_F5))
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(457.f, -12.f, 241.f, 1.f));

    XMStoreFloat4(&m_vPos, XMVectorSetW(m_pTransformCom->Get_State(STATE::POSITION), 1.f));

    if (m_pGameInstance->Key_Down(DIK_M))
        static_cast<CTarget_LockOn*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("LockOn")))->LockOn(&m_vPos);

    if (m_pGameInstance->Key_Down(DIK_N))
        static_cast<CTarget_LockOn*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("LockOn")))->LockOff();

    if (m_pGameInstance->Key_Down(DIK_B))
    {
        CTarget_BrutalAttack* pObject = static_cast<CTarget_BrutalAttack*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_BrutalAttack")));
        pObject->Setting_BrutalAttack(&m_vPos, 0.f);

        m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::TEST), TEXT("Layer_UI"), pObject);
    }
}

void CKhazan_Sample::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_R))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::DEFAULT, m_pTransformCom->Get_State(STATE::POSITION), 100, { 0.f, 10.f });
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }

    if (m_pGameInstance->Key_Down(DIK_T))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::BACK, m_pTransformCom->Get_State(STATE::POSITION), 1234);
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }
    if (m_pGameInstance->Key_Down(DIK_Y))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::SPECIAL, m_pTransformCom->Get_State(STATE::POSITION), 12345);
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }
    if (m_pGameInstance->Key_Down(DIK_U))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::PLAYER, m_pTransformCom->Get_State(STATE::POSITION), 100);
            m_pGameInstance->Push_PoolObject_ToLayer(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_UI"), pDamage);
        }
    }

    if (m_isEnableControl)
    {
        if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::LB))
        {
            _float3     vPickedPos{};
            _bool isPicked = m_pGameInstance->isPicked(&vPickedPos);
            if (true == isPicked)
            {
                m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickedPos), 1.f));
                m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 1.f));
            }
        }

        Update_State(fTimeDelta);
    }

#pragma region 상호 작용 맵 오브젝트 임시 테스트 용
    Event_Interact_Object(fTimeDelta);
#pragma endregion

    __super::Update(fTimeDelta);



    RayCast(fTimeDelta);

    //m_pRigidBodyCom->Update(fTimeDelta, m_pTransformCom->Get_WorldMatrix());

   /* m_pCharVirCom->Sync_Update(m_pTransformCom);
    m_pCharVirCom->Update(fTimeDelta, m_pTransformCom);*/
}

void CKhazan_Sample::Late_Update(_float fTimeDelta)
{




    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DYNAMIC, this)))
        return;

    __super::Late_Update(fTimeDelta);
}


HRESULT CKhazan_Sample::Render()
{


    return S_OK;

}

void CKhazan_Sample::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

void CKhazan_Sample::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
}

HRESULT CKhazan_Sample::RayCast(_float fTimeDelta)
{
    _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
    _vector vTargetPos = XMVectorSet(vPos.m128_f32[0], vPos.m128_f32[1] - 5.f, vPos.m128_f32[2], 1.f);

    _float fFraction;
    _float4 vPosition;
    _float3 outNormal;

    if (m_pGameInstance->RayCast(
        _float3(vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2]),
        _float3(vTargetPos.m128_f32[0], vTargetPos.m128_f32[1], vTargetPos.m128_f32[2]),
        fFraction,
        vPosition,
        &outNormal
    ))
    {
        // 충돌되는 지점 fFraction, vPosition
        int a = 0;
    }

    return S_OK;
}

#pragma region 상호 작용 맵 오브젝트 임시 테스트 용
void CKhazan_Sample::Event_Interact_Object(_float fTimeDelta)
{
    // 상호 작용 오브젝트 쪽에서 BEGIN STATE 내보낼 시
    if (EventInteractType::EVENT_STATE::BEGIN == m_EventInteract.eState)
    {
        // 플레이어 이동, LOOK 보간??
        // 완료하면 이벤트 반대로 던져주기

        if (true)               // 특정 조건 완성하면 이벤트 발생
        {
            // 상호작용 활성화시 맵 오브젝트한테 ObjectOn 을 true 로 던져주고, ObjectOff 를 false 로 던져준다.
            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OnEvent() });
            m_EventInteract.eState = EventInteractType::EVENT_STATE::NONE;
        }
    }

    // 상호 작용 오브젝트 쪽에서 END STATE 내보낼 시
    if (EventInteractType::EVENT_STATE::END == m_EventInteract.eState)
    {
        if (true)               // 특정 조건 완성하면 이벤트 발생
        {
            // 상호작용 비 활성화시 맵 오브젝트한테 ObjectOn 을 false 로 던져주고, ObjectOff 를 true 로 던져준다.
            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { EventObject::OffEvent() });
            m_EventInteract.eState = EventInteractType::EVENT_STATE::NONE;
        }
    }

    // 이벤트가 발생 했을 때
    if (true == m_EventInteract.isEvent)
    {
        // 귀검일때
        if (INTERACTIVE_TYPE::CHECKPOINT == m_EventInteract.eInteractType)
        {
            BladeNexus_Event(fTimeDelta);
        }
        // 상자일때 ( 나중에 창고, 파밍 상자 나눌 예정 )
        if (INTERACTIVE_TYPE::CHEST == m_EventInteract.eInteractType)
        {
            Chest_Event(fTimeDelta);
        }
        // 경계의 틈 툼스톤일때
        if (INTERACTIVE_TYPE::TOMBSTONE == m_EventInteract.eInteractType)
        {
            TombStone_Event(fTimeDelta);
        }
    }
}

void CKhazan_Sample::BladeNexus_Event(_float fTimeDelta)
{
    EventBladeNexus BNEvent = m_EventInteract.BNEvent;

    // 귀검에 접촉 후 상호 작용 ( 귀검 가동 )
    if (false == BNEvent.isBNOpened)
    {
        // 귀검 첫 해금 시
        if (true == BNEvent.isUnLock)
        {
            // 첫 해금 플레이어 애니메이션?
        }
        // 이미 해금된 귀검
        else if (false == BNEvent.isUnLock)
        {
            // 해금된 귀검 플레이어 애니메이션?
        }

        // 플레이어 Look -> 귀검 ( 기우는거 보정하려고 이렇게 코드 넣어놨습니다. )
        BNEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&BNEvent.vPosition), 1.f));
    }
    // 귀검 가동 끝나고 UI 팝업 ( 귀검 UI 창 활성화 )
    else if (true == BNEvent.isBNOpened)
    {
        // 귀검 첫 해금 시
        if (true == BNEvent.isUnLock)
        {
            // 첫 해금 플레이어 귀검 LOOP Animation?
        }
        // 이미 해금된 귀검
        else if (false == BNEvent.isUnLock)
        {
            // 해금 된 플레이어 귀검 LOOP Animation?
        }
    }

    m_EventInteract.End_Event();
}

void CKhazan_Sample::Chest_Event(_float fTimeDelta)
{
    EventChest ChestEvent = m_EventInteract.ChestEvent;

    // 상자에 접촉 후 상호 작용 ( 닫힌 상태 )
    if (false == ChestEvent.isChestOpened)
    {
        // 플레이어 Look -> 상자, Position 상자 본 위치로 이동 ( 기우는거 보정 )
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&ChestEvent.vPlayerPosition), 1.f));
        ChestEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&ChestEvent.vPosition), 1.f));

        m_EventInteract.End_Event();
    }
    // 상자 열리는 애니메이션 종료되면 ( 열린 상태 )
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

void CKhazan_Sample::TombStone_Event(_float fTimeDelta)
{
    EventTombStone TSEvent = m_EventInteract.TSEvent;

    // 툼스톤에 접촉 후 상호 작용 ( 툼스톤 가동 )
    if (false == TSEvent.isTSOpened)
    {
        // 플레이어 Look -> 툼스톤 ( 기우는거 보정하려고 이렇게 코드 넣어놨습니다. )
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&TSEvent.vPlayerPosition), 1.f));
        TSEvent.vPosition.y = m_pTransformCom->Get_State(STATE::POSITION).m128_f32[1];
        m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&TSEvent.vPosition), 1.f));
    }
    // 툼스톤 가동 끝나고 가동 LOOP 진입
    else if (true == TSEvent.isTSOpened)
    {
        // 플레이어 툼스톤 LOOP 애니메이션?
    }

    m_EventInteract.End_Event();
}
#pragma endregion

void CKhazan_Sample::Update_State(_float fTimeDelta)
{
    if (Has_State(ATTACK_FAST))
    {
        if (m_pBody->Get_Model()->Check_MinAnimationTime())
        {
            if (m_iFastAttackIndex == 0)
            {
				m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk02"));
				++m_iFastAttackIndex;
			}
			else if (m_iFastAttackIndex == 1)
			{
				m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk03_02"));
				//m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_LightningSpear_Advanced"));
				m_iFastAttackIndex = 0;
				Remove_State(ATTACK_FAST);
			}
        }
        return;
    }
    if (Has_State(ATTACK_STRONG))
    {
        if (m_pBody->Get_Model()->Check_MinAnimationTime())
        {
            if (m_iStrongAttackIndex == 0)
            {
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk02"));
                ++m_iStrongAttackIndex;
            }
            else if (m_iStrongAttackIndex == 1)
            {
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk03"));
                m_iStrongAttackIndex = 0;
                Remove_State(ATTACK_STRONG);
            }
        }
        return;
    }

    Key_Input(fTimeDelta);


}

void CKhazan_Sample::Key_Input(_float fTimeDelta)
{

    if (!Has_State(ATTACK_ALL))
    {
        if (m_pGameInstance->Key_Down(DIK_S))
        {
            ++m_isMove;
            Add_DirState(DOWN);
        }
        if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
        {
            m_pTransformCom->Go_Backward(fTimeDelta * m_fMoveSpeed);
        }
        if (m_pGameInstance->Key_Up(DIK_S))
        {
            m_isMove = 0;
            // m_isMove = m_isMove - 1 < 0 ? 0 : m_isMove - 1;
            Remove_DirState(DOWN);
        }

        if (m_pGameInstance->Key_Down(DIK_W))
        {
            ++m_isMove;
            Add_DirState(UP);
        }
        if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
        {
            m_pTransformCom->Go_Straight(fTimeDelta * m_fMoveSpeed);
        }
        if (m_pGameInstance->Key_Up(DIK_W))
        {
            m_isMove = 0;

            //m_isMove = m_isMove - 1 < 0 ? 0 : m_isMove - 1;
            Remove_DirState(UP);
        }

        if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);

        if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
    }

    if (m_isMove > 0)
    {
        Add_State(WALK);
        Remove_State(IDLE);
    }
    else
    {
        Add_State(IDLE);
        Remove_State(MOVING);
    }


    if (Has_State(WALK) && m_pGameInstance->Key_Down(DIK_LSHIFT))
    {
        Add_State(RUN);
    }
    else if (Has_State(WALK) && m_pGameInstance->Key_Up(DIK_LSHIFT))
    {
        Remove_State(RUN);
    }

    if (!m_pBody->Get_Model()->Check_MinAnimationTime())
        return;

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_Z))
    {
        Clear_State();
        m_isMove = 0;
        m_StrongComboIndex = 0;

        _bool isNext = false;
        if (m_FastComboIndex > 0)
        {
            if (*m_pBody->Get_Model()->Get_CurTrackPosition() > 16.f)
                isNext = true;
        }

        if (m_FastComboIndex == 0) {
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk01"));
            m_FastComboIndex++;
        }
        if (isNext && m_FastComboIndex == 1) {
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk02"));
            m_FastComboIndex++;
        }
        if (isNext && m_FastComboIndex == 2) {
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk03_02"));
            m_FastComboIndex = 0;
        }

		Add_State(ATTACK_FAST);
	}

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_X))
    {
        Clear_State();
        m_isMove = 0;
        m_FastComboIndex = 0;

        _bool isNext = false;
        if (m_StrongComboIndex > 0)
        {
            if (*m_pBody->Get_Model()->Get_CurTrackPosition() > 16.f)
                isNext = true;
        }

        if (m_StrongComboIndex == 0) {
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk01"));
            m_StrongComboIndex++;
        }
        if (isNext && m_StrongComboIndex == 1) {
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk02"));
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk02"));
            m_StrongComboIndex++;
        }
        if (isNext && m_StrongComboIndex == 2) {
            m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk03"));
            m_StrongComboIndex = 0;
        }
        Add_State(ATTACK_STRONG);
    }

    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_C))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_SpaceTimeCutter03"));
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_FULLMOON);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_V))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk01"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_STRONG);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_B))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_SpiralSpear"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_SPIRAL);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_N))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_TwisterSpear"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_TWISTE);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_M))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_PureMind_SeismicKick"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_STRIKE);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_G))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_LowFlying_F"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_SOON);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_H))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Crescent"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_VITALPOINT);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_J))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_MoonVeil"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_SHADOW2);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta) && m_pGameInstance->Key_Down(DIK_K))
    {
        Clear_State();
        m_isMove = 0;
        m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_PureMind_TranceSpirit_GrappleAtk01"));
        m_FastComboIndex = m_StrongComboIndex = 0;
        m_FastComboIndex = m_StrongComboIndex = 0;

        Add_State(ATTACK_BRUTAL);
    }
    //if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && m_pGameInstance->Key_Down(DIK_L))
    //{
    //    Clear_State();
    //    m_isMove = 0;
    //    m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_LowFlying_F"));

    //    Add_State(ATTACK_COUNT);
    //}

}

HRESULT CKhazan_Sample::Ready_Components()
{
    return S_OK;
}

HRESULT CKhazan_Sample::Ready_PartObjects()
{
    CBody_Khazan_Sample::BODY_KHAZAN_SAMPLE_DESC         BodyDesc{};
    BodyDesc.pState = &m_iState;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_CKhazan_Sample_Body"), ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Body_Khazan_Sample"), &BodyDesc)))
        return E_FAIL;

    m_pBody = static_cast<CBody_Khazan_Sample*>(Find_PartObject(TEXT("Part_CKhazan_Sample_Body")));
    m_pWeaponR_Matrix = m_pBody->Get_BoneMatrix("Weapon_R");
    Safe_AddRef(m_pBody);

    CSpear_Khazan_Sample::SPEAR_KHAZAN_SAMPLE_DESC         SpearDesc{};
    SpearDesc.pState = &m_iState;
    SpearDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    SpearDesc.pParentTransform = m_pTransformCom;
    if (FAILED(__super::Add_PartObject(TEXT("Part_CKhazan_Sample_Weapon_Spear"), ENUM_CLASS(LEVEL::TEST), TEXT("Prototype_GameObject_Spear_Khazan_Sample"), &SpearDesc)))
        return E_FAIL;

    m_pSpear = static_cast<CSpear_Khazan_Sample*>(Find_PartObject(TEXT("Part_CKhazan_Sample_Weapon_Spear")));
    //m_pSpearFX_Matrix = m_pSpear->Get_BoneMatrix("FX");
    //m_SpearOffset_Matrix = m_pSpear->Get_OffestMatrix();
    Safe_AddRef(m_pSpear);

    /* 넘겨주기  */
    m_pSpear->Set_matWeaponR(m_pWeaponR_Matrix);
    //m_pBody->Set_matSpearWeaponR(m_pWeaponR_Matrix);

    //m_pBody->Set_matSpearFX(m_pSpearFX_Matrix);
    //m_pBody->Set_matSpearOffset(m_SpearOffset_Matrix);
    //m_pBody->Set_matWorldSpearBladeFX(&m_SpearFX_WorldMatrix);

    //m_pBody->Set_matWorldSpearEndFX(&m_SpearEndFX_WorldMatrix);

	return S_OK;
}

HRESULT CKhazan_Sample::Ready_Collision()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 0.7f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    tCharVirDesc.fRadius = 0.5f;
    tCharVirDesc.fHeight = 0.5f;
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fMass = 100000.f;
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

inline _bool CKhazan_Sample::Has_States()
{
    for (_uint i = 0; i < GetBitPosition(CKhazan_Sample::END); ++i)
    {
        if (Has_State(1 << i))
            return true;

    }
    return false;
}

#ifdef _DEBUG
void CKhazan_Sample::Debug_Widget()
{
    m_pGameInstance->AddWidget(TEXT("Client"), [this]() {

        ImGui::Begin("Control Guide");

        //  Movement
        ImGui::BeginChild("LeftPanel", ImVec2(220, 0), true);
        {
            ImGui::Text("Movement");
            ImGui::Separator();
            ImGui::BulletText("U key Move Forward (WALK)");
            ImGui::BulletText("D key Move Backward");
            ImGui::BulletText("L key Turn Left");
            ImGui::BulletText("R key Turn Right");
            ImGui::BulletText("U key + LSHIFT  Run (RUN)");

            //ImGui::Text("Spear Ratio");
            //ImGui::DragFloat("Blade Dist Ratio ", &m_fBladeDist, 0.01f);
            //ImGui::DragFloat("End Dist Ratio ", &m_fEndDist, 0.01f);

        }
        ImGui::EndChild();

        // Attack & Other
        ImGui::SameLine();
        ImGui::BeginChild("MiddlePanel", ImVec2(300, 0), true);
        {
            ImGui::Text("Attack");
            ImGui::Separator();
            ImGui::BulletText("Z  Fast Attack (ATTACK_FAST)");
            ImGui::BulletText("X  Set Attack (ATTACK_SET)");

            ImGui::Spacing();
            ImGui::Text("Other");
            ImGui::Separator();
            ImGui::BulletText("LShift + Mouse(LB) Ground click -> teleport");
            ImGui::BulletText("Idle state when not moving");

            /*_vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
            char szPlayerPos[128];
            sprintf_s(szPlayerPos, sizeof(szPlayerPos), "Sample Pos : %.2f, %.2f, %.2f",
                vPos.m128_f32[0],
                vPos.m128_f32[1],
                vPos.m128_f32[2]);
            ImGui::Text(szPlayerPos);

            char szOffset[128];
            sprintf_s(szOffset, sizeof(szOffset), "Offset Pos : %.2f, %.2f, %.2f",
               m_SpearOffset_Matrix.r[3].m128_f32[0],
               m_SpearOffset_Matrix.r[3].m128_f32[1],
               m_SpearOffset_Matrix.r[3].m128_f32[2]);
            ImGui::Text(szOffset);

            char szSpearBlade[128];
            sprintf_s(szSpearBlade, sizeof(szSpearBlade), " local Spear Blade Pos : %.2f, %.2f, %.2f",
                m_pSpearFX_Matrix->_41,
                m_pSpearFX_Matrix->_42,
                m_pSpearFX_Matrix->_43);
            ImGui::Text(szSpearBlade);

            char szSpearMid[128];
            sprintf_s(szSpearMid, sizeof(szSpearMid), "local Spear Mid Pos : %.2f, %.2f, %.2f",
                m_pWeaponR_Matrix->_41,
                m_pWeaponR_Matrix->_42,
                m_pWeaponR_Matrix->_43);
            ImGui::Text(szSpearMid);

            char szSpearBladeW[128];
            sprintf_s(szSpearBladeW, sizeof(szSpearBladeW), " World Spear Blade Pos : %.2f, %.2f, %.2f",
                m_SpearFX_WorldMatrix._41,
                m_SpearFX_WorldMatrix._42,
                m_SpearFX_WorldMatrix._43);
            ImGui::Text(szSpearBladeW);

            char szSpearEnd[128];
            sprintf_s(szSpearEnd, sizeof(szSpearEnd), " World Spear End Pos : %.2f, %.2f, %.2f",
                m_SpearEndFX_WorldMatrix._41,
                m_SpearEndFX_WorldMatrix._42,
                m_SpearEndFX_WorldMatrix._43);
            ImGui::Text(szSpearEnd);*/
        }
        ImGui::EndChild();

        // 오른쪽 패널: 사용자 정의 공간
        ImGui::SameLine();
        ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
        {
            if (ImGui::Button(m_isEnableControl ? "Clicked Enable Control" : "Clicked Disable Control", ImVec2(-1.0f, 0.0f)))
                m_isEnableControl = !m_isEnableControl;

            ImGui::Text("Manual Area");

            // === Speed Section ===
            ImGui::Separator();
            ImGui::DragFloat("Move Speed", &m_fMoveSpeed, 0.1f, 0.f, 100.f);

            // === Animation Control Section ===
            ImGui::Spacing();
            ImGui::Text("Animation Control");
            ImGui::Separator();

            // Fast Attack Combo
            if (ImGui::Button("Fast Attack 1", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk01"));
                m_FastComboIndex = 0;
                m_StrongComboIndex = 0;
                Add_State(ATTACK_FAST);
            }

            if (ImGui::Button("Fast Attack 2", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk02"));
                m_FastComboIndex = 0;
                m_StrongComboIndex = 0;
                Add_State(ATTACK_FAST);
            }

            if (ImGui::Button("Fast Attack 3", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_FastAtk03_02"));
                m_FastComboIndex = 0;
                m_StrongComboIndex = 0;
                Add_State(ATTACK_FAST);
            }

            ImGui::Spacing();

            // Strong Attack Combo
            if (ImGui::Button("Strong Attack 1", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk01"));
                m_FastComboIndex = 0;
                m_StrongComboIndex = 0;
                Add_State(ATTACK_STRONG);
            }

            if (ImGui::Button("Strong Attack 2", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk02"));
                m_FastComboIndex = 0;
                m_StrongComboIndex = 0;
                Add_State(ATTACK_STRONG);
            }

            if (ImGui::Button("Strong Attack 3", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Com_StrongAtk03"));
                m_FastComboIndex = 0;
                m_StrongComboIndex = 0;
                Add_State(ATTACK_STRONG);
            }

            ImGui::Spacing();

            // Special Attacks
            if (ImGui::Button("Full Moon (C)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_SpaceTimeCutter03"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_FULLMOON);
            }

            if (ImGui::Button("Spiral Spear (B)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_SpiralSpear"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_SPIRAL);
            }

            if (ImGui::Button("Twister Spear (N)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_TwisterSpear"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_TWISTE);
            }

            if (ImGui::Button("Seismic Kick (M)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_PureMind_SeismicKick"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_STRIKE);
            }

            if (ImGui::Button("Low Flying (G)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_LowFlying_F"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_SOON);
            }

            if (ImGui::Button("Crescent (H)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Crescent"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_VITALPOINT);
            }

            if (ImGui::Button("Moon Veil (J)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_Tempest_MoonVeil"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_SHADOW2);
            }

            if (ImGui::Button("Brutal Attack (K)", ImVec2(-1.0f, 0.0f)))
            {
                Clear_State();
                m_isMove = 0;
                m_pBody->Get_Model()->Set_Animation(m_pBody->Get_Model()->Get_AnimIndexByName("CA_P_Kazan_Spear_PureMind_TranceSpirit_GrappleAtk01"));
                m_FastComboIndex = m_StrongComboIndex = 0;
                Add_State(ATTACK_BRUTAL);
            }

            ImGui::Spacing();
            ImGui::Separator();

            // === HP Section ===
            ImGui::Text("Health");
            ImGui::Separator();
            ImGui::SliderFloat("Current HP", &m_fCurrentHP, 0.0f, m_fMaxHP, "%.1f");
            ImGui::InputFloat("Max HP", &m_fMaxHP);
            ImGui::ProgressBar(m_fCurrentHP / max(0.0001f, m_fMaxHP), ImVec2(-1.0f, 0.0f), "HP");

            // === Stamina Section ===
            ImGui::Spacing();
            ImGui::Text("Stamina");
            ImGui::Separator();
            ImGui::SliderFloat("Current Stamina", &m_fCurrentStamina, 0.0f, m_fMaxStamina, "%.1f");
            ImGui::InputFloat("Max Stamina", &m_fMaxStamina);
            ImGui::ProgressBar(m_fCurrentStamina / max(0.0001f, m_fMaxStamina), ImVec2(-1.0f, 0.0f), "Stamina");

            // === Attack Section ===
            ImGui::Spacing();
            ImGui::Text("Attack");
            ImGui::Separator();
            ImGui::SliderFloat("Attack Power", &m_fAttack, 0.0f, 500.0f, "%.1f");
        }
        ImGui::EndChild();

        ImGui::End();
        });
}
#endif // _DEBUG


CKhazan_Sample* CKhazan_Sample::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CKhazan_Sample* pInstance = new CKhazan_Sample(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CKhazan_Sample"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CKhazan_Sample::Clone(void* pArg)
{
    CKhazan_Sample* pInstance = new CKhazan_Sample(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CKhazan_Sample"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKhazan_Sample::Free()
{
    __super::Free();
    //Safe_Release(m_pCharVirCom);
    Safe_Release(m_pBody);
    Safe_Release(m_pSpear);

}