#include "Editor_TempCamera.h"
#include "GameInstance.h"

CEditor_TempCamera::CEditor_TempCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CEditor_TempCamera::CEditor_TempCamera(const CEditor_TempCamera& Prototype)
    : CCamera{ Prototype }
{
}

HRESULT CEditor_TempCamera::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CEditor_TempCamera::Initialize_Clone(void* pArg)
{
	TEMPCAMERA_DESC* pDesc = static_cast<TEMPCAMERA_DESC*>(pArg);

    m_fMouseSensor = pDesc->fMouseSensor;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_pGameInstance->AddWidget(TEXT("Animatiaon"), [this]() {
        ImGui::Begin("Camera");
        
        ImGui::End();
        });

    return S_OK;
}

void CEditor_TempCamera::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) )
    {
        m_pTransformCom->Go_Straight(fTimeDelta* 2.f);
        _float3 temp;
        XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }
    if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) )
    {
        m_pTransformCom->Go_Backward(fTimeDelta * 2.f);
        _float3 temp;
        XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }
    if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
    {
        m_pTransformCom->Go_Left(fTimeDelta * 2.f);
        _float3 temp;
        XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }
    if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
    {
        m_pTransformCom->Go_Right(fTimeDelta * 2.f);
        _float3 temp;
        XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }

    _int    iMouseMove = {};


    if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

    if (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y))
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::RIGHT), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

    __super::Update_PipeLines();
}

void CEditor_TempCamera::Update(_float fTimeDelta)
{
    
}

void CEditor_TempCamera::Late_Update(_float fTimeDelta)
{
}

HRESULT CEditor_TempCamera::Render()
{
    return S_OK;
}

CEditor_TempCamera* CEditor_TempCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEditor_TempCamera* pInstance = new CEditor_TempCamera(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_TempCamera"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEditor_TempCamera::Clone(void* pArg)
{
    CEditor_TempCamera* pInstance = new CEditor_TempCamera(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEditor_TempCamera"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEditor_TempCamera::Free()
{
    __super::Free();


}