#include "Level_AI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Perception.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "AIController_Edit.h"
#include "Edit_Monster.h"

CLevel_AI::CLevel_AI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_AI::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */


	if (FAILED(Ready_Edit_Objects()))
		return E_FAIL;


	m_pGameInstance->AddWidget(TEXT("AI"), [&]() {

		ImGui::Begin("AI TOOL", nullptr, ImGuiWindowFlags_MenuBar);

		static char szDefaultFileName[MAX_PATH] = "DefaultModelName.dat";
		ImGui::InputText("FilePath", szDefaultFileName, IM_ARRAYSIZE(szDefaultFileName));

		ImGui::Separator();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Layout")) {}
				if (ImGui::MenuItem("Load Layout")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("AITabs"))
		{
			if (ImGui::BeginTabItem("Perception"))
			{
				// Perception
				Show_Peception_Menu(szDefaultFileName);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("BlackBoard"))
			{
				// BlackBoard
				Show_BlackBoard_Menu(szDefaultFileName);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("BehaviorTree"))
			{
				// BehaviorTree
				Show_BehaviorTree_Menu(szDefaultFileName);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
		});


	return S_OK;
}

void CLevel_AI::Update(_float fTimeDelta)
{
	return;
}

HRESULT CLevel_AI::Render()
{
	SetWindowText(g_hWnd, TEXT("AI 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_AI::Ready_Edit_Objects()
{
	/*CEdit_Monster::GAMEOBJECT_DESC MonsterDesc{};
	MonsterDesc.fSpeedPerSec = 3.f;
	MonsterDesc.fRotationPerSec = 180.f;

	m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::AI),TEXT("Layer_Yeti"),
		ENUM_CLASS(LEVEL::AI),TEXT("Prototype_GameObject_Monster"), &MonsterDesc);*/

	return S_OK;
}

#pragma region Perception

void CLevel_AI::Show_Peception_Menu(const char* szDefaultFileName)
{
	ImGui::Text("Create SightDesc");
	ImGui::Separator();

	static vector<SIGHT_DESC> SightList;
	static _int iSelectedIndex = -1;
	// 몬스터 이름 
	static _char szMonsterType[128] = "DefaultMonster";
	// Callback Tag
	static vector<string> CallbackTags;
	static _char szCallbackTag[128];

	// 몬스터 타입 입력
	ImGui::InputText("Monster Type", szMonsterType, IM_ARRAYSIZE(szMonsterType));
	ImGui::Separator();

	ImGui::Text("Callback Tags:");
	ImGui::BeginChild("CallbackList", ImVec2(200, 100), true);
	for (int i = 0; i < CallbackTags.size(); ++i)
	{
		ImGui::Text("%d. %s", i + 1, CallbackTags[i].c_str());
		ImGui::SameLine();

		// 삭제 버튼
		std::string btnLabel = "X##" + std::to_string(i);
		if (ImGui::SmallButton(btnLabel.c_str()))
		{
			CallbackTags.erase(CallbackTags.begin() + i);
			break;
		}
	}
	ImGui::EndChild();

	// 새로운 콜백 추가
	ImGui::InputText("New Callback", szCallbackTag, IM_ARRAYSIZE(szCallbackTag));
	ImGui::SameLine();
	if (ImGui::Button("Add Callback") && strlen(szCallbackTag) > 0)
	{
		CallbackTags.push_back(szCallbackTag);
		strcpy_s(szCallbackTag, "");
	}
	ImGui::Separator();


	// 시야 목록
	ImGui::BeginChild("SightList",ImVec2(150,200),true);
	for (_uint i = 0; i < SightList.size(); i++)
	{
		_char Lable[128];
		sprintf_s(Lable, "%s_Sight", szMonsterType, i);

		if (ImGui::Selectable(Lable, i == iSelectedIndex))
			iSelectedIndex = i;
	}
	ImGui::EndChild();
	ImGui::SameLine();


	ImGui::BeginChild("SightEditor", ImVec2(0, 200), true);
	if (iSelectedIndex >= 0 && iSelectedIndex < SightList.size())
	{
		SIGHT_DESC& desc = SightList[iSelectedIndex];
		ImGui::DragFloat("Radius", &desc.fRadius, 0.1f, 0.f, 1000.f);
		ImGui::DragFloat("FOV (deg)", &desc.fFov, 1.f, 0.f, 180.f);
		ImGui::DragFloat("Lose Sight Time", &desc.fLoseSightTime, 0.1f, 0.f, 10.f);
		ImGui::DragFloat("Check Interval", &desc.fCheckInterval, 0.01f, 0.01f, 2.f);
		ImGui::DragFloat("Height Offset", &desc.fHeightOffset, 0.1f, -10.f, 10.f);
		//ImGui::DragFloat("AggroTime", &desc.fAggroTime, 0.1f, -10.f, 10.f);
		ImGui::Checkbox("Require Line of Sight", &desc.isRequireLineOfSight);
	}
	else
		ImGui::Text("No Sight selected.");
	ImGui::EndChild();

	ImGui::Separator();

	// 하단 버튼
	if (ImGui::Button("Add New Sight"))
	{
		SIGHT_DESC newDesc{};
		newDesc.fRadius = 5.f;
		newDesc.fFov = 90.f;
		newDesc.fLoseSightTime = 1.f;
		newDesc.fCheckInterval = 0.2f;
		newDesc.fHeightOffset = 1.7f;
		newDesc.isRequireLineOfSight = true;
		newDesc.fFovCos = cosf(XMConvertToRadians(newDesc.fFov * 0.5f));
        //newDesc.fAggroTime = 0.f;
		SightList.push_back(newDesc);
		iSelectedIndex = (_int)SightList.size() - 1;
	}

	/*ImGui::SameLine();
	if (ImGui::Button("Delete Selected"))
	{
		if (iSelectedIndex >= 0 && iSelectedIndex < SightList.size())
		{
			SightList.erase(SightList.begin() + iSelectedIndex);
			iSelectedIndex = -1;
		}
	}*/

	ImGui::SameLine();
	if (ImGui::Button("Save to JSON"))
	{
		Save_Perception(SightList, szMonsterType, szDefaultFileName, CallbackTags);
	}

	ImGui::SameLine();
	if (ImGui::Button("Load from JSON"))
	{
		Load_Perception(SightList, szDefaultFileName);
		iSelectedIndex = -1;
	}
}


AIPERCEPTION_DATA CLevel_AI::Convert_AIPerceptionData(const SIGHT_DESC& Desc, const string& MonsterType, const string& FileName, const vector<string>& Callbacks)
{
	AIPERCEPTION_DATA Data;
	Data.strMonsterType = MonsterType;
	Data.strFileName = FileName;
	Data.CallbackTags = Callbacks;
	Data.fRadius = Desc.fRadius;
	Data.fFov = Desc.fFov;
	Data.fLoseSightTime = Desc.fLoseSightTime;
	Data.fCheckInterval = Desc.fCheckInterval;
	Data.fFovCos = cosf(Desc.fFov * 0.5f * XM_PI / 180.0f);
	Data.isRequireLineOfSight = Desc.isRequireLineOfSight;
	Data.fHeightOffset = Desc.fHeightOffset;

	return Data;
}

void CLevel_AI::Save_Perception(const vector<SIGHT_DESC>& SightList, const string& MonsterType, const string& FileName, const vector<string>& Callbacks)
{
	vector<AIPERCEPTION_DATA> SaveList;
	for (auto& Desc : SightList)
	{
		AIPERCEPTION_DATA Data{};
		Data.strMonsterType = MonsterType;
		Data.strFileName = FileName;
		Data.CallbackTags = Callbacks;
		Data.fRadius = Desc.fRadius;
		Data.fFov = Desc.fFov;
		Data.fLoseSightTime = Desc.fLoseSightTime;
		Data.fCheckInterval = Desc.fCheckInterval;
		Data.fFovCos = cosf(Desc.fFov * 0.5f * XM_PI / 180.0f);
		Data.isRequireLineOfSight = Desc.isRequireLineOfSight;
		Data.fHeightOffset = {};
        //Data.fAggroHoldTime = Desc.fAggroTime;
		SaveList.push_back(Data);
	}
		
	
	JSON j = SaveList;
	
	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP,0,FileName.c_str(),-1,szFileName,MAX_PATH);
	wstring basePath = TEXT("../../Client/Bin/Data/Monster/Perception/");

	wstring strFullPath = basePath + szFileName + TEXT("/") + szFileName;

	if (strFullPath.find(TEXT(".json")) == wstring::npos)
		strFullPath += TEXT(".json");

	_char FullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, FullPath, MAX_PATH, NULL, NULL);


	ofstream ofs(FullPath);
	if (ofs.is_open())
	{
		ofs << setw(4) << j;
		ofs.close();
	}
}

void CLevel_AI::Load_Perception(vector<SIGHT_DESC>& SightList, const string& FileName)
{
	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), -1, szFileName, MAX_PATH);

	wstring basePath = TEXT("../../Client/Bin/Data/Monster/Perception/");
	wstring strFullPath = basePath + szFileName + TEXT("/") + szFileName;

	if (strFullPath.find(TEXT(".json")) == wstring::npos)
		strFullPath += TEXT(".json");

	_char FullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, FullPath, MAX_PATH, NULL, NULL);

	ifstream ifs(FullPath);
	if (!ifs.is_open())
		return;

	JSON j;
	ifs >> j;
	ifs.close();

	SightList.clear();

	for (auto& elem : j)
	{
		SIGHT_DESC Desc{};
		Desc.fRadius = elem.value("fRadius", 5.f);
		Desc.fFov = elem.value("fFov", 90.f);
		Desc.fLoseSightTime = elem.value("fLoseSightTime", 1.f);
		Desc.fCheckInterval = elem.value("fCheckInterval", 0.2f);
		Desc.fHeightOffset = elem.value("fHeightOffset", 1.7f);
		Desc.isRequireLineOfSight = elem.value("isRequireLineOfSight", true);
		Desc.fFovCos = cosf(XMConvertToRadians(Desc.fFov * 0.5f));
        //Desc.fAggroTime = elem.value("fAggroHoldTime",1.f);
		SightList.push_back(Desc);
	}
}
#pragma endregion

