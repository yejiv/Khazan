#include "AnimationTool.h"
#include "GameInstance.h"
#include "Editor_Model.h"
#include "JOH_EditorModelTest.h"
#include <commdlg.h>


CAnimationTool::CAnimationTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAnimationTool::Initialize_Prototype()
{

	Widget();

	return S_OK;
}

void CAnimationTool::Update(_float fTimeDelta)
{
}



void CAnimationTool::Add_Model()
{
	_wstring prototypeTag = TEXT("Prototype_Component_Editor_Model_") + m_strModelName;

	/* Prototype_Component_Editor_Model_xxxx */
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(m_vPreScale.x, m_vPreScale.y, m_vPreScale.z) * XMMatrixRotationY(XMConvertToRadians(180.0f));

    // 프로토타입 추가
    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::ANIMATION), prototypeTag,
        CEditor_Model::Create(m_pDevice, m_pContext,
            m_isAnim ? MODELTYPE::ANIM : MODELTYPE::NONANIM,
            m_strModelPath.c_str(), PreTransformMatrix))))
    {
        MSG_BOX(TEXT("모델 로드 실패!"));
        return;
    }

    // 게임 오브젝트 생성
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(
        ENUM_CLASS(LEVEL::ANIMATION), TEXT("Layer_Model"),
        ENUM_CLASS(LEVEL::ANIMATION),
        TEXT("Prototype_GameObject_Editor_Animation_TestModel"),
        &prototypeTag)))
    {
        MSG_BOX(TEXT("게임 오브젝트 클론 실패!"));
        return;
    }
    CGameObject* pGameObject = m_pGameInstance->Get_BackGameObject(ENUM_CLASS(LEVEL::ANIMATION), TEXT("Layer_Model"));

    if (pGameObject)
    {
        CJOH_EditorModelTest* pModelTest = dynamic_cast<CJOH_EditorModelTest*>(pGameObject);
        if (pModelTest)
        {
            m_GameObjects.emplace_back(pModelTest);
            m_ObjectNames.emplace_back(m_strModelName);
            m_strModelName = L"";
            m_strModelPath = "";
        }
        else
        {
            MSG_BOX(TEXT("타입 변환 실패!"));
        }
    }
}

void CAnimationTool::Remove_Model()
{
    if (m_GameObjects.empty())
    {
        MSG_BOX(TEXT("삭제할 모델이 없습니다!"));
        return;
    }

    if (m_iSelectedIndex < 0 || m_iSelectedIndex >= (_int)m_GameObjects.size())
    {
        MSG_BOX(TEXT("유효하지 않은 선택입니다!"));
        m_iSelectedIndex = -1;
        return;
    }

    CJOH_EditorModelTest* pObject = m_GameObjects[m_iSelectedIndex];

    if (pObject == nullptr)
    {
        MSG_BOX(TEXT("선택된 오브젝트가 유효하지 않습니다!"));
        m_GameObjects.erase(m_GameObjects.begin() + m_iSelectedIndex);
        m_iSelectedIndex = -1;
        return;
    }

    // 오브젝트 비활성화
    pObject->OnUnEnble();

    // 벡터에서 제거
    m_GameObjects.erase(m_GameObjects.begin() + m_iSelectedIndex);
    m_ObjectNames.erase(m_ObjectNames.begin() + m_iSelectedIndex);

    // 인덱스 재조정
    m_iSelectedIndex = -1;

}

void CAnimationTool::Widget()
{
	m_pGameInstance->AddWidget(TEXT("Animatiaon"), [this]() {
		ImGui::Begin("Anim Tool");

        /* Open Model */
        if (ImGui::Button("Open Model Manager", ImVec2(150, 25))) m_bShowOpenModel = !m_bShowOpenModel;
        ImGui::SameLine();
        if (m_bShowOpenModel) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[OPEN]");
        else ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[CLOSED]");
        ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();
        if (m_bShowOpenModel) OpenModel_Widget();

        /* Tool  */
        if (ImGui::Button("Tool Manager", ImVec2(150, 25))) m_bShowTool = !m_bShowTool;
        ImGui::SameLine();
        if (m_bShowTool) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[OPEN]");
        else ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[CLOSED]");
        ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();
        if (m_bShowTool) Tool_Widget();


        /* Info */
        if(!m_bShowOpenModel && !m_bShowTool)
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Click 'Open Model Manager' to load models");
            ImGui::Spacing();
            ImGui::Text("Current Loaded Models: %d", (_int)m_GameObjects.size());
        }

        


		ImGui::End();
		});


}

