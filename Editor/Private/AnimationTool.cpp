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
    /* 파일시스템에서 실행파일 위치를 .exe로 고정 */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    string exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of("\\/");
    if (lastSlash != string::npos)
        exeDir = exeDir.substr(0, lastSlash);
      
    filesystem::path projectRoot = filesystem::path(exeDir).parent_path().parent_path() / "Default";

    string projectRootStr = projectRoot.string();
    SetCurrentDirectoryA(projectRootStr.c_str());

    OutputDebugStringA(("[Working Directory Set] " + projectRootStr + "\n").c_str());

    // 확인
    _char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);
    OutputDebugStringA(("[Current Working Directory] " + string(currentDir) + "\n").c_str());


	Widget();

	return S_OK;
}

void CAnimationTool::Update(_float fTimeDelta)
{
}

void CAnimationTool::Widget()
{
	m_pGameInstance->AddWidget(TEXT("Animatiaon"), [this]() {
		ImGui::Begin("Anim Tool");

        /* Open Model */
        if (ImGui::Button("Open Model Manager", ImVec2(150, 25))) {
            m_isShowOpenModel = !m_isShowOpenModel;
            m_isShowTool = false;
            m_isShowTool_Control = false;
        }
        ImGui::SameLine();
        if (m_isShowOpenModel) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[OPEN]");
        else ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[CLOSED]");
        ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();
        if (m_isShowOpenModel) OpenModel_Widget();


        /* Tool  */
        if (ImGui::Button("Tool Manager", ImVec2(150, 25))) { 
            m_isShowTool = !m_isShowTool;
            m_isShowOpenModel = false;
            m_isShowTool_Control = false;

        }
        ImGui::SameLine();
        if (m_isShowTool) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[OPEN]");
        else ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[CLOSED]");
        ImGui::Spacing(); ImGui::Separator();  ImGui::Spacing();
        if (m_isShowTool) Tool_Widget();

        /* Animation Control */
        if (!m_GameObjects.empty())
        {
            if (ImGui::Button("Animation Controler", ImVec2(150, 25))) {
                m_isShowTool_Control = !m_isShowTool_Control;
                m_isShowTool = false;
                m_isShowOpenModel = false;
            }
            ImGui::SameLine();
            if (m_isShowTool_Control) ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[OPEN]");
            else ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[CLOSED]");
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            if (m_isShowTool_Control) Tool_AnimationControl_Widget();
        }

        /* Info */
        if(!m_isShowOpenModel && !m_isShowTool)
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
        _char savedDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, savedDir);

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
            string absolutePath = szFile;
            m_strModelPath = ConvertToRelativePath(absolutePath);

            // 파일 이름 추출 (확장자 제외)
            string fullPath = szFile;
            size_t lastSlash = fullPath.find_last_of("\\/");
            size_t lastDot = fullPath.find_last_of(".");

            if (lastSlash != string::npos && lastDot != string::npos && lastDot > lastSlash)
            {
                string fileName = fullPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
                m_strModelName = wstring(fileName.begin(), fileName.end());
            }
        }

        SetCurrentDirectoryA(savedDir);
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

        // 모델 경로 표시 (이제 상대 경로)
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
    if (ImGui::Button("Add Model to Scene", ImVec2(200, 25)))
        Add_Model();
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

        _bool bIsValid = (m_GameObjects[m_iSelectedIndex] != nullptr &&
            m_GameObjects[m_iSelectedIndex]->get_Model() != nullptr);

        ImGui::BeginDisabled(!bIsValid);

        // ===== Export Model 버튼 =====
        if (ImGui::Button("Export Model", ImVec2(200, 25)))
        {
            _char savedDir[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, savedDir);

            OPENFILENAMEA ofn;
            char szFile[260] = { 0 };

            // 기본 파일명
            string defaultName = WStringToAnsi(m_ObjectNames[m_iSelectedIndex]);
            strcpy_s(szFile, defaultName.c_str());

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Model Files\0*.model\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrInitialDir = "../../Client/Bin/Data/";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

            if (GetSaveFileNameA(&ofn) == TRUE)
            {
                string absolutePath = szFile;
                string savePath = ConvertToClientRelativePath(absolutePath);

                m_GameObjects[m_iSelectedIndex]->get_Model()->ExportModel(savePath);

                // MSG_BOX(TEXT("모델 Export 완료!"));
            }

            SetCurrentDirectoryA(savedDir);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Save to: Client/Bin/Data/");
        }

        ImGui::Spacing();

        // ===== Update DAT From JSON 버튼 =====
        if (ImGui::Button("Update DAT From JSON", ImVec2(200, 25)))
        {
            _char savedDir[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, savedDir);

            OPENFILENAMEA ofn;
            char szFile[260] = { 0 };

            // 기본 파일명
            string defaultName = WStringToAnsi(m_ObjectNames[m_iSelectedIndex]);
            strcpy_s(szFile, defaultName.c_str());

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Model Files\0*.model\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrInitialDir = "../../Client/Bin/Data/";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn) == TRUE)
            {
                string absolutePath = szFile;
                string savePath = ConvertToClientRelativePath(absolutePath);

                m_GameObjects[m_iSelectedIndex]->get_Model()->Update_DAT_From_JSON(savePath);

                //MSG_BOX(TEXT("DAT 파일 업데이트 완료!"));
            }

            SetCurrentDirectoryA(savedDir);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Save to: Client/Bin/Data/");
        }

        ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
    else
    {
        // 선택된 모델이 없을 때
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Please select a model from the list above");
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

    /* Export Update */
    if (ImGui::Button("Export and Update", ImVec2(250, 25))) {
        m_isShowTool_ExportUpdate = !m_isShowTool_ExportUpdate;
    }
    if(m_isShowTool_ExportUpdate)Tool_Export_Update_Widget();

    /* Animation time Controler*/
    if (ImGui::Button("Animation time Controler", ImVec2(250, 25))) {
        m_isShowTool_Control = !m_isShowTool_Control;
    }
    if (m_isShowTool_Control && m_iSelectedIndex > -1)Tool_AnimationControl_Widget();

    
}