#pragma region BlackBoard

void CLevel_AI::Show_BlackBoard_Menu(const char* szDefaultFileName)
{
	ImGui::Text("Create BlackBoard");
	ImGui::Separator();

	static vector<AIBLACKBOARD_DATA> BBList;
	static _int iSelectedIndex = -1;

	static _char szMonsterType[128] = "DefaultMonster";
	ImGui::InputText("Monster Type", szMonsterType, IM_ARRAYSIZE(szMonsterType));
	ImGui::Separator();

	// 블랙보드 리스트
	ImGui::BeginChild("BBList", ImVec2(150, 200), true);
	for (_uint i = 0; i < BBList.size(); i++)
	{
		_char Label[128];
		sprintf_s(Label, "%s_BB", BBList[i].strMonsterType.c_str());

		if (ImGui::Selectable(Label, i == iSelectedIndex))
			iSelectedIndex = i;
	}
	ImGui::EndChild();
	ImGui::SameLine();


	ImGui::BeginChild("BBEditor", ImVec2(0, 200), true);
	if (iSelectedIndex >= 0 && iSelectedIndex < BBList.size())
	{
		AIBLACKBOARD_DATA& BBData = BBList[iSelectedIndex];

		ImGui::Text("BlackBoard Values");
		ImGui::Separator();

		for (auto& [key, val] : BBData.Values)
		{
			if (val.is_boolean())
			{
				_bool b = val.get<_bool>();
				if (ImGui::Checkbox(key.c_str(), &b))
					val = b;
			}
			else if (val.is_number_float())
			{
				_float f = val.get<_float>();
				if (ImGui::DragFloat(key.c_str(), &f, 0.1f))
					val = f;
			}
			else if (val.is_number_unsigned())
			{
				_uint n = val.get<_uint>();
				if (ImGui::DragInt(key.c_str(), reinterpret_cast<int*>(&n), 1))
					val = n;
			}
			else if (val.is_array() && val.size() == 3)
			{
				_float3 vec = { val[0].get<_float>(), val[1].get<_float>(), val[2].get<_float>() };
				if (ImGui::DragFloat3(key.c_str(), &vec.x, 0.1f))
					val = { vec.x, vec.y, vec.z };
			}
			else if (val.is_string()) 
			{
				string objName = val.get<string>();
				static char buf[128];
				strcpy_s(buf, objName.c_str());
				if (ImGui::InputText(key.c_str(), buf, IM_ARRAYSIZE(buf)))
					val = string(buf);
			}
		}

		// 새로운 값 추가
		static char szNewKey[64] = "";
		static int iNewType = 0;
		ImGui::InputText("New Key", szNewKey, IM_ARRAYSIZE(szNewKey));
		ImGui::Combo("Type", &iNewType, "Bool\0Uint\0Float\0Float3\0GameObject\0");
		if (ImGui::Button("Add Value"))
		{
			if (strlen(szNewKey) > 0 && BBData.Values.find(szNewKey) == BBData.Values.end())
			{
				switch (iNewType)
				{
				case 0: BBData.Values[szNewKey] = false;
					break;
				case 1: BBData.Values[szNewKey] = 0; 
					break;
				case 2: BBData.Values[szNewKey] = 0.f; 
					break;
				case 3: BBData.Values[szNewKey] = { 0.f, 0.f, 0.f }; 
					  break; 
				case 4: BBData.Values[szNewKey] = string(""); 
					break;				
				}
			}
		}
	}
	else
	{
		ImGui::Text("No BlackBoard selected.");
	}
	ImGui::EndChild();

	ImGui::Separator();

	// 하단 버튼
	if (ImGui::Button("Add New BB"))
	{
		AIBLACKBOARD_DATA newBB;
		newBB.strMonsterType = szMonsterType;
		BBList.push_back(newBB);
		iSelectedIndex = (_int)BBList.size() - 1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete Selected"))
	{
		if (iSelectedIndex >= 0 && iSelectedIndex < BBList.size())
		{
			BBList.erase(BBList.begin() + iSelectedIndex);
			iSelectedIndex = -1;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Save to JSON"))
	{
		Save_BlackBoard(BBList, szDefaultFileName);
	}

	ImGui::SameLine();
	if (ImGui::Button("Load from JSON"))
	{
		Load_BlackBoard(BBList, szDefaultFileName);
		iSelectedIndex = -1;
	}
}

void CLevel_AI::Save_BlackBoard(const vector<AIBLACKBOARD_DATA>& BBList, const string& FileName)
{
	JSON j = BBList;

	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), -1, szFileName, MAX_PATH);
	wstring basePath = TEXT("../../Client/Bin/Data/Monster/BlackBoard/");

	wstring strFullPath = basePath + szFileName + TEXT("/") + szFileName;

	if (strFullPath.find(TEXT(".json")) == wstring::npos)
		strFullPath += TEXT(".json");

	_char FullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, FullPath, MAX_PATH, NULL, NULL);


	ofstream ofs(FullPath);
	if (ofs.is_open())
	{
		ofs << setw(4) << j;
		ofs.close();
	}
}
void CLevel_AI::Load_BlackBoard(vector<AIBLACKBOARD_DATA>& BBList, const string& FileName)
{
	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), -1, szFileName, MAX_PATH);

	wstring basePath = TEXT("../../Client/Bin/Data/Monster/BlackBoard/");
	wstring strFullPath = basePath + szFileName + TEXT("/") + szFileName;

	if (strFullPath.find(TEXT(".json")) == wstring::npos)
		strFullPath += TEXT(".json");

	_char FullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, FullPath, MAX_PATH, NULL, NULL);

	ifstream ifs(FullPath);
	if (!ifs.is_open())
		return;

	JSON j;
	ifs >> j;
	ifs.close();

	BBList = j.get<vector<AIBLACKBOARD_DATA>>();
}

