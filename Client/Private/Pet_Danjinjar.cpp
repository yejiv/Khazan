#include "Pet_Danjinjar.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "CharacterVirtual.h"

#include "Body_Pet_Danjinjar.h"
#include "AI_Controller_Pet_Danjinjar.h"

#include "UI_Talk_Danjinjar.h"

CPet_Danjinjar::CPet_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMonster{ pDevice,pContext }
{
}

CPet_Danjinjar::CPet_Danjinjar(const CPet_Danjinjar& Prototype)
    :CMonster(Prototype)
{
}

void CPet_Danjinjar::isTalk(_bool isTalk, _int iIndex)
{
    if (isTalk)
    {
        if (iIndex < 0)
            return;

        m_pTalk->On_Panel(iIndex);
    }
    else
    {
        m_pTalk->Off_Panel();
    }
}

CPet_Danjinjar::MONDATA& CPet_Danjinjar::Get_Data()
{
    return m_Data;
}

void CPet_Danjinjar::LockOnLerp(_float fTimeDetla, _float fSpeed)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDetla, fSpeed);
}

void CPet_Danjinjar::LockOn()
{
    m_pTransformCom->LookAt(m_pTarget->Get_Position());
}

void CPet_Danjinjar::Player_TP()
{
    m_pCharVirCom->Teleport(XMVectorSetY(m_pTarget->Get_Position(), XMVectorGetY(m_pTarget->Get_Position()) + 5.f), m_pTarget->Get_Transform()->Get_Rotation_Quat(), m_pTransformCom);
}

void CPet_Danjinjar::LookAt_Lerp(_float fTimeDelta)
{
    m_pTransformCom->LookAt_Lerp(m_pTarget->Get_Position(), fTimeDelta, 0.8f);

}

_bool CPet_Danjinjar::Check_Ranage(string strKey)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));
    _float fAttackRanage = m_pBlackBoard->Get_Value<_float>(m_strName, strKey);

    if (fAttackRanage <= 0)
        return false;

    if (fDist <= fAttackRanage)
        return true;
    else
        return false;
}

_bool CPet_Danjinjar::Check_Ranage(_float fRange)
{
    _float fDist = XMVectorGetX(XMVector3Length(m_pTarget->Get_Transform()->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION)));

    if (fRange <= 0)
        return true;

    if (fDist <= fRange)
        return true;
    else
        return false;
}

TARGET_DIR CPet_Danjinjar::Get_DIR()
{
    return Check_Dir(m_pTransformCom->Get_WorldMatrix(), m_pTarget->Get_Transform()->Get_State(STATE::POSITION));
}

HRESULT CPet_Danjinjar::Initialize_Prototype(_int iLevel)
{

    m_iPrototypeIndex = iLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);

    return S_OK;
}

HRESULT CPet_Danjinjar::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    CHECK_FAILED(Ready_MonData(), E_FAIL);
    CHECK_FAILED(Ready_ETC(), E_FAIL);
    CHECK_FAILED(Ready_PartObjects(), E_FAIL);
    CHECK_FAILED(Ready_AnimEvent(), E_FAIL);
    CHECK_FAILED(Ready_Components(), E_FAIL);

    m_pGameInstance->Subscribe_Event<EVENT_PET_STATE>(ENUM_CLASS(EVENT_TYPE::PET), [&](const EVENT_PET_STATE& e) {State_Change(e.isStart); });
    m_isActive = false;
    return S_OK;
}

void CPet_Danjinjar::Priority_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    CContainerObject::Priority_Update(fTimeDelta);
    m_pTalk->Priority_Update(fTimeDelta);
}

void CPet_Danjinjar::Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_fTimeDelta = fTimeDelta;

    m_pController->Update(this, fTimeDelta);
    __super::Update(fTimeDelta);

    m_pTalk->Update_UITransform(m_pTransformCom->Get_State(STATE::POSITION));
    m_pTalk->Update(fTimeDelta);
}

void CPet_Danjinjar::Late_Update(_float fTimeDelta)
{
    if (!m_isActive)
        return;

    m_pTalk->Late_Update(fTimeDelta);
    CContainerObject::Late_Update(fTimeDelta);
}