void CAnimationTool::Tool_AnimationControl_Widget()
{
    ANIMATION_SETUP_DATA* animSetupData{};
    animSetupData = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_CurAnimSet();
    ImGui::Text(("Cur Animation : " + animSetupData->strName).c_str());
    ImGui::Checkbox("Animation Time Control", &animSetupData->isAnimTimeControl);
    ImGui::Spacing();   ImGui::Separator();    ImGui::Spacing();

    _float leftWidth = 300.0f;
    _float middleWidth = 400.0f;

    // === 왼쪽: 애니메이션 리스트 ===
    ImGui::BeginChild("LeftPanel", ImVec2(leftWidth, 0), true);
    {
        Tool_AnimationList_Widget();
    }
    ImGui::EndChild();
    // === 중간: 애니메이션 정보 ===
	ImGui::SameLine();
	ImGui::BeginChild("MiddlePanel", ImVec2(middleWidth, 0), true);
	{
        /* 애니메이션 정보  */
        Tool_AnimationInfo_Widget();
	}
	ImGui::EndChild();

	// === 오른쪽:  컨트롤 ===
	ImGui::SameLine();
	ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
	{
        /* 시간 조절 */
        /* 애니메이션 세트 */
        /* 이벤트  */

	}
	ImGui::EndChild();

}