#pragma endregion

#pragma region BehaviorTree

static AIBTNODE_DATA* g_SelectedNode = nullptr;

void CLevel_AI::Show_BehaviorTree_Menu(const char* szDefaultFileName)
{
	static AI_BTDATA BTData{};
	static char szMonsterTag[64] = "";
	ImGui::InputText("MonsterTag", szMonsterTag, IM_ARRAYSIZE(szMonsterTag));
	BTData.MonsterType = szMonsterTag;

	ImGui::Separator();

	if (ImGui::Button("New Tree"))
	{
		BTData = {};
		BTData.MonsterType = szMonsterTag;
		g_SelectedNode = nullptr;
	}

	ImGui::SameLine();

	if (ImGui::Button("Load"))
	{
		 Load_BehaviorTree(szDefaultFileName, BTData);
		g_SelectedNode = nullptr;
	}

	ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
		Save_BehaviorTree(BTData, szDefaultFileName);
	}

	ImGui::Separator();

	Show_BT_Editor(BTData);
}

void CLevel_AI::Show_BT_Editor(AI_BTDATA& TreeData)
{
	ImGui::BeginChild("Hierarchy", ImVec2(300, 0), true);
	if (TreeData.RootNode.strNodeName.empty())
	{
		if (ImGui::Button("Create Root Node"))
		{
			TreeData.RootNode.strNodeName = "Root";
			TreeData.RootNode.strNodeType = "Composite";
			TreeData.RootNode.strSubtype = "Selector";
		}
	}
	else
	{
		Show_BTNode_Hierarchy(TreeData.RootNode);
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Inspector", ImVec2(0, 0), true);
	if (g_SelectedNode)
	{
		// 이름
		static _char name[128];
		strcpy_s(name, g_SelectedNode->strNodeName.c_str());
		if (ImGui::InputText("Name", name, IM_ARRAYSIZE(name)))
			g_SelectedNode->strNodeName = name;

		// 노드 타입
		const _char* NodeTypes[] = { "Composite", "Decorator", "Leaf" };
		_uint iCurrentType = 0;
		for (_uint i = 0; i < 3; i++)
			if (g_SelectedNode->strNodeType == NodeTypes[i]) iCurrentType = i;

		if (ImGui::BeginCombo("Node Type", NodeTypes[iCurrentType]))
		{
			for (int i = 0; i < 3; i++)
			{
				_bool isSelected = (iCurrentType == i);
				if (ImGui::Selectable(NodeTypes[i], isSelected))
					g_SelectedNode->strNodeType = NodeTypes[i];
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		// 서브 타입

		const char* SubTypes[] = { "Selector", "Sequence", "InterruptibleSelector", "UtilitySelector" ,"Repeater", "Inverter", "CoolDown","UtilityAction" ,"Condition","Action","Wait"};
		_uint iCurrentSub = 0;
		for (_uint i = 0; i < 11; i++)
			if (g_SelectedNode->strSubtype == SubTypes[i])
				iCurrentSub = i;

		if (ImGui::BeginCombo("SubType", SubTypes[iCurrentSub]))
		{
			for (_uint i = 0; i < 11; i++)
			{
				_bool isSelected = (iCurrentSub == i);
				if (ImGui::Selectable(SubTypes[i], isSelected))
					g_SelectedNode->strSubtype = SubTypes[i];

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (g_SelectedNode->strNodeType == "Composite")
		{
			if (g_SelectedNode->strSubtype == "InterruptibleSelector")
			{
				static char cb[128];
				strcpy_s(cb, g_SelectedNode->strCallbackFunction.c_str());
				if (ImGui::InputText("Interrupt Callback", cb, IM_ARRAYSIZE(cb)))
					g_SelectedNode->strCallbackFunction = cb;
			}
		}

		if (g_SelectedNode->strNodeType == "Leaf")
		{
			if (g_SelectedNode->strSubtype == "Wait")
				ImGui::InputFloat("Wait Time", &g_SelectedNode->fWaitTime);
			else
			{
				static char cb[128];
				strcpy_s(cb, g_SelectedNode->strCallbackFunction.c_str());
				if (ImGui::InputText("Callback", cb, IM_ARRAYSIZE(cb)))
					g_SelectedNode->strCallbackFunction = cb;
			}
		}
		else if (g_SelectedNode->strNodeType == "Decorator")
		{
			if (g_SelectedNode->strSubtype == "CoolDown")
				ImGui::InputFloat("CoolDown", &g_SelectedNode->fCoolDownTime);
			else if (g_SelectedNode->strSubtype == "Repeater")
				ImGui::InputInt("Repeat Count", (int*)&g_SelectedNode->iRepeatCount);
		}

		// Children 관리
		_bool isCanAddChild = (g_SelectedNode->strNodeType != "Leaf") &&
			(g_SelectedNode->strNodeType != "Decorator" || g_SelectedNode->Children.size() < 1);

		if (!isCanAddChild) ImGui::BeginDisabled();

		if (ImGui::Button("Add Child"))
		{
			auto* pNewNode = new AIBTNODE_DATA();
			pNewNode->strNodeName = "NewNode";
			pNewNode->strNodeType = "Leaf";
			pNewNode->strSubtype= "Action";
			pNewNode->Parent = g_SelectedNode;
			g_SelectedNode->Children.push_back(pNewNode);

		}
		if (!isCanAddChild) ImGui::EndDisabled();

		if (!g_SelectedNode->Children.empty())
		{
			if (ImGui::Button("Remove Last Child"))
			{
				auto* pLast = g_SelectedNode->Children.back();
				g_SelectedNode->Children.pop_back();
				delete pLast;
			}
		}
	}
	ImGui::EndChild();

}

void CLevel_AI::Show_BTNode_Hierarchy(AIBTNODE_DATA& Node)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (&Node == g_SelectedNode) flags |= ImGuiTreeNodeFlags_Selected;

	_bool isOpen = ImGui::TreeNodeEx(Node.strNodeName.c_str(), flags);

	if (ImGui::IsItemClicked())
		g_SelectedNode = &Node;

	// 드래그
	AIBTNODE_DATA* pNodePtr = &Node;
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("BTNODE", &pNodePtr, sizeof(AIBTNODE_DATA*));
		ImGui::Text("Move: %s", Node.strNodeName.c_str());
		ImGui::EndDragDropSource();
	}
	// 드롭 대상 선택
	if (ImGui::BeginDragDropTarget())
	{
		// 드래그한 노드를 받아서 
		if (const ImGuiPayload* Payload = ImGui::AcceptDragDropPayload("BTNODE"))
		{
			AIBTNODE_DATA* DraggedNode = *(AIBTNODE_DATA**)Payload->Data;

			if (DraggedNode->Parent == Node.Parent)
			{
				// 같은 부모 안에서 순서 변경
				auto& siblings = Node.Parent->Children;
				_uint newIndex = 0;
				for (_uint i = 0; i < siblings.size(); ++i)
				{
					if (siblings[i] == &Node) { newIndex = i; break; }
				}
				if (!Change_Hierarchy(DraggedNode, newIndex))
					MSG_BOX(TEXT("Failed Change_Hierarchy"));
			}
			else
			{
				// 다른 부모면 이동
				if (!MoveNode(Node, *DraggedNode))
					MSG_BOX(TEXT("Failed MoveNode"));
			}


		}
		ImGui::EndDragDropTarget();
	}

	if (isOpen)
	{
		for (auto& child : Node.Children)
			Show_BTNode_Hierarchy(*child);

		ImGui::TreePop();
	}
}


void CLevel_AI::Save_BehaviorTree(const AI_BTDATA& Data, const string& FileName)
{
	JSON jRoot;
	jRoot["MonsterType"] = Data.MonsterType;
	JSON jNode;
	SaveNode(jNode, &Data.RootNode);
	jRoot["RootNode"] = jNode;

	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), -1, szFileName, MAX_PATH);
	wstring basePath = TEXT("../../Client/Bin/Data/Monster/BehaviorTree/");

	wstring strFullPath = basePath + szFileName + TEXT("/") + szFileName;

	if (strFullPath.find(TEXT(".json")) == wstring::npos)
		strFullPath += TEXT(".json");

	_char FullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, FullPath, MAX_PATH, NULL, NULL);

	ofstream ofs(FullPath, ios::out | ios::trunc);
	if (ofs.is_open())
	{
		ofs << jRoot.dump(4);
		ofs.close();
	}
}

