#include "AnimationTool.h"
#include "GameInstance.h"
#include "Editor_Model.h"
#include "JOH_EditorModelTest.h"
#include "Editor_Animation.h"
#include <commdlg.h>
#include <intrin.h>


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
    /* 파일시스템에서 실행파일 위치를 .vcxproj로 고정 */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    string exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of("\\/");
    if (lastSlash != string::npos) exeDir = exeDir.substr(0, lastSlash);
    SetCurrentDirectoryA(exeDir.c_str());
    OutputDebugStringA(("[Working Directory Set] " + string(exePath) + "\n").c_str());

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

        if (!m_GameObjects.empty())
        {
            m_GameObjects[0]->Debug_RenderState();
            if (ImGui::CollapsingHeader("Rotation Info"))
            {
                _matrix worldMatrix = m_GameObjects[0]->Debug_GetTransformMatrix();
                _vector scale, rotQ, pos;
                XMMatrixDecompose(&scale, &rotQ, &pos, worldMatrix);

                _float x = XMVectorGetX(rotQ);
                _float y = XMVectorGetY(rotQ);
                _float z = XMVectorGetZ(rotQ);
                _float w = XMVectorGetW(rotQ);

                // 라디안 계산
                _float roll = atan2f(2 * (w * x + y * z), 1 - 2 * (x * x + y * y)); // X
                _float pitch = asinf(2 * (w * y - z * x));                           // Y
                _float yaw = atan2f(2 * (w * z + x * y), 1 - 2 * (y * y + z * z)); // Z

                // 각도 변환
                _float rollDeg = XMConvertToDegrees(roll);
                _float pitchDeg = XMConvertToDegrees(pitch);
                _float yawDeg = XMConvertToDegrees(yaw);

                // ImGui 출력
                ImGui::SeparatorText("Rotation (Euler)");
                ImGui::Text("X (Roll)  : %.4f rad  |  %.2f°", roll, rollDeg);
                ImGui::Text("Y (Pitch) : %.4f rad  |  %.2f°", pitch, pitchDeg);
                ImGui::Text("Z (Yaw)   : %.4f rad  |  %.2f°", yaw, yawDeg);
            }
        }

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

        /* Info */
        if (!m_isShowOpenModel && !m_isShowTool)
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
    ImGui::DragFloat3("Pre-Scale", (_float*)&m_vPreScale, 0.001f, 0.001f, 10.0f, "%.5f");
    if (ImGui::Button("Fast 0.0001")) m_vPreScale = _float3(0.0001f, 0.0001f, 0.0001f);
    ImGui::SameLine();
    if (ImGui::Button("Fast 1.0")) m_vPreScale = _float3(1.f, 1.f, 1.f);

    // 애니메이션 타입 선택 
    ImGui::SeparatorText("Model Type");
    ImGui::Checkbox("Is Animated Model", &m_isAnim);

	//랜더 그룹 선택 
	ImGui::SeparatorText("RenderGroup");
    const _char* renderGroupNames[] = {"PRIORITY", "SHADOW", "NONBLEND", "NONLIGHT", "BLEND", "UI"};
    if (ImGui::Combo("##RenderGroupCombo", &m_iCurrentRenderGroup, renderGroupNames, IM_ARRAYSIZE(renderGroupNames)))
    {
        //m_iCurrentRenderGroup = 3;
    }
    ImGui::Text("Selected: %s", renderGroupNames[m_iCurrentRenderGroup]);

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
        ofn.lpstrFilter = "FBX Files (*.fbx)\0*.fbx\0"
            "GLTF Files (*.gltf)\0*.gltf\0"
            "All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = "../../../Client/Bin/Resources/Models/";
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

    // 버튼 
    _uint iLevelIndex = {};
    ImGui::BeginChild("AnimPanel", ImVec2(0, 30), true);
    {
        if (ImGui::Button("MAP", ImVec2(60, 0))) { iLevelIndex = ENUM_CLASS(LEVEL::MAP); m_isLevelBtnPress = true; }
        ImGui::SameLine();
        if (ImGui::Button("ANIMATION", ImVec2(60, 0))) { iLevelIndex = ENUM_CLASS(LEVEL::ANIMATION); m_isLevelBtnPress = true; }
        ImGui::SameLine();
        if (ImGui::Button("EFFECT", ImVec2(60, 0))) { iLevelIndex = ENUM_CLASS(LEVEL::EFFECT); m_isLevelBtnPress = true; }
        ImGui::SameLine();
        if (ImGui::Button("UI", ImVec2(60, 0))) { iLevelIndex = ENUM_CLASS(LEVEL::UI); m_isLevelBtnPress = true; }
        ImGui::SameLine();
        if (ImGui::Button("SHADER", ImVec2(60, 0))) { iLevelIndex = ENUM_CLASS(LEVEL::SHADER); m_isLevelBtnPress = true; }

    }
    ImGui::EndChild();
    if (m_isLevelBtnPress)
        if (ImGui::Button("Add Model to Scene", ImVec2(200, 25))) {
            Add_Model(iLevelIndex);
            m_isLevelBtnPress = !m_isLevelBtnPress;
        }
    if (bDisabled) ImGui::EndDisabled();

    ImGui::Spacing();   ImGui::Separator();    ImGui::Spacing();

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


    ImGui::Spacing();   ImGui::Separator();    ImGui::Spacing();


    ImGui::SeparatorText("Update Data ( fixed .json -> dat )");
    if (ImGui::Button("Browse Model File....", ImVec2(200, 0)))
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
        ofn.lpstrFilter = "FBX Files\0*.dat\0All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = "../../../Client/Bin/Data/";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            string absolutePath = szFile;

            string relativePath = ConvertToClientRelativePath(absolutePath);

            Update_DataModel(relativePath);
        }

        SetCurrentDirectoryA(savedDir);
    }
    /* 설명  */
    ImGui::TextWrapped("Select a .dat file to update it with modified JSON files.");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        "This will merge changes from _Anim.json, _Summary_Anim.json, and _Material.json back into the .dat file.");

    ImGui::Spacing();   ImGui::Separator();    ImGui::Spacing();

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
            }

            SetCurrentDirectoryA(savedDir);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Save to: Client/Bin/Data/");
        }

        ImGui::Spacing();

        // ===== Update DAT From JSON 버튼 =====
        if (ImGui::Button("Update DAT From JSON ( Apply Model !! )", ImVec2(200, 25)))
        {
            _char savedDir[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, savedDir);

            OPENFILENAMEA ofn;
            _char szFile[260] = { 0 };

            // 기본 파일명
            string defaultName = WStringToAnsi(m_ObjectNames[m_iSelectedIndex]);
            strcpy_s(szFile, defaultName.c_str());

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Model Files\0*.dat\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrInitialDir = "../../Client/Bin/Data/";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn) == TRUE)
            {
                string absolutePath = szFile;
                string savePath = ConvertToRelativePath(absolutePath);

                m_GameObjects[m_iSelectedIndex]->get_Model()->Update_DAT_From_JSON(savePath);
            }

            SetCurrentDirectoryA(savedDir);
        }


        // ===== Road Data  버튼 =====
        if (ImGui::Button(" Load Model Data (.dat)", ImVec2(200, 25)))
        {
            _char savedDir[MAX_PATH];
            GetCurrentDirectoryA(MAX_PATH, savedDir);

            OPENFILENAMEA ofn;
            _char szFile[260] = { 0 };

            // 기본 파일명
            string defaultName = WStringToAnsi(m_ObjectNames[m_iSelectedIndex]);
            strcpy_s(szFile, defaultName.c_str());

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = "Model Files\0*.dat\0All Files\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrInitialDir = "../../Client/Bin/Data/";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileNameA(&ofn) == TRUE)
            {
                string absolutePath = szFile;
                string savePath = ConvertToRelativePath(absolutePath);

                m_GameObjects[m_iSelectedIndex]->get_Model()->LoadModel(savePath);
            }

            SetCurrentDirectoryA(savedDir);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Load .dat file after exporting model");
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

    if (m_isEnble_AnimList && !m_GameObjects.empty())
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
    if (m_isShowTool_ExportUpdate)Tool_Export_Update_Widget();

    /* Animation Controler*/
    if (ImGui::Button("Animation Controler", ImVec2(250, 25))) {
        m_isShowTool_Control = !m_isShowTool_Control;
        m_isEnble_AnimList = !m_isEnble_AnimList;
    }
    if (m_isShowTool_Control && m_iSelectedIndex > -1)Tool_AnimationControl_Widget();


}

