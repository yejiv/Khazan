#include "Edit_Interface_UI.h"
#include "GameInstance.h"
#include "Edit_UIBackGround.h"
#include "Edit_UIBase.h"

CEdit_Interface_UI::CEdit_Interface_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance{ CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

void CEdit_Interface_UI::Update_UIInterface(_float fTimeDelta)
{
	ImGui::Begin("UI_InterFace");
	Update_BackColor(fTimeDelta);
	Create_UI();
	SaveLoad_UI();
	ImGui::End();

	Selete_UI(fTimeDelta);
	for (auto pRootUi : m_pRootUIs)
		pRootUi->Late_Update(fTimeDelta);
}

HRESULT CEdit_Interface_UI::Initialize(LEVEL eLevel)
{
	m_iUISize[0] = g_iWinSizeX;
	m_iUISize[1] = g_iWinSizeY;
	m_eLevel = eLevel;

	strcpy_s(m_szPrototypePath, "Prototype_Component_");
	strcpy_s(m_szFilePath, "../../Client/Bin/Resources/UI/UIData/");

	CHECK_FAILED(Ready_Object(eLevel), E_FAIL);

	return S_OK;
}

HRESULT CEdit_Interface_UI::Ready_Object(LEVEL eLevel)
{
	/* Prototype_GameObject_UI_BackGround */
	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), TEXT("Prototype_GameObject_UI_BackGround"),
		CEdit_UIBackGround::Create(m_pDevice, m_pContext)), E_FAIL);

	m_RenderBackGround = false;
	CUIObject::UIOBJECT_DESC Desc;
	Desc.fDepth = 1.f;
	Desc.iUIType = 0;
	Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };
	Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
	m_pBackGround = static_cast<CEdit_UIBackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(eLevel), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));

	if (m_pBackGround == nullptr)
		return E_FAIL;

	CHECK_FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), TEXT("Prototype_GameObject_UI_Base"),
		CEdit_UIBase::Create(m_pDevice, m_pContext)), E_FAIL);

	return S_OK;
}

void CEdit_Interface_UI::Update_BackColor(_float fTimeDelta)
{
	if (ImGui::CollapsingHeader("BackGround"))
	{
		ImGui::Checkbox("Render", &m_RenderBackGround);
		
		if (m_RenderBackGround)
			ImGui::ColorEdit3("Back_Color", (float*)&m_pBackGround->Get_BackColor());
	}

	if (m_RenderBackGround)
		m_pBackGround->Late_Update(fTimeDelta);
}

void CEdit_Interface_UI::SaveLoad_UI()
{
	if (m_iSeletRootUI < 0)
		return;

	if (ImGui::CollapsingHeader("SaveLoad_UI"))
	{
		ImGui::InputText("UIFilePath", m_szFilePath, MAX_PATH);
		if (ImGui::Button("Save_UI"))
		{
			string filePath = m_szFilePath;
			filePath += ".json";
			nlohmann::json SaveData;
			m_pRootUIs[m_iSeletRootUI]->Save_UI(SaveData);
			ofstream Out(filePath, ios::out | ios::trunc);
			if (!Out.is_open())
			{
				MSG_BOX(TEXT("Json 파일 저장 실패"));
				Out.close();
			}
			else
			{
				MSG_BOX(TEXT("Json 파일 저장 성공"));
				Out << SaveData.dump(4);
				Out.close();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Load_UI"))
		{
			
		}
	}
}

void CEdit_Interface_UI::Create_UI()
{
	if (ImGui::CollapsingHeader("Create_UI"))
	{

		ImGui::InputText("UIName", m_szUIName, MAX_PATH);

		ImGui::RadioButton("PANEL", &m_iUIType, ENUM_CLASS(CEdit_UIBase::UITYPE::PANEL));
		ImGui::SameLine();
		ImGui::RadioButton("TAP", &m_iUIType, ENUM_CLASS(CEdit_UIBase::UITYPE::TAP));
		ImGui::SameLine();
		ImGui::RadioButton("BUTTON", &m_iUIType, ENUM_CLASS(CEdit_UIBase::UITYPE::BUTTON));
		ImGui::SameLine();
		ImGui::RadioButton("SLOT", &m_iUIType, ENUM_CLASS(CEdit_UIBase::UITYPE::SLOT));
		ImGui::SameLine();
		ImGui::RadioButton("SCROLLBAR", &m_iUIType, ENUM_CLASS(CEdit_UIBase::UITYPE::SCROLLBAR));
		ImGui::SameLine();
		ImGui::RadioButton("PROGRESSBAR", &m_iUIType, ENUM_CLASS(CEdit_UIBase::UITYPE::PROGRESSBAR));

		ImGui::InputInt2("Size", m_iUISize, 0);

		if (ImGui::Button("Create_Parent"))
		{
			if (m_szUIName != string("").c_str())
			{
				CUIObject::UIOBJECT_DESC UIDesc{};
				UIDesc.szName = m_szUIName;
				UIDesc.iUIType = m_iUIType;
				UIDesc.vLocalSize = { (_float)m_iUISize[0],(_float)m_iUISize[1] };
				UIDesc.fDepth = 0;
				UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

				CEdit_UIBase* pRootUI = static_cast<CEdit_UIBase*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(m_eLevel), TEXT("Prototype_GameObject_UI_Base"), &UIDesc));

				if (pRootUI != nullptr)
				{
					m_szSeleteUIName = m_szUIName;
					m_pRootUIs.push_back(pRootUI);
					m_iSeletRootUI = m_pRootUIs.size() - 1;
				}
			}
			else
				MSG_BOX(TEXT("이름 입력!!"));
		}
		if (ImGui::Button("Create_Chiled"))
		{
			if (m_iSeletRootUI > -1)
			{
				if (m_szUIName != string("").c_str())
				{
					CUIObject::UIOBJECT_DESC UIDesc{};
					UIDesc.szName = m_szUIName;
					UIDesc.iUIType = m_iUIType;
					UIDesc.vLocalSize = { (_float)m_iUISize[0],(_float)m_iUISize[1] };
					UIDesc.vLocalPos = { 0.f, 0.f };
					m_pRootUIs[m_iSeletRootUI]->Create_Child(ENUM_CLASS(m_eLevel), TEXT("Prototype_GameObject_UI_Base"), &UIDesc, m_szSeleteUIName);
				}
				else
					MSG_BOX(TEXT("이름 입력!!"));
			}
			else
				MSG_BOX(TEXT("RootUI 선택 필요!!"));
		}
		if (ImGui::Button("Clear_Panel"))
		{
			if (m_pRootUIs[m_iSeletRootUI] != nullptr)
				Safe_Release(m_pRootUIs[m_iSeletRootUI]);

			m_pRootUIs.erase(m_pRootUIs.begin() + m_iSeletRootUI);
			m_iSeletRootUI = m_pRootUIs.size() - 1;
		}
	}
}

