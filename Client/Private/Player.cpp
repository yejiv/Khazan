#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

#include "RigidBody.h"
#include "CharacterVirtual.h"

#include "Damage_Text.h"
#include "Mon_HP.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{

}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainerObject{ Prototype }
{

}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void* pArg)
{
    m_iMaxHp = 1000;
    m_iHp = 1000;
    m_iMaxStamina = 1000;
    m_iStamina = 1000;
    GAMEOBJECT_DESC         Desc{};
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);
    
    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    if (FAILED(Ready_Collision()))
        return E_FAIL;

#pragma region 상호 작용 맵 오브젝트 임시 테스트용
    m_pGameInstance->Subscribe_Event<EventInteractType>(ENUM_CLASS(EVENT_TYPE::INTERACT_TYPE), [&](const EventInteractType& e)
        {
            m_EventInteract = e;
        });
#pragma endregion

    CMon_HP* pHP = static_cast<CMon_HP*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Mon_HP")));
    if (pHP != nullptr)
    {
        pHP->Setting_HP(m_pTransformCom->Get_WorldMatrixPtr(), {0.f, 200.f}, &m_iHp, &m_iMaxHp, &m_iStamina, &m_iMaxStamina);
        m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::STAGE1), TEXT("Layer_UI"), pHP);
    }
    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    if (m_pGameInstance->Key_Down(DIK_O))
    {
        m_iHp -= 30.f;
        m_iStamina -= 30.f;
    }
    if (m_pGameInstance->Key_Down(DIK_P))
    {
        m_iHp += 30.f;
        m_iStamina += 30.f;
    }
}

void CPlayer::Update(_float fTimeDelta)
{

    if (GetKeyState(VK_LBUTTON) & 0x8000)
    {
        _float3     vPickedPos{};
        _bool isPicked = m_pGameInstance->isPicked(&vPickedPos);
        if (true == isPicked)
        {
            m_pTransformCom->Set_State(Engine::STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPickedPos), 1.f));
            m_pCharVirCom->Set_Velocity(XMVectorSet(0.f, 0.f, 0.f, 1.f));
        }
    }

    if (GetKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransformCom->Go_Backward(fTimeDelta * 0.5);
    }
    if (GetKeyState(VK_LEFT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * -1.f);
    }
    if (GetKeyState(VK_RIGHT) & 0x8000)
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * 1.f);
    }

    if (GetKeyState(VK_UP) & 0x8000)
    {
        m_pTransformCom->Go_Straight(fTimeDelta * 0.5);

        if (m_iState & IDLE)
            m_iState ^= IDLE;

        m_iState |= RUN;
    }
    else
    {
        if (m_iState & RUN)
            m_iState ^= RUN;

        m_iState |= IDLE;
    }

#pragma region 상호 작용 맵 오브젝트 임시 테스트 용
    
    if (m_pGameInstance->Key_Down(DIK_F))
    {
        // 스페이스 누르면 오브젝트 상호작용 이벤트 발생
        m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { true, false });
    }

    if (m_pGameInstance->Key_Down(DIK_LCONTROL))
    {
        // LCONTROL 누르면 상자랑 귀검 상호작용
        m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), { false, true });
    }

    if(true == m_EventInteract.isEvent)
    {
        m_EventInteract.isEvent = false;

        if (INTERACTIVE_TYPE::CHECKPOINT == m_EventInteract.eInteractType)
        {
            _int a = 10;
        }
        if (INTERACTIVE_TYPE::CHEST == m_EventInteract.eInteractType)
        {
            EventChest ChestEvent = m_EventInteract.ChestEvent;

            if (false == ChestEvent.isChestOpened)
            {
                m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&ChestEvent.vPlayerPosition), 1.f));
                m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&ChestEvent.vPosition), 1.f));
            }
        }
    }
    
#pragma endregion

     __super::Update(fTimeDelta);

    //m_pRigidBodyCom->Update(fTimeDelta, m_pTransformCom->Get_WorldMatrix());

    m_pCharVirCom->Sync_Update(m_pTransformCom);
    m_pCharVirCom->Update(fTimeDelta, m_pTransformCom);

    if (m_pGameInstance->Key_Down(DIK_R))
    {
        CDamage_Text* pDamage = static_cast<CDamage_Text*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Damage_Text")));
        if (pDamage != nullptr)
        {
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::DEFAULT, m_pTransformCom->Get_State(STATE::POSITION), 100, {0.f, 10.f});
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
            pDamage->Render_Damage(CDamage_Text::DAMAGE_TYPE::SPECIAL, m_pTransformCom->Get_State(STATE::POSITION), 12345657656);
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
    //m_pCharacterCom->Update(fTimeDelta, m_pBodyCom, m_pTransformCom);

}

void CPlayer::Late_Update(_float fTimeDelta)
{
    /*   m_pTransformCom->Set_State(Engine::STATE::POSITION,
           m_pNavigationCom->Compute_OnCell(m_pTransformCom->Get_State(Engine::STATE::POSITION)));*/

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
        return;

#ifdef _DEBUG

#endif

    __super::Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{

    return S_OK;
}

void CPlayer::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    int a = 0;
}

void CPlayer::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
{
    int a = 0;
}

HRESULT CPlayer::Ready_Components()
{

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    CBody_Player::BODY_DESC         BodyDesc{};
    BodyDesc.pState = &m_iState;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::STAGE1), TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_Collision()
{
    /*CRigidBody::RIGID_CAPSULESHAPE_DESC RigidDesc{};
    RigidDesc.fHeight = 1.f;
    RigidDesc.fRadius = 1.f;*/
    /*CRigidBody::RIGID_BOXSHAPE_DESC RigidDesc{};
    RigidDesc.vExtent = { 0.5f, 0.5f, 0.5f };*/

    //CRigidBody::RIGID_MESHSHAPE_DESC RigidDesc{};
   /* CRigidBody::RIGID_CONVEXSHAPE_DESC RigidDesc{};
    CBody_Player* pBody = dynamic_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));

    RigidDesc.pModel = pBody->Get_Model();

    RigidDesc.bIsTrigger = false;
    RigidDesc.bStartActive = true;
    RigidDesc.eMotion = EMotionType::Kinematic;
    RigidDesc.eQuality = EMotionQuality::Discrete;
    RigidDesc.eShapeType = SHAPE::MESH;
    RigidDesc.fFriction = 0.8f;
    RigidDesc.fMass = 0.0f;
    RigidDesc.fRestitution = 0.0f;
    RigidDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PLAYER);
    _float3 vPos{};
    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    _float4 vQuat{};
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    RigidDesc.vPos = vPos;
    RigidDesc.vQuat = vQuat;



    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_RigidBody"),
        TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidDesc)))
        return E_FAIL;*/

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
    m_tCollisionDesc.pGameObject = this;
    //pCollDesc.pInfo = ?? // 작성하기
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
        return E_FAIL;

    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();
    //Safe_Release(m_pRigidBodyCom);
    Safe_Release(m_pCharVirCom);
}