void CAnimationTool::Tool_AnimationList_Widget()
{
	// ImGui::Begin("Animation List", &m_isShowTool_AnimationList);

	 // 모델 선택 확인
	if (m_iSelectedIndex < 0 || m_iSelectedIndex >= (_int)m_GameObjects.size())
	{
		ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Please select a model first!");
		ImGui::End();
		return;
	}

	CEditor_Model* pModel = m_GameObjects[m_iSelectedIndex]->get_Model();
	if (pModel == nullptr)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "CEditor_Model == nullptr");
		ImGui::End();
		return;
	}

	MODEL_DATA* modelData = pModel->Get_ModelData();
	_uint iNumAnimations = modelData->iNumAnimations;

	/* 애니메이션이 없음 */
	if (iNumAnimations == 0)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This model has no animations!");
		ImGui::End();
		return;
	}

	// 헤더 정보
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Model: %s", WStringToAnsi(m_ObjectNames[m_iSelectedIndex]).c_str());
	ImGui::Text("Total Animations: %d", iNumAnimations);
	ImGui::Separator();  ImGui::Spacing();

	_float leftWidth = 300.0f;
	_float middleWidth = 400.0f;

	ImGui::SeparatorText("Animation List");

	// 검색
	ImGui::SetNextItemWidth(-1);
	ImGui::InputTextWithHint("##Search", "Search...", m_szAnimSearchBuffer, sizeof(m_szAnimSearchBuffer));

	ImGui::Spacing();

	// 리스트
	if (ImGui::BeginChild("AnimListScroll", ImVec2(0, -30), true))
	{
		for (_uint i = 0; i < iNumAnimations; ++i)
		{
			const ANIMATION_DATA& animData = modelData->vecAnimation[i];
			string animName = animData.strName;

			// 검색 필터
			if (strlen(m_szAnimSearchBuffer) > 0)
			{
				if (animName.find(m_szAnimSearchBuffer) == string::npos)
					continue;
			}

			ImGui::PushID(i);

			_bool isSelected = (m_iSelectedAnimIndex == (_int)i);
			_int currentAnimIndex = pModel->Get_CurAnimIndex();
			_bool isPlaying = (currentAnimIndex == (_int)i);

			// 재생 중이면 초록색
			if (isPlaying)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

			if (ImGui::Selectable(animName.c_str(), isSelected))
			{
				m_iSelectedAnimIndex = i;
			}

			if (isPlaying)
				ImGui::PopStyleColor();

			// 더블클릭으로 재생
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				pModel->Set_Animation(i, animData.animSetup.isLoop);
				m_iSelectedAnimIndex = i;
			}

			// 호버 시 간단한 정보
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Animation: %s", animName.c_str());
				ImGui::Text("Duration: %.2f sec", animData.fDuration / animData.fTickPerSecond);
				ImGui::Text("Frames: %.0f", animData.fDuration);
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Double-click to play");
				ImGui::EndTooltip();
			}

			ImGui::PopID();
		}

	}
	ImGui::EndChild();

	// 하단 정보
	if (m_iSelectedAnimIndex >= 0)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f),"Selected: %d", m_iSelectedAnimIndex);
	}

}

