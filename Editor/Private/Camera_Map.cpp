#include "Camera_Map.h"
#include "GameInstance.h"

CCamera_Map::CCamera_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera { pDevice, pContext }
{
}

CCamera_Map::CCamera_Map(const CCamera_Map& Prototype)
    : CCamera (Prototype)
{
}

HRESULT CCamera_Map::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Map::Initialize_Clone(void* pArg)
{
    CAMERA_MAP_DESC* pDesc = static_cast<CAMERA_MAP_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Map::Priority_Update(_float fTimeDelta)
{
    Input(fTimeDelta);

    __super::Update_PipeLines();
}

void CCamera_Map::Update(_float fTimeDelta)
{
}

void CCamera_Map::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Map::Render()
{
    return S_OK;
}

void CCamera_Map::Input(_float fTimeDelta)
{
    CHECK_FALSE(m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::WB), );

    _float fSpeed = fTimeDelta;

    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta))
        fSpeed *= 20.f;

    if (m_pGameInstance->Key_Pressing(DIK_TAB, fTimeDelta))
        fSpeed *= 10.f;

    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))   m_pTransformCom->Go_Straight(fSpeed);
    if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))   m_pTransformCom->Go_Backward(fSpeed);
    if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))   m_pTransformCom->Go_Left(fSpeed);
    if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))   m_pTransformCom->Go_Right(fSpeed);

    if (m_pGameInstance->Key_Pressing(DIK_SPACE, fTimeDelta))
    {
        _vector		vPosition = m_pTransformCom->Get_State(STATE::POSITION);
        _vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

        vPosition += XMVector3Normalize(vUp) * 10.f * fSpeed;

        m_pTransformCom->Set_State(STATE::POSITION, vPosition);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LCONTROL, fTimeDelta))
    {
        _vector		vPosition = m_pTransformCom->Get_State(STATE::POSITION);
        _vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

        vPosition -= XMVector3Normalize(vUp) * 10.f * fSpeed;

        m_pTransformCom->Set_State(STATE::POSITION, vPosition);
    }

    _int iMouseMove = {};

    _vector vLook = XMVector4Normalize(m_pTransformCom->Get_State(STATE::LOOK));
    _vector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    _float fScalar = XMVectorGetX(XMVector3Dot(vLook, vWorldUp));
    fScalar = clamp(fScalar, -1.f, 1.f);
    _float fAngle = XMConvertToDegrees(acosf(fScalar));

    if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X))
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);

    if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y))
    {
        const _float fMinAngle = 5.f;
        const _float fMaxAngle = 175.f;

        _float fDeltaAngle = fTimeDelta * iMouseMove * m_fMouseSensor;

        if (fDeltaAngle < 0.f)
        {
            if (fAngle > fMinAngle)     m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fDeltaAngle);
        }
        else
        {
            if (fAngle < fMaxAngle)     m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fDeltaAngle);
        }
    }
}

CCamera_Map* CCamera_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Map* pInstance = new CCamera_Map(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCamera_Map"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Map::Clone(void* pArg)
{
    CCamera_Map* pInstance = new CCamera_Map(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCamera_Map"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Map::Free()
{
    __super::Free();
}