void CLevel_AI::Load_BehaviorTree(const string& FileName, AI_BTDATA& OutData)
{
	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP, 0, FileName.c_str(), -1, szFileName, MAX_PATH);

	wstring strBasePath = TEXT("../../Client/Bin/Data/Monster/BehaviorTree/");
	wstring strFullPath = strBasePath + szFileName + TEXT("/") + szFileName;

	if (strFullPath.find(TEXT(".json")) == wstring::npos)
		strFullPath += TEXT(".json");

	_char FullPath[MAX_PATH] = {};
	WideCharToMultiByte(CP_ACP, 0, strFullPath.c_str(), -1, FullPath, MAX_PATH, NULL, NULL);

	ifstream ifs(FullPath);
	if (!ifs.is_open())
		return;

	JSON jRoot;
	ifs >> jRoot;
	ifs.close();

	OutData.MonsterType = jRoot.value("MonsterType", "");
	if (jRoot.contains("RootNode"))
		LoadNode(jRoot["RootNode"], nullptr, OutData.RootNode);
}

void CLevel_AI::SaveNode(JSON& j, const AIBTNODE_DATA* pNode)
{
	j["NodeType"] = pNode->strNodeType;
	j["SubType"] = pNode->strSubtype;
	j["NodeName"] = pNode->strNodeName;

	if (pNode->strNodeType == "Composite")
	{
		if (pNode->strSubtype == "InterruptibleSelector")
		{
			j["Callback"] = pNode->strCallbackFunction;
		}
	}


	if (pNode->strNodeType == "Leaf")
	{
		if (pNode->strSubtype == "Wait" && pNode->fWaitTime > 0.f)
			j["WaitTime"] = pNode->fWaitTime;
		else if (!pNode->strCallbackFunction.empty())
			j["Callback"] = pNode->strCallbackFunction;
	}
	else if (pNode->strNodeType == "Decorator")
	{
		if (pNode->strSubtype == "CoolDown" && pNode->fCoolDownTime > 0.f)
			j["CoolDownTime"] = pNode->fCoolDownTime;
		else if (pNode->strSubtype == "RepeatCount" && pNode->iRepeatCount > 0)
			j["RepeatCount"] = pNode->iRepeatCount;
	}

	if (!pNode->Children.empty())
	{
		j["Children"] = JSON::array();
		for (auto* child : pNode->Children)
		{
			JSON jChild;
			SaveNode(jChild, child);
			j["Children"].push_back(jChild);
		}
	}
}


