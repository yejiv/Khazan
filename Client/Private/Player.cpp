#include "ClientPch.h"
#include "Player.h"
#include "GameInstance.h"

#include "Body_Player.h"

#include "PhysicsBody.h"
#include "Character.h"

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

HRESULT CPlayer::Initialize(void* pArg)
{
    GAMEOBJECT_DESC         Desc{};
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.0f);

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
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
        }
    }

    if (GetKeyState(VK_DOWN) & 0x8000)
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
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
        m_pTransformCom->Go_Straight(fTimeDelta);

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


    __super::Update(fTimeDelta);

    //m_pBodyCom->Update(fTimeDelta, m_pTransformCom);

    m_pCharacterCom->Update(fTimeDelta, m_pBodyCom, m_pTransformCom);

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

HRESULT CPlayer::Ready_Components()
{
    CPhysicsBody::BODY_SHAPE_DESC tShape{};
    tShape.eType = SHAPE::BOX;
    tShape.fHalfHeight = 0.9f;
    tShape.fRadius = 0.5f;
    tShape.vHalfExtents = { 0.5f, 0.5f, 0.5f };
    CPhysicsBody::BODY_MATERIAL_DESC tMaterial{};
    tMaterial.fFriction = 0.8f;
    tMaterial.fRestitution = 0.0f;
    CPhysicsBody::BODY_DESC tBody{};
    tBody.tShape = tShape;
    tBody.tMat = tMaterial;
    tBody.pTransform = m_pTransformCom;
    tBody.eMotion = EMotionType::Static;
    tBody.iObjectLayer = JoltLayers::CHARACTER;
    tBody.bStartActive = true;
    tBody.bIsTrigger = false;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_PhysicsBody"),
        TEXT("Com_PhysicsBody"), reinterpret_cast<CComponent**>(&m_pBodyCom), &tBody)))
        return E_FAIL;

    CCharacter::CHARACTER_DESC tChar{};
    tChar.pBody = m_pBodyCom;
    tChar.pTransform = m_pTransformCom;
    tChar.fMaxSlopeDeg = 50.f;
    tChar.fStepOffset = 0.4f;
    tChar.fGroundSnap = 0.1f;
    tChar.fGravity = 9.81f;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Character"),
        TEXT("Com_Character"), reinterpret_cast<CComponent**>(&m_pCharacterCom), &tChar)))
        return E_FAIL;

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

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();
}