void CAnimationTool::Tool_AnimationControl_Widget()
{
    ANIMATION_SETUP_DATA* animSetupData = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_CurAnimSet();
    ImGui::Text(("Cur Animation : " + animSetupData->strName).c_str());
    ImGui::Spacing();   ImGui::Separator();    ImGui::Spacing();


    // === 왼쪽: 애니메이션 리스트 ===
    ImGui::BeginChild("LeftPanel", ImVec2(m_pannelLeftWidth, 0), true);
    {
        Tool_AnimationList_Widget();
    }
    ImGui::EndChild();

    // === 중간: 애니메이션 정보 ===
    ImGui::SameLine();
    ImGui::BeginChild("MiddlePanel", ImVec2(m_pannelMiddleWidth, 0), true);
    {
        Tool_AnimationInfo_Widget();
    }
    ImGui::EndChild();

    // === 오른쪽: 컨트롤 ===
    ImGui::SameLine();
    ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
    {
        if (m_isEnble_MakeAnimSet) Tool_MakeAnimSet_Widget();
        if  (m_isEnble_AnimSlider&& m_isEnble_MakeAnimEvent)Tool_MakeAnimEvent_Widget();
    }
    ImGui::EndChild();

}

void CAnimationTool::Tool_AnimationList_Widget()
{
    // 모델 선택 확인
    if (m_iSelectedIndex < 0 || m_iSelectedIndex >= (_int)m_GameObjects.size())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Please select a model first!");
        return;
    }

    CEditor_Model* pModel = m_GameObjects[m_iSelectedIndex]->get_Model();
    if (pModel == nullptr)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "CEditor_Model == nullptr");
        return;
    }

    MODEL_DATA* modelData = pModel->Get_ModelData();
    _uint iNumAnimations = modelData->iNumAnimations;

    /* 애니메이션이 없음 */
    if (iNumAnimations == 0)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "This model has no animations!");
        return;
    }

    // 헤더 정보
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Model: %s", WStringToAnsi(m_ObjectNames[m_iSelectedIndex]).c_str());
    ImGui::Text("Total Animations: %d", iNumAnimations);
    ImGui::Separator();  ImGui::Spacing();

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

                m_fCurrentFrame = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_CurTrackPosition();
                m_pCurAnimaion = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_CurAnimtion();
                m_fCurrentDuration = m_pCurAnimaion->Get_Duration();
                m_isEnble_AnimSlider = true;
            }

            // 호버 시 간단한 정보
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Animation: %s", animName.c_str());
                ImGui::Text("Duration: %.5f sec", animData.fDuration / animData.fTickPerSecond);
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
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Selected: %d", m_iSelectedAnimIndex);
    }

}