void CLevel_AI::LoadNode(const JSON& j, AIBTNODE_DATA* pParent, AIBTNODE_DATA& OutNode)
{
	OutNode.strNodeType = j.value("NodeType", "");
	OutNode.strSubtype = j.value("SubType", "");
	OutNode.strNodeName = j.value("NodeName", "");
	OutNode.strCallbackFunction = j.value("Callback", "");
	OutNode.fCoolDownTime = j.value("CoolDownTime", 0.f);
	OutNode.fWaitTime = j.value("WaitTime", 0.f);
	OutNode.iRepeatCount = j.value("RepeatCount", 0);

	OutNode.Parent = pParent;

	if (j.contains("Children"))
	{
		for (auto& jc : j["Children"])
		{
			auto* newChild = new AIBTNODE_DATA();
			LoadNode(jc, &OutNode, *newChild);
			OutNode.Children.push_back(newChild);
		}
	}
}

_bool CLevel_AI::MoveNode(AIBTNODE_DATA& NewParentNode, AIBTNODE_DATA& DraggedNode)
{

	// 자기 자신이나 자손을 부모로 처리하지 못하게 체크
	AIBTNODE_DATA* iter = &NewParentNode;

	while (iter)
	{
		if (iter == &DraggedNode)
			return false;
		iter = iter->Parent;
	}

	// Leaf는 자식 X
	if (NewParentNode.strNodeType == "Leaf")
		return false;
	// Decorator일때 비어있지 않으면 false로 체크 하나만 허용
	if (NewParentNode.strNodeType == "Decorator" && !NewParentNode.Children.empty())
		return false;

	// 기존 부모에서 제거
	if (DraggedNode.Parent)
		RemoveNodeFromParent(&DraggedNode);
	
	
	// 새로운 부모에 붙이기
	/*NewParentNode.Children.push_back(DraggedNode);
	NewParentNode.Children.back().Parent = &NewParentNode;*/

	NewParentNode.Children.push_back(&DraggedNode);
	DraggedNode.Parent = &NewParentNode;
	g_SelectedNode = &DraggedNode;

	return true;

}

