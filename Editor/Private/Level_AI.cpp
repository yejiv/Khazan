#include "Level_AI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Perception.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"

CLevel_AI::CLevel_AI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_AI::Initialize()
{
	/* 현재 레벨을 구성해주기 위한 객체들을 생성한다. */


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
		SightList.push_back(newDesc);
		iSelectedIndex = (_int)SightList.size() - 1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete Selected"))
	{
		if (iSelectedIndex >= 0 && iSelectedIndex < SightList.size())
		{
			SightList.erase(SightList.begin() + iSelectedIndex);
			iSelectedIndex = -1;
		}
	}

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

		SaveList.push_back(Data);
	}
		
	
	JSON j = SaveList;
	
	_tchar szFileName[MAX_PATH] = {};
	MultiByteToWideChar(CP_ACP,0,FileName.c_str(),-1,szFileName,MAX_PATH);
	wstring strFullPath = TEXT("../../Client/Bin/Data/Monster/Perception/") + wstring(szFileName);

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

void CLevel_AI::Load_Perception(const vector<SIGHT_DESC>& SightList, const string& FilePath)
{

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
			// CGameObject*를 문자열 ID로 저장
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
	wstring strFullPath = TEXT("../../Client/Bin/Data/Monster/Blackboard/") + wstring(szFileName);

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

}
#pragma endregion

#pragma region BehaviorTree

void CLevel_AI::Show_BehaviorTree_Menu(const char* szDefaultFileName)
{

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
