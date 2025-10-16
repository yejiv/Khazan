#include "Camera_UI.h"
#include "GameInstance.h"

CCamera_UI::CCamera_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CCamera{ pDevice,pContext }
{
}

CCamera_UI::CCamera_UI(const CCamera_UI& Prototype)
    :CCamera{ Prototype }
{
}

HRESULT CCamera_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_UI::Initialize_Clone(void* pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;


    return S_OK;
}

void CCamera_UI::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Mouse_Down(MOUSEKEYSTATE::RB))
    {
        if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
            m_pTransformCom->Go_Straight(fTimeDelta);
        if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
            m_pTransformCom->Go_Backward(fTimeDelta);
        if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
            m_pTransformCom->Go_Left(fTimeDelta);
        if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
            m_pTransformCom->Go_Right(fTimeDelta);

        _int    iMouseMove = {};


        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);


        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y))
            m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
    }



    __super::Update_PipeLines();


}

void CCamera_UI::Update(_float fTimeDelta)
{

}

void CCamera_UI::Late_Update(_float fTimeDelta)
{

}

HRESULT CCamera_UI::Render()
{
    return S_OK;
}

CCamera_UI* CCamera_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_UI* pInstance = new CCamera_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CCamera_UI"));
    }

    return pInstance;
}

CGameObject* CCamera_UI::Clone(void* pArg)
{
    CCamera_UI* pInstance = new CCamera_UI(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Cloned : CCamera_UI"));
    }

    return pInstance;
}

void CCamera_UI::Free()
{
    __super::Free();
}