_bool CLevel_AI::RemoveNodeFromParent(AIBTNODE_DATA* pNode)
{
	// 부모 포인터로 접근해서 자기 자신을 지우게 하도록한다.
	if (nullptr == pNode || nullptr == pNode->Parent)
		return false;

	if (g_SelectedNode == pNode)
		g_SelectedNode = nullptr;

	auto& Siblings = pNode->Parent->Children;

	Siblings.erase(remove(Siblings.begin(), Siblings.end(), pNode), Siblings.end());

	pNode->Parent = nullptr;

	return true;
}

void CLevel_AI::RemoveNodeRecursive(AIBTNODE_DATA* pNode)
{
	if (nullptr == pNode)
		return;

	for (auto& pChild : pNode->Children)
		RemoveNodeRecursive(pChild);

	pNode->Children.clear();

	// 부모와 연결 해제
	if (pNode->Parent)
	{
		auto& siblings = pNode->Parent->Children;

		siblings.erase(std::remove(siblings.begin(), siblings.end(), pNode), siblings.end());
		pNode->Parent = nullptr;
	}

	//자기 자신 제거
	delete pNode;
}

_bool CLevel_AI::Change_Hierarchy(AIBTNODE_DATA* pNode, _uint iNewIndex)
{
	if (!pNode || !pNode->Parent)
		return false;

	auto& siblings = pNode->Parent->Children;

	_int iOldIndex = -1;
	for (_uint i = 0; i < siblings.size(); i++)
	{
		if (siblings[i] == pNode)
		{
			iOldIndex = i;
			break;
		}
	}
	if (iOldIndex == -1 || iOldIndex == iNewIndex)
		return false;

	auto pNodePtr = siblings[iOldIndex];
	siblings.erase(siblings.begin() + iOldIndex);

	if (iNewIndex > siblings.size())
		iNewIndex = siblings.size();

	siblings.insert(siblings.begin() + iNewIndex, pNodePtr);

	return true;
}

#pragma endregion



CLevel_AI* CLevel_AI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_AI* pInstance = new CLevel_AI(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_AI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_AI::Free()
{
	__super::Free();

}
