#include "EditorPch.h"
#include "Camera_Effect.h"
#include "GameInstance.h"

CCamera_Effect::CCamera_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Effect::CCamera_Effect(const CCamera_Effect& Prototype)
    : CCamera(Prototype)
{
}

HRESULT CCamera_Effect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Effect::Initialize_Clone(void* pArg)
{
    CAMERA_EFFECT_DESC* pDesc = static_cast<CAMERA_EFFECT_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Effect::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_DIKeyState(DIK_W))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_S))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_A))
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_D))
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }

    _int iMouseMove = {};

    if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::RB))
    {
        if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::X))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);

        if (iMouseMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::Y))
            m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

    __super::Update_PipeLines();
}

void CCamera_Effect::Update(_float fTimeDelta)
{
}

void CCamera_Effect::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Effect::Render()
{
    return S_OK;
}

CCamera_Effect* CCamera_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Effect* pInstance = new CCamera_Effect(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCamera_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Effect::Clone(void* pArg)
{
    CCamera_Effect* pInstance = new CCamera_Effect(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCamera_Effect"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Effect::Free()
{
    __super::Free();
}