void CAnimationTool::OpenModel_Widget()
{
    // PreScale 조절 
    ImGui::SeparatorText("Model Scale");
    ImGui::DragFloat3("Pre-Scale", (_float*)&m_vPreScale, 0.001f, 0.001f, 10.0f, "%.3f");

    // 애니메이션 타입 선택 
    ImGui::SeparatorText("Model Type");
    ImGui::Checkbox("Is Animated Model", &m_isAnim);

    // 파일 선택 
    ImGui::SeparatorText("Load Model");

    // 파일 선택 버튼
    if (ImGui::Button("Browse Model File...", ImVec2(200, 0)))
    {
        // Windows 파일 다이얼로그 열기
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "FBX Files\0*.fbx\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = "../../Client/Bin/Resources/Models/";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            // 경로 저장
            m_strModelPath = szFile;

            // 파일 이름 추출 (확장자 제외)
            string fullPath = szFile;
            size_t lastSlash = fullPath.find_last_of("\\/");
            size_t lastDot = fullPath.find_last_of(".");

            if (lastSlash != string::npos && lastDot != string::npos && lastDot > lastSlash)
            {
                string fileName = fullPath.substr(lastSlash + 1, lastDot - lastSlash - 1);

                // string을 wstring으로 변환
                m_strModelName = wstring(fileName.begin(), fileName.end());
            }
        }
    }

    ImGui::Spacing();

    // 선택된 파일 정보 표시
    if (!m_strModelPath.empty())
    {
        ImGui::BeginGroup();

        // 모델 이름 표시
        ImGui::Text("Model Name:");
        ImGui::SameLine();
        string modelNameStr(m_strModelName.begin(), m_strModelName.end());
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", modelNameStr.c_str());

        // 모델 경로 표시
        ImGui::Text("Model Path:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", m_strModelPath.c_str());

        ImGui::EndGroup();
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No model selected");
    }

    ImGui::Spacing();
    ImGui::Separator();

    _bool bDisabled = m_strModelPath.empty();

    // 모델이 선택되지 않았으면 버튼 비활성화
    if (bDisabled)  ImGui::BeginDisabled();
    if (ImGui::Button("Add Model to Scene", ImVec2(200, 25)))  Add_Model();
    if (bDisabled) ImGui::EndDisabled();

    ImGui::Spacing();

    // 로드된 모델 목록
    ImGui::SeparatorText("Loaded Models");
    ImGui::Text("Model Count: %d", (_int)m_GameObjects.size());

    if (!m_GameObjects.empty())
    {
        // 리스트박스로 모델 선택
        if (ImGui::BeginListBox("##ModelList", ImVec2(-1, 150)))
        {
            for (_int i = 0; i < (_int)m_GameObjects.size(); ++i)
            {
                _bool isSelected = (m_iSelectedIndex == i);
                string label = WStringToAnsi(m_ObjectNames[i]);

                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    m_iSelectedIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        // 선택된 모델 제거 버튼
        _bool bRemoveDisabled = (m_iSelectedIndex < 0);
        if (bRemoveDisabled) ImGui::BeginDisabled();

        if (ImGui::Button("Remove Selected Model", ImVec2(200, 0)))
        {
            Remove_Model();
        }
        if (bRemoveDisabled)  ImGui::EndDisabled();
    }
}

void CAnimationTool::Tool_Widget()
{
    ImGui::SeparatorText("Loaded Models");
    ImGui::Text("Model Count: %d", (_int)m_GameObjects.size());

    if (!m_GameObjects.empty())
    {
        ImGui::Text("Current Selected Index: %d", m_iSelectedIndex);
        ImGui::Spacing();

        // 리스트박스로 모델 선택
        if (ImGui::BeginListBox("##ModelList", ImVec2(-1, 100)))
        {
            for (_int i = 0; i < (_int)m_GameObjects.size(); ++i)
            {
                _bool isSelected = (m_iSelectedIndex == i);
                string label = WStringToAnsi(m_ObjectNames[i]);

                if (ImGui::Selectable(label.c_str(), isSelected))
                {
                    m_iSelectedIndex = i;
                }
            }
            ImGui::EndListBox();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }

    Tool_Export_Update_Widget();

}

void CAnimationTool::Tool_Export_Update_Widget()
{
    // 선택된 모델이 있을 때만 버튼 활성화
    if (m_iSelectedIndex >= 0 && m_iSelectedIndex < (_int)m_GameObjects.size())
    {
        ImGui::SeparatorText("Model Tools");

        // 선택된 모델 정보 표시
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f),
            "Selected: %s", WStringToAnsi(m_ObjectNames[m_iSelectedIndex]).c_str());

        ImGui::Spacing();

        // Export Model 버튼
        if (ImGui::Button("Export Model", ImVec2(200, 25)))
        {
            if (m_GameObjects[m_iSelectedIndex] != nullptr &&
                m_GameObjects[m_iSelectedIndex]->get_Model() != nullptr)
            {
                m_GameObjects[m_iSelectedIndex]->get_Model()->ExportModel();
            }
            else
            {
                MSG_BOX(TEXT("유효하지 않은 모델입니다!"));
            }
        }

        ImGui::Spacing();

        // Update DAT From JSON 버튼
        if (ImGui::Button("Update DAT From JSON", ImVec2(200, 25)))
        {
            if (m_GameObjects[m_iSelectedIndex] != nullptr &&
                m_GameObjects[m_iSelectedIndex]->get_Model() != nullptr)
            {
                m_GameObjects[m_iSelectedIndex]->get_Model()->Update_DAT_From_JSON();
            }
            else
            {
                MSG_BOX(TEXT("유효하지 않은 모델입니다!"));
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();


    }
    else
    {
        // 선택된 모델이 없을 때
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f),
            "Please select a model from the list above");
    }
}

CAnimationTool* CAnimationTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimationTool* pInstance = new CAnimationTool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CAnimationTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimationTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
