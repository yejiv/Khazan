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

    m_pGameInstance->AddWidget(TEXT("Animatiaon"), [&]() {
        ImGui::Begin("Camera");
        ImGui::DragFloat("Camera Speed", &m_fCameraSpeed, 0.01f, 0.f, 1000.f);

        // 키 가이드 박스
        ImGui::BeginChild("KeyGuide", ImVec2(0, 150), true, ImGuiWindowFlags_NoScrollbar);

        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Quick Reference:");
        ImGui::Dummy(ImVec2(0, 5));
        if (ImGui::BeginTable("ControlTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "W / A / S / D");
            ImGui::TableNextColumn();
            ImGui::Text("Camera Movement");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Left Shift + Mouse");
            ImGui::TableNextColumn();
            ImGui::Text("Camera Rotation");

            ImGui::EndTable();
        }

        ImGui::EndChild();
        ImGui::End();
        });

    return S_OK;
}

void CEditor_TempCamera::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Pressing(DIK_W, fTimeDelta) )
    {
        m_pTransformCom->Go_Straight(fTimeDelta * 2.f);
        //_float3 temp;
        //XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        //OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }
    if (m_pGameInstance->Key_Pressing(DIK_S, fTimeDelta) )
    {
        m_pTransformCom->Go_Backward(fTimeDelta * 2.f);
        //_float3 temp;
        //XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        //OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }
    if (m_pGameInstance->Key_Pressing(DIK_A, fTimeDelta))
    {
        m_pTransformCom->Go_Left(fTimeDelta * 2.f);
        //_float3 temp;
        //XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        //OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }
    if (m_pGameInstance->Key_Pressing(DIK_D, fTimeDelta))
    {
        m_pTransformCom->Go_Right(fTimeDelta * 2.f);
        //_float3 temp;
       //XMStoreFloat3(&temp, m_pTransformCom->Get_State(STATE::POSITION));
        //OutputDebugStringA((std::to_string(temp.x) + " " + std::to_string(temp.y) + " " + std::to_string(temp.z) + "\n").c_str());
    }

    _int    iMouseMove = {};


    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::X)))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * iMouseMove * m_fMouseSensor);
    }

    if (m_pGameInstance->Key_Pressing(DIK_LSHIFT, fTimeDelta) && (iMouseMove = m_pGameInstance->Mouse_Move(MOUSEMOVESTATE::Y)))
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