void CEdit_Interface_UI::Selete_UI(_float fTimeDelta)
{
	if (m_iSeletRootUI >= 0)
	{
		ImGui::Begin("Root_List");
		for (_int i = 0; i < m_pRootUIs.size(); ++i)
			m_pRootUIs[i]->Root_SeleteButton(m_szSeleteUIName, i, m_iSeletRootUI, m_iMovePos[0], m_iMovePos[1], m_iScalingSize[0], m_iScalingSize[1]);
		ImGui::End();

		ImGui::Begin("UI_List");
		m_pRootUIs[m_iSeletRootUI]->SeleteButton(m_szSeleteUIName, -1, m_iMovePos[0], m_iMovePos[1], m_iScalingSize[0], m_iScalingSize[1]);
		ImGui::End();

		ImGui::Begin("UI_Option");
		SetName_UI();
		SizePos_UI(fTimeDelta);

		SetTexture_UI();
		_bool AnimCehck = m_pRootUIs[m_iSeletRootUI]->Anim_Empty(m_szSeleteUIName);
		Anime_Option(fTimeDelta, AnimCehck);
		ImGui::End();
		if (!AnimCehck)
		{
			ImGui::Begin("Animation_List");
			Animation_UI(fTimeDelta);
			ImGui::End();
		}
	}
}

void CEdit_Interface_UI::Animation_UI(_float fTimeDelta)
{
	m_pRootUIs[m_iSeletRootUI]->Set_Anim(m_szSeleteUIName);
	if (ImGui::Button("AddTrack"))
	{
		m_pRootUIs[m_iSeletRootUI]->Add_Anim(m_szSeleteUIName);
	}
}

