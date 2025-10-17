#include "Camera_Shader.h"
#include "GameInstance.h"

CCamera_Shader::CCamera_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_Shader::CCamera_Shader(const CCamera_Shader& Prototype)
    : CCamera(Prototype)
{
}

HRESULT CCamera_Shader::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_Shader::Initialize_Clone(void* pArg)
{
    CAMERA_EFFECT_DESC* pDesc = static_cast<CAMERA_EFFECT_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Shader::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }

    _int iMouseMove = {};

    if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::RB))
    {
        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X))
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);

        if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y))
            m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

    __super::Update_PipeLines();
}

void CCamera_Shader::Update(_float fTimeDelta)
{
}

void CCamera_Shader::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_Shader::Render()
{
    return S_OK;
}

CCamera_Shader* CCamera_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCamera_Shader* pInstance = new CCamera_Shader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CCamera_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCamera_Shader::Clone(void* pArg)
{
    CCamera_Shader* pInstance = new CCamera_Shader(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CCamera_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Shader::Free()
{
    __super::Free();
}