void CPet_Danjinjar::Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject)
{
}

void CPet_Danjinjar::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER_ATTACK))
    {
        m_pCharVirCom->Jump_Direction(Get_Look() * -1.f, 10.f, 30.f);
        m_Data.isDamage = true;
    }
}

void CPet_Danjinjar::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{

}

void CPet_Danjinjar::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{

}

HRESULT CPet_Danjinjar::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Pet_Danjinjar_Body"),
        CBody_Pet_Danjinjar::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPet_Danjinjar::Ready_ETC()
{
    m_pController = CAI_Controller_Pet_Danjinjar::Create(this);
    CHECK_NULLPTR(m_pController, E_FAIL);

    m_pBlackBoard = m_pController->Get_BlackBoard();
    Safe_AddRef(m_pBlackBoard);
    m_pBlackBoard->Set_Value<CGameObject*>(m_strName, "Target", m_pTarget);

    CUIObject::UIOBJECT_DESC Desc;

    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 3.625f, 1.f };
    Desc.szName = "Pet_DanjinJar_TalkUI";
    m_pTalk = static_cast<CUI_Talk_Danjinjar*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_TalkDanjinjar"), &Desc));
    CHECK_NULLPTR(m_pTalk, E_FAIL);

    return S_OK;
}

HRESULT CPet_Danjinjar::Ready_Components()
{
    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
    _float3 vPos{};
    _float4 vQuat{};

    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
    tCharVirDesc.vPos = vPos;
    tCharVirDesc.vQuat = vQuat;
    tCharVirDesc.vShapeOffset = _float3(0.f, 1.35f, 0.f);
    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::PET);
    tCharVirDesc.fMaxSlopeAngle = 45.f;
    tCharVirDesc.fPenetrationRecoverySpeed = 0.1f;

    m_tCollisionDesc.pGameObject = this;
    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;

    //캡슐 Desc
    tCharVirDesc.fRadius = 1.2f;
    tCharVirDesc.fHeight = 0.4f;

    CHECK_FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc), E_FAIL);


    return S_OK;
}

HRESULT CPet_Danjinjar::Ready_PartObjects()
{
    CBody_Pet_Danjinjar::BODY_DESC BodyDesc{};
    BodyDesc.pData = &m_Data;
    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
    BodyDesc.pOwnerTransform = m_pTransformCom;

    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), m_iPrototypeIndex, TEXT("Prototype_PartObject_Monster_Pet_Danjinjar_Body"), &BodyDesc)))
        return E_FAIL;

    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
    if (nullptr == pBody)
        return E_FAIL;

    m_pBody = dynamic_cast<CBody_Pet_Danjinjar*>(pBody);
    Safe_AddRef(m_pBody);

    return S_OK;
}

HRESULT CPet_Danjinjar::Ready_AnimEvent()
{
    CModel* pModel = m_pBody->Get_Model();

    return S_OK;
}

HRESULT CPet_Danjinjar::Ready_MonData()
{
    m_Data.pOwner = this;
    m_Data.fEdgeWidth = 0.1f;
    m_Data.fEdgeColor = { 4.2f, 1.6f, 0.2f, 1.f };

    return S_OK;
}

void CPet_Danjinjar::State_Change(_bool isStart)
{
    if (isStart)
        m_isActive = true;
    else
        m_isActive = false;
}

CPet_Danjinjar* CPet_Danjinjar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel)
{
    CPet_Danjinjar* pInstance = new CPet_Danjinjar(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CPet_Danjinjar"));
    }
    return pInstance;
}

CGameObject* CPet_Danjinjar::Clone(void* pArg)
{
    CPet_Danjinjar* pInstance = new CPet_Danjinjar(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Clone : CPet_Danjinjar"));
    }
    return pInstance;
}

void CPet_Danjinjar::Free()
{
    m_Data.pOwner = nullptr;

    __super::Free();
    Safe_Release(m_pBody);
    Safe_Release(m_pBlackBoard);
    Safe_Release(m_pTalk);
}