void CEdit_Interface_UI::SetName_UI()
{
	ImGui::Text("UI_Name : ");
	ImGui::SameLine();
	ImGui::Text(m_szSeleteUIName.c_str());

	ImGui::InputText("##UINameLabel", m_szUIReName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("ReName"))
	{
		m_pRootUIs[m_iSeletRootUI]->ReName(m_szSeleteUIName, m_szUIReName);
		m_szSeleteUIName = m_szUIReName;
	}
	m_pRootUIs[m_iSeletRootUI]->Update_ClassName(m_szSeleteUIName);
	ImGui::InputText("##ClassName", m_szClassName, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Class(Down:P)") || m_pGameInstance->Key_Down(DIK_P))
	{
		m_pRootUIs[m_iSeletRootUI]->Set_ClassName(m_szSeleteUIName, m_szClassName);
	}
}

void CEdit_Interface_UI::SizePos_UI(_float fTimeDelta)
{
	_bool isMoveUI = false;
	if (m_pGameInstance->Key_Pressing(DIK_Q, fTimeDelta, nullptr))
		isMoveUI = true;
	ImGui::Checkbox("Move(Hold:Q)", &isMoveUI);
	if (isMoveUI)
	{
		if (m_pGameInstance->Mouse_Pressing(MOUSEKEYSTATE::LB))
		{
			POINT	ptMouse{};
			GetCursorPos(&ptMouse);
			ScreenToClient(g_hWnd, &ptMouse);
			m_pRootUIs[m_iSeletRootUI]->Move_UI(m_szSeleteUIName, (_float)ptMouse.x, (_float)ptMouse.y, nullptr, true);
		}
	}
	if (ImGui::CollapsingHeader("Size&Move"))
	{
		ImGui::InputInt2("##UIMoveButton", m_iMovePos);
		ImGui::SameLine();
		if (ImGui::Button("UIMove"))
		{
			m_pRootUIs[m_iSeletRootUI]->Move_UI(m_szSeleteUIName, (_float)m_iMovePos[0], (_float)m_iMovePos[1], nullptr, true);
		}

		ImGui::InputInt2("##UISizeButton", m_iScalingSize);
		ImGui::SameLine();
		if (ImGui::Button("UISize"))
		{
			m_pRootUIs[m_iSeletRootUI]->Scaling_UI(m_szSeleteUIName, (_float)m_iScalingSize[0], (_float)m_iScalingSize[1]);
		}
	}
}

void CEdit_Interface_UI::SetTexture_UI()
{
	if (ImGui::CollapsingHeader("Texture"))
	{
		ImGui::RadioButton("TEX", &m_iTexType, 0);
		ImGui::SameLine();
		ImGui::RadioButton("ATLAS", &m_iTexType, 1);

		ImGui::InputText("Prototype_Path", m_szPrototypePath, MAX_PATH);
		if(m_iTexType == 1)
			ImGui::InputText("Frame_Path", m_szFrameName, MAX_PATH);

		if (ImGui::Button("SetTex"))
		{
			_tchar szText[MAX_PATH];
			MultiByteToWideChar(CP_ACP, 0, m_szPrototypePath, -1, szText, MAX_PATH);

			_wstring szPrototypePath(szText);

			m_pRootUIs[m_iSeletRootUI]->Set_AtlasTextTure(m_szSeleteUIName, ENUM_CLASS(m_eLevel), szPrototypePath.c_str(), m_szFrameName, m_iTexType);
		}
		if (m_iTexType == 1)
		{
			ImGui::InputFloat("TexSize", &m_fTexSize, 0.1f, 0.1f);
			if (ImGui::Button("SetTexSize"))
			{
				_tchar szText[MAX_PATH];
				MultiByteToWideChar(CP_ACP, 0, m_szPrototypePath, -1, szText, MAX_PATH);

				_wstring szPrototypePath(szText);

				m_pRootUIs[m_iSeletRootUI]->Set_AtlasTexSize(m_szSeleteUIName, m_szFrameName, m_fTexSize);
			}
		}
		m_pRootUIs[m_iSeletRootUI]->Update_Option(m_szSeleteUIName, m_szFrameName, m_iTexType);
	}
}

void CEdit_Interface_UI::Anime_Option(_float fTimeDelta, _bool bAnimCehck)
{
	if (bAnimCehck)
	{
		if (ImGui::Button("CreateAnim"))
		{
			m_pRootUIs[m_iSeletRootUI]->Create_Anim(m_szSeleteUIName);
		}
	}
	else
	{
		if (ImGui::CollapsingHeader("Animation"))
		{

			if (ImGui::Button("Apply"))
			{
				m_fAccTime = 0.f;
				m_pRootUIs[m_iSeletRootUI]->ReSet_Track(m_szSeleteUIName);
				m_isAnime = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				m_isAnime = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Play"))
			{
				m_isAnime = true;
			}
			_float fLastTime = {};
			if (m_pRootUIs[m_iSeletRootUI]->Get_LastTime(m_szSeleteUIName, fLastTime))
			{
				if (m_isAnime && m_fAccTime < fLastTime)
					m_fAccTime += fTimeDelta;
				else if (m_isAnime && m_fAccTime >= fLastTime)
					m_fAccTime = fLastTime;

				if (ImGui::SliderFloat("Time", &m_fAccTime, 0.0f, fLastTime, "%.2f"))
				{
					m_pRootUIs[m_iSeletRootUI]->Play_Animation(m_szSeleteUIName, m_fAccTime);
				}
				if (m_isAnime)
					m_pRootUIs[m_iSeletRootUI]->Play_Animation(m_szSeleteUIName, m_fAccTime);
			}
			if (ImGui::Button("SetTrackPos"))
			{
				m_pRootUIs[m_iSeletRootUI]->Set_AnimPos(m_szSeleteUIName);
			}
		}
	}
}

CEdit_Interface_UI* CEdit_Interface_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CEdit_Interface_UI* pInstance = new CEdit_Interface_UI(pDevice, pContext);
	if (FAILED(pInstance->Initialize(eLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CEdit_Interface_UI"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEdit_Interface_UI::Free()
{
	__super::Free();

	for (auto RootUI : m_pRootUIs)
		Safe_Release(RootUI);
	
	m_pRootUIs.clear();

	Safe_Release(m_pBackGround);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

}