void CAnimationTool::Tool_AnimationInfo_Widget()
{
    // 버튼 
    ImGui::BeginChild("AnimPanel", ImVec2(0, 30), true);
    {
        if (ImGui::Button("Info", ImVec2(60, 0))) m_isEnble_AnimInfo = !m_isEnble_AnimInfo;
        ImGui::SameLine();
        if (ImGui::Button("Set", ImVec2(60, 0))) m_isEnble_AnimSet = !m_isEnble_AnimSet;
        ImGui::SameLine();
        if (ImGui::Button("Root", ImVec2(60, 0))) m_isEnble_AnimRootMotion = !m_isEnble_AnimRootMotion;
        ImGui::SameLine();
        if (ImGui::Button("Event", ImVec2(60, 0))) m_isEnble_AnimEvent = !m_isEnble_AnimEvent;
        ImGui::SameLine();
        if (ImGui::Button("Make Event", ImVec2(80, 0))) {
            m_isEnble_MakeAnimEvent = !m_isEnble_MakeAnimEvent;
            m_isEnble_MakeAnimSet = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Make Set", ImVec2(80, 0))) {
            m_isEnble_MakeAnimSet = !m_isEnble_MakeAnimSet;
            m_isEnble_MakeAnimEvent = false;
        }
    }
    ImGui::EndChild();

    if (m_iSelectedIndex < 0 || m_iSelectedAnimIndex < 0)
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select an animation");
        return;
    }

    CEditor_Model* pModel = m_GameObjects[m_iSelectedIndex]->get_Model();
    if (!pModel) return;

    MODEL_DATA* modelData = pModel->Get_ModelData();
    ANIMATION_DATA* animData = &modelData->vecAnimation[m_iSelectedAnimIndex];
    ANIMATION_SETUP_DATA* setup = &animData->animSetup;

    // 기본 정보
    ImGui::SeparatorText("Animation Information");
    if (m_isEnble_AnimInfo)
    {
        ImGui::Text("strName: %s", setup->strName.c_str());
        if (ImGui::Checkbox("isLoop", &setup->isLoop))
        {
            _bool a = &setup->isLoop;
            int aaa = 0;
        }

        ImGui::Text("iDirection: %d", setup->iDirection);
        ImGui::SameLine();
        string  strDir = "  ->  " + DirectionToString(setup->iDirection);
        ImGui::Text(strDir.c_str());
        if (ImGui::Button("Clear", ImVec2(70.f, 0.f))) setup->iDirection = 0;  ImGui::SameLine();
        if (ImGui::Button("F", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::F);  ImGui::SameLine();
        if (ImGui::Button("B", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::B);  ImGui::SameLine();
        if (ImGui::Button("L", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::L);  ImGui::SameLine();
        if (ImGui::Button("R", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::R);  ImGui::SameLine();
        if (ImGui::Button("U", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::U);  ImGui::SameLine();
        if (ImGui::Button("D", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::D);  ImGui::SameLine();
        if (ImGui::Button("C", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::C);  ImGui::SameLine();
        if (ImGui::Button("CC", ImVec2(20.f, 0.f))) setup->iDirection += ENUM_CLASS(DIRECTION::CC);  ImGui::SameLine();
        if (ImGui::Button("ALL", ImVec2(25.f, 0.f))) setup->iDirection  += ENUM_CLASS(DIRECTION::ALL);  ImGui::SameLine();
        if (ImGui::Button("BBL", ImVec2(25.f, 0.f))) setup->iDirection  += ENUM_CLASS(DIRECTION::BBL);  ImGui::SameLine();
        if (ImGui::Button("BLL", ImVec2(25.f, 0.f))) setup->iDirection  += ENUM_CLASS(DIRECTION::BLL); 

        ImGui::Spacing();

        ImGui::Text("Rotation Type: %d", animData->iRotationType);
        ImGui::SameLine();
         strDir = "  ->  " + RotationToString(animData->iRotationType);
        ImGui::Text(strDir.c_str()); ImGui::SameLine();
        if (ImGui::Button("None", ImVec2(70.f, 0.f)))  animData->iRotationType = 0; 
        if (ImGui::Button("L 45", ImVec2(60.f, 0.f)))  animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_L45);  ImGui::SameLine();
        if (ImGui::Button("L 90", ImVec2(60.f, 0.f)))  animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_L90);  ImGui::SameLine();
        if (ImGui::Button("L 135", ImVec2(60.f, 0.f))) animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_L135);  ImGui::SameLine();
        if (ImGui::Button("L 180", ImVec2(60.f, 0.f))) animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_L180);  ImGui::SameLine();
        if (ImGui::Button("L 225", ImVec2(60.f, 0.f))) animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_L225); 
        if (ImGui::Button("R 45", ImVec2(60.f, 0.f)))  animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_R45);  ImGui::SameLine();
        if (ImGui::Button("R 90", ImVec2(60.f, 0.f)))  animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_R90);  ImGui::SameLine();
        if (ImGui::Button("R 135", ImVec2(60.f, 0.f))) animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_R135);  ImGui::SameLine();
        if (ImGui::Button("R 180", ImVec2(60.f, 0.f))) animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_R180);  ImGui::SameLine();
        if (ImGui::Button("R 225", ImVec2(60.f, 0.f))) animData->iRotationType = ENUM_CLASS(ANIM_ROTATIONTYPE::ROT_R225);  ImGui::SameLine();



        ImGui::Spacing();

        // 애니메이션 전환 설정
        ImGui::Text("=== Transition Settings ===");

        const char* transitionTypes[] = { "Auto", "Flag", "Input", "Manual" };
        _int currentType = setup->iTransitionType;
        ImGui::Text("Transition Type:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::Combo("##TransitionType", &currentType, transitionTypes, IM_ARRAYSIZE(transitionTypes)))
        {
            setup->iTransitionType = currentType;
        }
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip("이 애니메이션에서 다음 애니메이션으로 전환하는 조건");
        //}

        ImGui::Checkbox("Wait For Complete", &setup->isWaitForComplete);
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip("애니메이션이 끝난 후 다음으로 넘어갈지 여부");
        //}

        ImGui::Text("(Single Anim)Blend Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##BlendTime", &animData->fAnimationBlendTime, 0.01f, 0.0f, 5.0f, "%.3f");


        ImGui::Text("(Set Anim)Blend Out Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##BlendOut", &setup->fBlendOutTime, 0.01f, 0.0f, 5.0f, "%.3f");
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip("이 애니메이션이 종료될 때 블렌드 시간");
        //}

        ImGui::Text("(Set Anim)Blend In Time: ");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##BlendIn", &setup->fBlendInTime, 0.01f, 0.0f, 5.0f, "%.3f");
        //if (ImGui::IsItemHovered())
        //{
        //    ImGui::SetTooltip("이 애니메이션이 시작될 때 블렌드 시간");
        //}
    }
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // ===== 애니메이션 세트 (MODEL_DATA 레벨) =====
    ImGui::SeparatorText("Animation Sets (Model Level)");
    if (m_isEnble_AnimSet)
    {
        if (modelData->vecAnimationSets.size() > 0)
        {
            ImGui::Text("Total Animation Sets: %d", (_int)modelData->vecAnimationSets.size());
            ImGui::Spacing();

            for (_int i = 0; i < (_int)modelData->vecAnimationSets.size(); ++i)
            {
                ANIMATION_SET_DATA& animSet = modelData->vecAnimationSets[i];

                ImGui::PushID(i);

                if (ImGui::TreeNode(("Set [" + to_string(i) + "] " + animSet.strAnimSetName).c_str()))
                {
                    ImGui::Indent();

                    // strAnimSetName 변경
                    ImGui::Text("Set Name:");
                    ImGui::SameLine();
                    _char szSetNameBuffer[256];
                    strcpy_s(szSetNameBuffer, animSet.strAnimSetName.c_str());
                    ImGui::SetNextItemWidth(200);
                    if (ImGui::InputText("##SetName", szSetNameBuffer, sizeof(szSetNameBuffer)))
                    {
                        animSet.strAnimSetName = string(szSetNameBuffer);
                    }

                    ImGui::Spacing();

                    // vecAnimIndices - 애니메이션 리스트
                    ImGui::Text("Animations in Set: %d", (_int)animSet.vecAnimIndices.size());
                    if (ImGui::BeginChild(("AnimSetList" + to_string(i)).c_str(), ImVec2(0, 150), true))
                    {
                        for (_int j = 0; j < (_int)animSet.vecAnimIndices.size(); ++j)
                        {
                            _int animIdx = animSet.vecAnimIndices[j];
                            if (animIdx >= 0 && animIdx < (_int)modelData->vecAnimation.size())
                            {
                                string animName = modelData->vecAnimation[animIdx].strName;
                                ImGui::Text("[%d] Anim[%d]: %s", j, animIdx, animName.c_str());

                                ImGui::SameLine();
                                ImGui::PushID(j);
                                if (ImGui::SmallButton("Remove"))
                                {
                                    animSet.vecAnimIndices.erase(animSet.vecAnimIndices.begin() + j);
                                    ImGui::PopID();
                                    break;
                                }
                                ImGui::PopID();
                            }
                            else
                            {
                                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                    "[%d] Invalid Index: %d", j, animIdx);
                            }
                        }
                    }
                    ImGui::EndChild();

                    ImGui::Spacing();

                    //플레이 버튼  todo
                    if (ImGui::Button("Play. This Set", ImVec2(150, 0)))
                    {
                        m_GameObjects[m_iSelectedIndex]->get_Model()->Set_SetAnimation(modelData->vecAnimationSets[i].strAnimSetName);
                        ImGui::Unindent();
                        ImGui::TreePop();
                        ImGui::PopID();
                        break;
                    }

                    ImGui::Spacing();

                    // 세트 삭제 버튼
                    if (ImGui::Button("Delete This Set", ImVec2(150, 0)))
                    {
                        modelData->vecAnimationSets.erase(modelData->vecAnimationSets.begin() + i);
                        ImGui::Unindent();
                        ImGui::TreePop();
                        ImGui::PopID();
                        break;
                    }

                    ImGui::Unindent();
                    ImGui::TreePop();
                }
                else
                {
                    // 접혀있을 때 간략 정보
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                        "(%d animations)", (_int)animSet.vecAnimIndices.size());
                }

                ImGui::PopID();
                ImGui::Spacing();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Animation Sets");
        }

        ImGui::Spacing();    ImGui::Separator();    ImGui::Spacing();

        // 새 애니메이션 세트 추가
        //if (ImGui::Button("Create New Animation Set", ImVec2(200, 25)))
        //{
        //    ANIMATION_SET_DATA newSet;
        //    newSet.strAnimSetName = "NewAnimSet_" + to_string(modelData->vecAnimationSets.size());
        //    modelData->vecAnimationSets.push_back(newSet);
        //}
    }
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // ===== 루트 모션 =====
    ImGui::SeparatorText("Root Motion");
    if (m_isEnble_AnimRootMotion)
    {
        ImGui::Checkbox("isRootMotion", &setup->isRootMotion);
        if (setup->isRootMotion)
        {
            ImGui::Indent();
           if( ImGui::Button("Fast Standing Motion"))
            {
				setup->isApplyRootPosition = true;
				setup->isIgnoreRootRot = false;
				setup->isIgnoreRootPos = false;
				setup->isIgnoreRootPosFirstFrame = true;
				setup->isAbsoluteRootPosition = false;
				setup->RootMitionScale = FLOAT3_DATA(1.f, 1.f, 1.f);
            }
		   ImGui::SameLine();
           if (ImGui::Button("Fast Rotation Motion"))
           {
			   setup->isApplyRootPosition = true;
			   setup->isIgnoreRootRot = true;
			   setup->isIgnoreRootPos = true;
			   setup->isIgnoreRootPosFirstFrame = false;
			   setup->isAbsoluteRootPosition = false;
			   setup->RootMitionScale = FLOAT3_DATA(1.f, 1.f, 1.f);
           }
           ImGui::SameLine();
           if (ImGui::Button("Fast Absolute Motion (plz apply 2th.. )"))
           {
               setup->isApplyRootPosition = true;
               setup->isIgnoreRootRot = false;
               setup->isIgnoreRootPos = false;
               setup->isIgnoreRootPosFirstFrame = false;
               setup->isAbsoluteRootPosition = true;
               setup->RootMitionScale = FLOAT3_DATA(1.f, 1.f, 1.f);
           }
            ImGui::Checkbox("isApplyRoot.P.o.s.ition", &setup->isApplyRootPosition);
            ImGui::Checkbox("isIgnoreRoot_Rot", &setup->isIgnoreRootRot);
            ImGui::Checkbox("isIgnoreRoot_Pos", &setup->isIgnoreRootPos);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Ignore root bone position every frame.\nUse for: idle rotation!! , turning in place!!");
            
            ImGui::Checkbox("isIgnoreRootPosFirstFrame", &setup->isIgnoreRootPosFirstFrame);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Ignore root bone position only on first frame.\nUse for: idle walk, idle run, breathing");
            
            ImGui::Checkbox("isAbsoluteRootPosition", &setup->isAbsoluteRootPosition);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Root bone uses absolute position from animation file.\nUse for: combo attacks (Attack2, Attack3)\nKeeps position continuous between animations");
            
            ImGui::Text("RootMotionScale:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            ImGui::DragFloat3("##rootScale", &setup->RootMitionScale.x, 0.1f, 0.0f, 10.0f, "%.1f");
            if (ImGui::Button("Fast Choice[1.f, 1.f, 1.f]")) setup->RootMitionScale = FLOAT3_DATA(1.f, 1.f, 1.f);
            if (ImGui::Button("Fast Choice[0.f, 1.f, 1.f]")) setup->RootMitionScale = FLOAT3_DATA(0.f, 1.f, 1.f);
            if (ImGui::Button("Fast Choice[1.f, 0.f, 1.f]")) setup->RootMitionScale = FLOAT3_DATA(1.f, 0.f, 1.f);
            if (ImGui::Button("Fast Choice[1.f, 1.f, 0.f]")) setup->RootMitionScale = FLOAT3_DATA(1.f, 1.f, 0.f);

            ImGui::Unindent();
        }
    }
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // ===== 이벤트 =====
    ImGui::SeparatorText("Events");
    if (m_isEnble_AnimEvent)
    {
        ImGui::Checkbox("isEvent", &setup->isEvent);
        if (setup->isEvent)
        {
            ImGui::Indent();
            ImGui::Text("Events: %d", (_int)setup->vecEventKeys.size());

            for (size_t i = 0; i < setup->vecEventKeys.size(); ++i)
            {
                ImGui::PushID((_int)i);

                if (ImGui::TreeNode(("Event " + to_string(i)).c_str()))
                {
                    ImGui::Text("Key: %s", setup->vecEventKeys[i].c_str());
                    ImGui::Text("Event Key Change:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(200);
                    _char szKeyBuffer[256];
                    strcpy_s(szKeyBuffer, setup->vecEventKeys[i].c_str());
                    if (ImGui::InputText("##EventKeyChange", szKeyBuffer, sizeof(szKeyBuffer)))
                    {
                        setup->vecEventKeys[i] = string(szKeyBuffer);
                    }

                    ImGui::Text("Start Frame:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(150);
                    ImGui::DragFloat("##eventStart", &setup->vecEventFrames[i].x, 0.1f, 0.0f, animData->fDuration, "%.5f");

                    ImGui::Text("End Frame:  ");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(150);
                    ImGui::DragFloat("##eventEnd", &setup->vecEventFrames[i].y, 0.1f, 0.0f, animData->fDuration, "%.5f");


                    // 트리거 상태 표시
                    _uint& trigger = setup->vecEventTriggers[i];

                    // 트리거 버튼 정보 배열
                    struct TriggerButton {
                        const char* label;
                        int bit;
                        float width;
                    };

                    TriggerButton buttons[] = {
                        {"[Once]", 0, 60.f},
                        {"[OnEnter]", 1, 70.f},
                        {"[OnExit]", 2, 70.f},
                        {"[Continuous]", 3, 90.f}
                    };

                    for (int j = 0; j < 4; ++j)
                    {
                       // bool isActive = (trigger & (1 << buttons[j].bit)) != 0;

                        string buttonLabel = buttons[j].label;

                        if (ImGui::Button(buttonLabel.c_str(), ImVec2(buttons[j].width, 0.f)))
                        {
                            trigger ^= (1 << buttons[j].bit);  // XOR로 비트 토글
                        }

                        if (j < 3) ImGui::SameLine();
                    }

                    ImGui::Spacing();

                    // 현재 트리거 상태 표시
                    ImGui::Text(("Triggers: "+to_string(trigger)+ "  ").c_str());
                    ImGui::SameLine();
                    string triggerStr;
                    if (trigger & (1 << 0)) triggerStr += "[Once] ";
                    if (trigger & (1 << 1)) triggerStr += "[Enter] ";
                    if (trigger & (1 << 2)) triggerStr += "[Exit] ";
                    if (trigger & (1 << 3)) triggerStr += "[Continuous] ";
                    if (triggerStr.empty()) triggerStr = "None";
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", triggerStr.c_str());



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
                        "(%s: %.2f-%.2f)",
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
    }
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
}

void CAnimationTool::Tool_MakeAnimEvent_Widget()
{
    if (ImGui::Button("begin")) {
        *m_fCurrentFrame = 0;
        m_isPlaying = false;
        m_pCurAnimaion->EnbleTrackPosition(m_isPlaying);
    }
    ImGui::SameLine();
    if (ImGui::Button(m_isPlaying ? "pause" : "start")) {
        m_isPlaying = !m_isPlaying;
        m_pCurAnimaion->EnbleTrackPosition(m_isPlaying);
    }
    ImGui::SameLine();
    if (ImGui::Button("end")) {
        *m_fCurrentFrame = m_fCurrentDuration;
        m_isPlaying = false;
        m_pCurAnimaion->EnbleTrackPosition(m_isPlaying);
    }
    ImGui::SameLine();
    ImGui::Text("Frame : %.5f / %.5f", *m_fCurrentFrame, m_fCurrentDuration);
    ImGui::SliderFloat("##frame_slider", m_fCurrentFrame, 0, m_fCurrentDuration, "");

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    ImGui::Text("Event Key:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("##EventKeyInput", m_szEventKeyInputText, IM_ARRAYSIZE(m_szEventKeyInputText));

    if (ImGui::Button("Frame X Store"))
    {
        m_vTempFrames.x = *m_fCurrentFrame;
    }
    ImGui::SameLine();
    if (ImGui::Button("Frame Y Store"))
    {
        m_vTempFrames.y = *m_fCurrentFrame;
    }
    ImGui::SameLine();
    if (ImGui::Button("Once Event ( Frame Y = 0)"))
    {
        m_vTempFrames.y = 0.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear Frame (X = 0, Y = 0)"))
    {
        m_vTempFrames.x = 0.f;
        m_vTempFrames.y = 0.f;
    }

    //CEditor_Model* pModel = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_ModelData()->vecAnimation[m_iSelectedAnimIndex]->animSetup;
    //MODEL_DATA* modelData = pModel->Get_ModelData();
    //ANIMATION_DATA* animData = &modelData->vecAnimation[m_iSelectedAnimIndex];
    //ANIMATION_SETUP_DATA* setup = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_CurAnimSet();

    ImGui::Checkbox("isTriggerOnce", &m_isTriggerOnce); ImGui::SameLine();
    ImGui::Checkbox("isTriggerOnEnter", &m_isTriggerOnEnter); ImGui::SameLine();
    ImGui::Checkbox("isTriggerOnExit", &m_isTriggerOnExit); ImGui::SameLine();
    ImGui::Checkbox("isTriggerContinuous", &m_isTriggerContinuous);

    // ===== 현재 설정 정보 =====
    ImGui::BeginChild("CurrentEventInfo", ImVec2(0, 80), true);
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Current Event Setup:");
        ImGui::Text("Event Key: %s", m_szEventKeyInputText);
        ImGui::Text("Frame X: %.5f, Frame Y: %.5f", m_vTempFrames.x, m_vTempFrames.y);

        // 트리거 상태 표시
        ImGui::Text("Triggers: ");
        ImGui::SameLine();
        string triggerStr;
        if (m_isTriggerOnce) triggerStr += "[Once] ";
        if (m_isTriggerOnEnter) triggerStr += "[Enter] ";
        if (m_isTriggerOnExit) triggerStr += "[Exit] ";
        if (m_isTriggerContinuous) triggerStr += "[Continuous] ";
        if (triggerStr.empty()) triggerStr = "None";
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", triggerStr.c_str());
    }
    ImGui::EndChild();


    if (ImGui::Button("Add Event to List"))
    {
        m_vecEventFrames.emplace_back(m_vTempFrames);
        m_vecEventKeys.emplace_back(string(m_szEventKeyInputText));

        m_vTempFrames.x = 0.f;
        m_vTempFrames.y = 0.f;

        _uint   iTrigger = {};
        if (m_isTriggerOnce)iTrigger += (1 << 0);
        if (m_isTriggerOnEnter)iTrigger += (1 << 1);
        if (m_isTriggerOnExit)iTrigger += (1 << 2);
        if (m_isTriggerContinuous)iTrigger += (1 << 3);
        m_vecTriggers.emplace_back(iTrigger);
    }

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Event List (%d)", (_int)m_vecEventFrames.size());
    ImGui::BeginChild("EventList", ImVec2(0, 200), true);
    {
        for (_int i = 0; i < (_int)m_vecEventFrames.size(); ++i)
        {
            const _bool isSelected = (m_iAnimSliderListSelectedIndex == i);

            ImGui::PushID(i);

            // 트리거 플래그 파싱
            _uint trigger = m_vecTriggers[i];
            string triggerText;
            ImVec4 triggerColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);

            if (trigger & (1 << 0)) { // Once
                triggerText += "[Once]";
                triggerColor = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
            }
            if (trigger & (1 << 1)) { // Enter
                if (!triggerText.empty()) triggerText += " ";
                triggerText += "[Enter]";
                triggerColor = ImVec4(0.0f, 1.0f, 0.5f, 1.0f);
            }
            if (trigger & (1 << 2)) { // Exit
                if (!triggerText.empty()) triggerText += " ";
                triggerText += "[Exit]";
                triggerColor = ImVec4(1.0f, 0.0f, 0.5f, 1.0f);
            }
            if (trigger & (1 << 3)) { // Continuous
                if (!triggerText.empty()) triggerText += " ";
                triggerText += "[Cont]";
                triggerColor = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
            }
            if (triggerText.empty()) {
                triggerText = "[None]";
            }

            // 이벤트 정보 라벨
            string label = "[" + to_string(i) + "] " + m_vecEventKeys[i];

            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                m_iAnimSliderListSelectedIndex = i;
            }

            // 같은 줄에 프레임과 트리거 정보 표시
            ImGui::SameLine(250);
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                "X:%.3f Y:%.3f", m_vecEventFrames[i].x, m_vecEventFrames[i].y);

            ImGui::SameLine(350);
            ImGui::TextColored(triggerColor, "%s", triggerText.c_str());

            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    // 선택된 항목이 있을 때만 삭제 버튼 표시
    if (m_iAnimSliderListSelectedIndex >= 0 && m_iAnimSliderListSelectedIndex < (_int)m_vecEventFrames.size())
    {
        ImGui::Text("Selected: %s", m_vecEventKeys[m_iAnimSliderListSelectedIndex].c_str());
        if (ImGui::Button("Delete Selected"))
        {
            m_vecEventFrames.erase(m_vecEventFrames.begin() + m_iAnimSliderListSelectedIndex);
            m_vecEventKeys.erase(m_vecEventKeys.begin() + m_iAnimSliderListSelectedIndex);
            m_vecTriggers.erase(m_vecTriggers.begin() + m_iAnimSliderListSelectedIndex);
            m_iAnimSliderListSelectedIndex = -1;
        }
    }

    ImGui::Spacing();

    // 모델 데이터에 저장
    if (ImGui::Button("Save to Real Model Data", ImVec2(200, 25)))
    {
        //MODEL_DATA* data = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_ModelData();
        ANIMATION_SETUP_DATA* data = m_GameObjects[m_iSelectedIndex]->get_Model()->Get_CurAnimSet();
        data->vecEventFrames = m_vecEventFrames;
        data->vecEventKeys = m_vecEventKeys;
        data->vecEventTriggers = m_vecTriggers;
        data->isEvent = true;

        // 저장 후 초기화
        m_vecEventFrames.clear();
        m_vecEventKeys.clear();
        m_vecTriggers.clear();
        m_iAnimSliderListSelectedIndex = -1;
    }
}

void CAnimationTool::Tool_MakeAnimSet_Widget()
{
    if (m_iSelectedIndex < 0) return;

    CEditor_Model* pModel = m_GameObjects[m_iSelectedIndex]->get_Model();
    if (!pModel) return;

    MODEL_DATA* modelData = pModel->Get_ModelData();

    ImGui::SeparatorText("Create New Animation Set");

    // 애니메이션 세트 이름 입력
    ImGui::Text("Set Name:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(250);
    ImGui::InputText("##NewSetName", m_szAnimSetNameInputText, sizeof(m_szAnimSetNameInputText));

    ImGui::Spacing();


    // 현재 추가된 애니메이션 리스트
    ImGui::Text("Animations to Add: %d", (_int)m_AnimSet.size());
    if (ImGui::BeginChild("TempAnimSetList", ImVec2(0, 200), true))
    {
        for (_int i = 0; i < (_int)m_AnimSet.size(); ++i)
        {
            ImGui::Text("[%d] Anim[%d]: %s", i, m_AnimSet[i].first, m_AnimSet[i].second.c_str());

            ImGui::SameLine();
            ImGui::PushID(i);
            if (ImGui::SmallButton("Remove"))
            {
                m_AnimSet.erase(m_AnimSet.begin() + i);
                ImGui::PopID();
                break;
            }
            if (ImGui::SmallButton("Up") && i > 0)
            {
                swap(m_AnimSet[i], m_AnimSet[i - 1]);
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Down") && i < (_int)m_AnimSet.size() - 1)
            {
                swap(m_AnimSet[i], m_AnimSet[i + 1]);
            }
            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();

    // 애니메이션 추가 섹션
    ImGui::SeparatorText("Add Animation to Set");

    // 검색
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##Search", "Search...", m_szAnimSetSearchBuffer, sizeof(m_szAnimSetSearchBuffer));

    // 모든 애니메이션 리스트
    if (ImGui::BeginChild("AllAnimListForSet", ImVec2(0, 200), true))
    {
        for (_uint i = 0; i < modelData->iNumAnimations; ++i)
        {
            const ANIMATION_DATA& animData = modelData->vecAnimation[i];
            string animName = animData.strName;

            // 검색 필터
            if (strlen(m_szAnimSetSearchBuffer) > 0)
            {
                if (animName.find(m_szAnimSetSearchBuffer) == string::npos)
                    continue;
            }

            ImGui::PushID(i);

            // 이미 추가된 애니메이션인지 체크
            _bool alreadyAdded = false;
            for (const auto& pair : m_AnimSet)
            {
                if (pair.first == (_int)i)
                {
                    alreadyAdded = true;
                    break;
                }
            }

            if (alreadyAdded)
                ImGui::BeginDisabled();

            if (ImGui::Selectable(animName.c_str(), false))
            {
                m_AnimSet.push_back({ (_int)i, animName });
            }

            if (alreadyAdded)
            {
                ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[Added]");
            }

            ImGui::PopID();
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // 세트 생성 버튼
    _bool bCanCreate = (strlen(m_szAnimSetNameInputText) > 0 && !m_AnimSet.empty());

    if (!bCanCreate) ImGui::BeginDisabled();

    if (ImGui::Button("Create Animation Set", ImVec2(200, 30)))
    {
        ANIMATION_SET_DATA newSet;
        newSet.strAnimSetName = string(m_szAnimSetNameInputText);

        for (const auto& pair : m_AnimSet)
        {
            newSet.vecAnimIndices.push_back(pair.first);
        }

        modelData->vecAnimationSets.push_back(newSet);

        // 초기화
        m_AnimSet.clear();
        memset(m_szAnimSetNameInputText, 0, sizeof(m_szAnimSetNameInputText));
    }

    if (!bCanCreate) ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Clear All", ImVec2(100, 30)))
    {
        m_AnimSet.clear();
        memset(m_szAnimSetNameInputText, 0, sizeof(m_szAnimSetNameInputText));
    }
}

void CAnimationTool::Add_Model(_uint iLevelIndex)
{
    _wstring prototypeTag = TEXT("Prototype_Component_Editor_Model_") + m_strModelName;

    _matrix PreTransformMatrix = XMMatrixIdentity();
    PreTransformMatrix = XMMatrixScaling(m_vPreScale.x, m_vPreScale.y, m_vPreScale.z) * XMMatrixRotationY(XMConvertToRadians(180.0f));

    if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), prototypeTag,
        CEditor_Model::Create(m_pDevice, m_pContext,
            m_isAnim ? MODELTYPE::ANIM : MODELTYPE::NONANIM,
            m_strModelPath.c_str(), PreTransformMatrix))))
    {
        MSG_BOX(TEXT("모델 로드 실패!"));
        return;
    }

    CJOH_EditorModelTest::EDITORTESTMODEL_DESC desc;
    desc.strPrototypeTag = prototypeTag;
    desc.isAnim = m_isAnim;
    desc.renderGroup = static_cast<RENDERGROUP>(m_iCurrentRenderGroup);
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(iLevelIndex, TEXT("Layer_Model"),
        iLevelIndex, TEXT("Prototype_GameObject_Editor_Animation_TestModel"), TIME_CHANNEL::WORLD, &desc)))
    {
        MSG_BOX(TEXT("게임 오브젝트 클론 실패!"));
        return;
    }

    CGameObject* pGameObject = m_pGameInstance->Get_BackGameObject(iLevelIndex, TEXT("Layer_Model"));

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

    pObject->OnUnEnble();

    m_GameObjects.erase(m_GameObjects.begin() + m_iSelectedIndex);
    m_ObjectNames.erase(m_ObjectNames.begin() + m_iSelectedIndex);

    m_iSelectedIndex = -1;
}

void CAnimationTool::Update_DataModel(const string& strPath)
{

    /* 현재 실행파일 저장 */
    _char savedDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, savedDir);

    /* 실행파일 위치 Client/default 로 고정  */
    _char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    filesystem::path exeDir = filesystem::path(exePath).parent_path();
    //OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

    filesystem::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
    //OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

    filesystem::path clientDefaultDir = editorDefaultDir.parent_path().parent_path() / "Client" / "Default";
    // OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

    SetCurrentDirectoryA(clientDefaultDir.string().c_str());

    filesystem::path fullPath(strPath);
    string strDirectory = fullPath.parent_path().string() + "/";
    string strFileName = fullPath.stem().string();  // 확장자 제외

    // 모델 폴더 경로
    //string strModelFolder = strDirectory + "/" + strFileName + "/";

    // 파일 경로 생성
    string strDatPath = strDirectory + strFileName + ".dat";
    string strAnimJsonPath = strDirectory + strFileName + "_Anim.json";
    string strAnimSummaryJsonPath = strDirectory + strFileName + "_Summary_Anim.json";
    string strMaterialJsonPath = strDirectory + strFileName + "_Material.json";

    // .dat 파일 존재 확인
    if (!filesystem::exists(strDirectory))
    {
        _tchar szMessage[MAX_PATH] = {};
        swprintf_s(szMessage, TEXT(".dat 파일이 존재하지 않습니다!\n경로: %S"),
            strDatPath.c_str());
        MessageBox(nullptr, szMessage, TEXT("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    MODEL_DATA tempModelData;

    // 1. 기존 .dat 파일 로드
    {
        ifstream ifs(strDatPath, ios::binary);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT(".dat 파일 열기 실패"));
            return;
        }
        tempModelData.LoadBinary(ifs);
        ifs.close();
    }

    // 2. Animation JSON 로드 (파일이 있으면)
    if (filesystem::exists(strAnimJsonPath))
    {
        ifstream ifs(strAnimJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Animation JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 애니메이션 교체
        tempModelData.vecAnimation = j.get<vector<ANIMATION_DATA>>();
        tempModelData.iNumAnimations = static_cast<_uint>(tempModelData.vecAnimation.size());
    }

    // 3. Summary Animation JSON 로드 (파일이 있으면)
    if (filesystem::exists(strAnimSummaryJsonPath))
    {
        ifstream ifs(strAnimSummaryJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Summary Animation JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 애니메이션 세트 교체
        tempModelData.vecAnimationSets = j.get<ANIMATION_SUMMARIES_DATA>().vecAnimationSets;
    }


    // 4. Material JSON 로드 (파일이 있으면)
    if (filesystem::exists(strMaterialJsonPath))
    {
        ifstream ifs(strMaterialJsonPath);
        if (!ifs.is_open())
        {
            MSG_BOX(TEXT("Material JSON 파일 열기 실패"));
            return;
        }

        JSON j;
        ifs >> j;
        ifs.close();

        // 머티리얼 교체
        tempModelData.vecMaterials = j.get<vector<MATERIAL_DATA>>();
        tempModelData.iNumMaterials = static_cast<_uint>(tempModelData.vecMaterials.size());
    }

    // 4. 업데이트된 데이터를 .dat에 다시 저장
    {
        ofstream ofs(strDatPath, ios::binary);
        if (!ofs.is_open())
        {
            MSG_BOX(TEXT(".dat 파일 쓰기 실패"));
            return;
        }
        tempModelData.SaveBinary(ofs);
        ofs.close();
    }

    // 성공 메시지
    _tchar szMessage[MAX_PATH] = {};
    swprintf_s(szMessage, TEXT(".dat 파일 업데이트 완료!\n\n폴더: %S\n파일: %S.dat"),
        strDirectory.c_str(), strFileName.c_str());
    MessageBox(nullptr, szMessage, TEXT("Success"), MB_OK | MB_ICONINFORMATION);

    //실행파일 위치 복귀
    SetCurrentDirectoryA(savedDir);
}

string CAnimationTool::ConvertToRelativePath(const string& absolutePath)
{
    namespace fs = std::filesystem;

    try
    {
        _char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        fs::path exeDir = fs::path(exePath).parent_path();
        OutputDebugStringA(("[Editor .exe Dir] " + exeDir.string() + "\n").c_str());

        fs::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
        OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

        fs::path absPath = fs::absolute(absolutePath);
        OutputDebugStringA(("[Absolute Path] " + absPath.string() + "\n").c_str());

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

string CAnimationTool::ConvertToClientRelativePath(const string& absolutePath)
{
    namespace fs = std::filesystem;

    try
    {
        _char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        fs::path exeDir = fs::path(exePath).parent_path();
        OutputDebugStringA(("[Editor.exe Dir] " + exeDir.string() + "\n").c_str());

        fs::path editorDefaultDir = exeDir.parent_path().parent_path() / "Default";
        OutputDebugStringA(("[Editor Default Dir] " + editorDefaultDir.string() + "\n").c_str());

        fs::path clientDefaultDir = editorDefaultDir.parent_path().parent_path()/ "Client" / "Default";
        OutputDebugStringA(("[Client Default] " + clientDefaultDir.string() + "\n").c_str());

        fs::path absPath = fs::absolute(absolutePath);
        OutputDebugStringA(("[Absolute Path] " + absPath.string() + "\n").c_str());

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

string CAnimationTool::DirectionToString(_uint iDir)
{
    if (iDir == 0)
        return  "NONE";

    string result = "";

    static string StrDir[] =
    {
        "F"    ,
        "B"    ,
        "L"    ,
        "R"    ,
        "U"    ,
        "D"    ,
        "C"    ,
        "CC"   ,
        "ALL"  ,
        "BBL"  ,
        "BLL"  ,
        "END"  ,
    };

    unsigned long   mask = iDir;

    unsigned long  iIndex; // 결과 비트 위치
    while (mask != 0) {
        _BitScanForward(&iIndex, mask); // 오른쪽에서 첫 번째 1 위치
        result += StrDir[iIndex] + " ";
        mask &= mask - 1; // 가장 오른쪽 1 제거
    }

    return result;
}

string CAnimationTool::RotationToString(_uint iDir)
{
	if (iDir == 0)
		return  "NONE";

	string result = "";

	static string StrDir[] =
	{
        "L 45",
        "L 90",
        "L 135",
        "L 180",
        "L 225",
        "R 45",
        "R 90",
        "R 135",
        "R 180",
        "R 225",
	};

	unsigned long   mask = iDir;

	unsigned long  iIndex; // 결과 비트 위치
	while (mask != 0) {
		_BitScanForward(&iIndex, mask); // 오른쪽에서 첫 번째 1 위치
		result += StrDir[iIndex] + " ";
		mask &= mask - 1; // 가장 오른쪽 1 제거
	}

	return result;
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