void CAnimationTool::Tool_AnimationInfo_Widget()
{
    if (m_iSelectedIndex < 0 || m_iSelectedAnimIndex < 0)
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),"Select an animation");
        return;
    }

    CEditor_Model* pModel = m_GameObjects[m_iSelectedIndex]->get_Model();
    if (!pModel) return;

    MODEL_DATA* modelData = pModel->Get_ModelData();
    ANIMATION_DATA* animData = &modelData->vecAnimation[m_iSelectedAnimIndex];
    ANIMATION_SETUP_DATA* setup = pModel->Get_CurAnimSet();

    ImGui::SeparatorText("Animation Information");

    // 기본 정보
    ImGui::Text("strName: %s", setup->strName.c_str());
    //ImGui::Text("isLoop: %s", (setup->isLoop ? "true" : "false"));
    ImGui::Checkbox("isLoop", &setup->isLoop);
    ImGui::Text("iDirection: %s", to_string(setup->iDirection).c_str());
    _int iTemp= setup->iDirection;
    ImGui::DragInt("iDirection", &iTemp, 1, 0, 24);
    setup->iDirection = iTemp;

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // 구간 별 시간 조절
    ImGui::SeparatorText("Setup Time Control");
    //ImGui::Text("isAnimTimeControl: %s", (setup->isAnimTimeControl ? "true" : "false"));
    //ImGui::SeparatorText("Time Control");
    ImGui::Checkbox("isAnimTimeControl", &setup->isAnimTimeControl);

    if (setup->isAnimTimeControl)
    {
        ImGui::Indent();
        ImGui::Text("Sections: %d", (_int)setup->vecAnimTimeControlFrame.size());

        // 각 구간 표시
        for (size_t i = 0; i < setup->vecAnimTimeControlFrame.size(); ++i)
        {
            ImGui::PushID((_int)i);

            FLOAT3_DATA& frame = setup->vecAnimTimeControlFrame[i];

            if (ImGui::TreeNode(("Section " + to_string(i)).c_str()))
            {
                ImGui::Text("Start Frame:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::DragFloat("##start", &frame.x, 1.0f, 0.0f, animData->fDuration, "%.0f");

                ImGui::Text("End Frame:  ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::DragFloat("##end", &frame.y, 1.0f, frame.x, animData->fDuration, "%.0f");

                ImGui::Text("Speed:      ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::DragFloat("##speed", &frame.z, 0.1f, 0.1f, 10.0f, "%.1fx");

                if (ImGui::Button("Remove Section", ImVec2(150, 0)))
                {
                    setup->vecAnimTimeControlFrame.erase(
                        setup->vecAnimTimeControlFrame.begin() + i);
                    ImGui::TreePop();
                    ImGui::PopID();
                    break;
                }

                ImGui::TreePop();
            }
            else
            {
                // 접혀있을 때 간략 정보
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    "(%.0f-%.0f, %.1fx)", frame.x, frame.y, frame.z);
            }

            ImGui::PopID();
        }

        //if (ImGui::Button("Add Section", ImVec2(150, 25)))
        //{
        //    FLOAT3_DATA newFrame(0.0f, animData->fDuration, 1.0f);
        //    setup->vecAnimTimeControlFrame.push_back(newFrame);
        //}

        ImGui::Unindent();
    }

    ImGui::Spacing();    ImGui::Separator();   ImGui::Spacing();

    // ===== 애니메이션 세트 =====
    //ImGui::SeparatorText("Animation Set");
    ImGui::Checkbox("isAnimSet", &setup->isAnimSet);

    if (setup->isAnimSet)
    {
        ImGui::Indent();

        ImGui::Text("strAnimSetName: %s", setup->strAnimSetName.c_str());

        ImGui::Text("vecAnimSet: %d animations", (_int)setup->vecAnimSet.size());
        if (ImGui::TreeNode("Animation Set List"))
        {
            for (size_t i = 0; i < setup->vecAnimSet.size(); ++i)
            {
                ImGui::BulletText("[%d] %s", (_int)i, setup->vecAnimSet[i].c_str());
            }
            ImGui::TreePop();
        }

        ImGui::Text("iAnimSetSelfIndex: %d", setup->iAnimSetSelfIndex);

        ImGui::Text("iTransitionType: %d", setup->iTransitionType);
        ImGui::SameLine();
        const char* transitionTypes[] = { "Auto", "Flag", "Input", "Manual" };
        if (setup->iTransitionType < 4)
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                "(%s)", transitionTypes[setup->iTransitionType]);

        ImGui::Checkbox("isWaitForComplete", &setup->isWaitForComplete);

        ImGui::Text("fAnimSetBlendOutTime: %.2f", setup->fAnimSetBlendOutTime);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##blendOut", &setup->fAnimSetBlendOutTime, 0.01f, 0.0f, 2.0f, "%.2f");

        ImGui::Text("fAnimSetBlendInTime:  %.2f", setup->fAnimSetBlendInTime);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##blendIn", &setup->fAnimSetBlendInTime, 0.01f, 0.0f, 2.0f, "%.2f");

        ImGui::Unindent();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ===== 루트 모션 =====
    ImGui::SeparatorText("Root Motion");

    ImGui::Checkbox("isRootMotion", &setup->isRootMotion);

    if (setup->isRootMotion)
    {
        ImGui::Indent();

        ImGui::Checkbox("isApplyRootRotation", &setup->isApplyRootRotation);
        ImGui::Checkbox("isApplyRootPosition", &setup->isApplyRootPosition);

        ImGui::Text("RootMotionScale:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::DragFloat3("##rootScale",
            &setup->RootMitionScale.x, 0.1f, 0.0f, 10.0f, "%.1f");

        ImGui::Unindent();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ===== 이벤트 =====
    ImGui::SeparatorText("Events");

    ImGui::Checkbox("isEvent", &setup->isEvent);

    if (setup->isEvent)
    {
        ImGui::Indent();

        ImGui::Checkbox("isTriggerOnce", &setup->isTriggerOnce);
        ImGui::Checkbox("isTriggerOnExit", &setup->isTriggerOnExit);

        ImGui::Text("Events: %d", (_int)setup->vecEventKeys.size());

        for (size_t i = 0; i < setup->vecEventKeys.size(); ++i)
        {
            ImGui::PushID((_int)i);

            if (ImGui::TreeNode(("Event " + to_string(i)).c_str()))
            {
                ImGui::Text("Key: %s", setup->vecEventKeys[i].c_str());

                ImGui::Text("Start Frame:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::DragFloat("##eventStart",
                    &setup->vecEventFrames[i].x, 1.0f, 0.0f, animData->fDuration, "%.0f");

                ImGui::Text("End Frame:  ");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(150);
                ImGui::DragFloat("##eventEnd",
                    &setup->vecEventFrames[i].y, 1.0f, 0.0f, animData->fDuration, "%.0f");

                if (ImGui::Button("Remove Event", ImVec2(150, 0)))
                {
                    setup->vecEventKeys.erase(setup->vecEventKeys.begin() + i);
                    setup->vecEventFrames.erase(setup->vecEventFrames.begin() + i);
                    ImGui::TreePop();
                    ImGui::PopID();
                    break;
                }

                ImGui::TreePop();
            }
            else
            {
                // 접혀있을 때 간략 정보
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    "(%s: %.0f-%.0f)",
                    setup->vecEventKeys[i].c_str(),
                    setup->vecEventFrames[i].x,
                    setup->vecEventFrames[i].y);
            }

            ImGui::PopID();
        }

        if (ImGui::Button("Add Event", ImVec2(150, 25)))
        {
            setup->vecEventKeys.push_back("NewEvent");
            setup->vecEventFrames.push_back(FLOAT2_DATA{ 0.0f, 0.0f });
        }

        ImGui::Unindent();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ===== 저장 버튼 =====
    //if (ImGui::Button("Save Changes", ImVec2(-1, 30)))
    //{
    //    MSG_BOX(TEXT("변경사항이 적용되었습니다!\nExport를 통해 파일로 저장하세요."));
    //}

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
        MSG_BOX(TEXT("[Remove_Model]삭제할 모델이 없습니다"));
        return;
    }

    if (m_iSelectedIndex < 0 || m_iSelectedIndex >= (_int)m_GameObjects.size())
    {
        MSG_BOX(TEXT("[Remove_Model]유효하지 않은 선택입니다"));
        m_iSelectedIndex = -1;
        return;
    }

    CJOH_EditorModelTest* pObject = m_GameObjects[m_iSelectedIndex];

    if (pObject == nullptr)
    {
        MSG_BOX(TEXT("[Remove_Model]선택된 오브젝트가 nullptr 입니다"));
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

// Editor.exe 기준 상대 경로로 변환 (모델 로드용)
string CAnimationTool::ConvertToRelativePath(const string& absolutePath)
{
    namespace fs = std::filesystem;

    try
    { 
        // 결과: C:\...\Khazan\Editor\Bin\Debug
        _char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        fs::path exeDir = fs::path(exePath).parent_path();
      
        // Editor\Bin\Debug -> Editor\Bin -> Editor -> Editor\Default
        fs::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";

        OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

        // 절대 경로
        fs::path absPath = fs::absolute(absolutePath);
        OutputDebugStringA(("[Absolute Path] " + absPath.string() + "\n").c_str());

        // Editor/Default 기준 상대 경로 계산
        fs::path relativePath = fs::relative(absPath, editorDefaultDir);

        string result = relativePath.string();
        replace(result.begin(), result.end(), '\\', '/');

        OutputDebugStringA(("[Editor Relative Path] " + result + "\n").c_str());

        return result;
    }
    catch (const fs::filesystem_error& e)
    {
        OutputDebugStringA(("[Path Error] " + string(e.what()) + "\n").c_str());
        return absolutePath;
    }
}
// Client.exe 기준 상대 경로로 변환 (저장용)
string CAnimationTool::ConvertToClientRelativePath(const string& absolutePath)
{
    namespace fs = std::filesystem;

    try
    { 
        // 결과: C:\...\Khazan\Editor\Bin\Debug
        _char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        fs::path exeDir = fs::path(exePath).parent_path();


        // Editor\Bin\Debug -> Editor\Bin -> Editor -> Khazan -> Client\Default
        fs::path clientDefaultDir = exeDir.parent_path().parent_path() / "Client" / "Default";

        OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());
        OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

        // 절대 경로
        fs::path absPath = fs::absolute(absolutePath);
        OutputDebugStringA(("[Absolute Path] " + absPath.string() + "\n").c_str());

        // Client/Default 기준 상대 경로 계산
        fs::path relativePath = fs::relative(absPath, clientDefaultDir);

        string result = relativePath.string();
        replace(result.begin(), result.end(), '\\', '/');

        OutputDebugStringA(("[Client Relative Path] " + result + "\n").c_str());

        return result;
    }
    catch (const fs::filesystem_error& e)
    {
        OutputDebugStringA(("[Path Error] " + string(e.what()) + "\n").c_str());
        return absolutePath;
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
