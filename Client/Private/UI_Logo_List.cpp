#include "UI_Logo_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CUI_Logo_List::CUI_Logo_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Tap{ pDevice, pContext }
{
}

CUI_Logo_List::CUI_Logo_List(const CUI_Logo_List& Prototype)
	: CUI_Tap(Prototype)
{
}

void CUI_Logo_List::Update_Pos(CUI_Logo::LISTTYPE eType, _float2 vPos, _float fOffSetY)
{
	m_eType = eType;
	m_vLocalPos.x = vPos.x;
	m_vLocalPos.y = vPos.y + ENUM_CLASS(eType) * fOffSetY;

	switch (m_eType)
	{
	case CUI_Logo::LISTTYPE::NEWGAME:
		m_pText->Set_Text(TEXT("새 게임"));
		break;
	case CUI_Logo::LISTTYPE::OPTION:
		m_pText->Set_Text(TEXT("설정"));
		break;
	case CUI_Logo::LISTTYPE::EXIT:
		m_pText->Set_Text(TEXT("종료"));
		break;
	}
	__super::Update_Transform(nullptr, m_vWorldPos);
}

void CUI_Logo_List::Set_Selete(_bool isSelete)
{
	if (m_bIsSelete == isSelete)
		return;

	m_bIsSelete = isSelete;

	if (m_bIsSelete)
		m_fAccTime = 0.7f;
}

HRESULT CUI_Logo_List::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CUI_Logo_List::Initialize_Clone(void* pArg)
{
	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
	CHECK_FAILED(Ready_Componet(), E_FAIL);

	return S_OK;
}

void CUI_Logo_List::Priority_Update(_float fTimeDelta)
{
}

void CUI_Logo_List::Update(_float fTimeDelta)
{
	if (!m_isVisible)
		return;

	if (m_fAccTime < 1.f)
	{
		m_fAccTime += fTimeDelta * 0.7f;
	}

	if (ButtonOver(g_hWnd))
	{
		CUI_Logo::LOGOBUBBLE_DESC Desc{};
		Desc.eListType = m_eType;
		Desc.isClick = false;
		Bubble_EventCall(&Desc);
	}
	if (ButtonClick(g_hWnd, false, true) || m_bIsSelete && m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::UI))
	{
		CUI_Logo::LOGOBUBBLE_DESC Desc{};
		Desc.eListType = m_eType;
		Desc.isClick = true;
		Bubble_EventCall(&Desc);
	}
}

void CUI_Logo_List::Late_Update(_float fTimeDelta)
{
	if (!m_isVisible)
		return;

	if (m_bIsSelete)
	{
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
		m_pDecoL->Update_Alpha(m_fAccTime);
		m_pDecoR->Update_Alpha(m_fAccTime);
		m_pDecoL->Late_Update(fTimeDelta);
		m_pDecoR->Late_Update(fTimeDelta);
		m_pText->Set_Color({ 0.f, 0.f, 0.f, 0.8f });
	}
	else
	{
		m_pText->Set_Color({ 1.f, 1.f, 1.f, 1.f });
	}
	m_pText->Late_Update(fTimeDelta);
}

HRESULT CUI_Logo_List::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
		return E_FAIL;

	if (FAILED(m_pMaskTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_MaskTexture", 3)))
		return E_FAIL;

	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fDissovle", &m_fAccTime, sizeof(_float)), E_FAIL);
	m_vColor.w = 0.9f;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

	m_pShaderCom->Begin(13);
	m_pVIBufferCom->Bind_Resources();
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CUI_Logo_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	m_szName = pInData.value("name", "");
	string strTexType = pInData.value("TexType", "");

	string szType = pInData.value("type", "");
	m_iUIType = CClientInstance::GetInstance()->UIType_StringToEnum(szType);

	m_iTexPass = pInData.value("TexIndex", 0);
	m_iShaderPass = pInData.value("shaderPass", 0);


	m_fDepth = pInData.value("depth", 0.f);

	if (pInData.contains("LocalPos"))
	{
		m_vLocalPos.x = pInData["LocalPos"].value("x", 0.f);
		m_vLocalPos.y = pInData["LocalPos"].value("y", 0.f);
	}

	if (pInData.contains("LocalSize"))
	{
		m_vLocalSize.x = pInData["LocalSize"].value("x", 0.f);
		m_vLocalSize.y = pInData["LocalSize"].value("y", 0.f);
	}

	if (pInData.contains("Angle"))
	{
		m_vAngle.x = pInData["Angle"].value("x", 0.f);
		m_vAngle.y = pInData["Angle"].value("y", 0.f);
		m_vAngle.z = pInData["Angle"].value("z", 0.f);
	}

	if (pInData.contains("Color"))
	{
		m_vColor.x = pInData["Color"].value("x", 0.f);
		m_vColor.y = pInData["Color"].value("y", 0.f);
		m_vColor.z = pInData["Color"].value("z", 0.f);
		m_vColor.w = pInData["Color"].value("w", 0.f);
	}

	if (pInData.contains("UV"))
	{
		m_vUV.clear();
		for (auto& uv : pInData["UV"])
		{
			_float4 uvData;
			uvData.x = uv.value("MinX", 0.f);
			uvData.y = uv.value("MinY", 0.f);
			uvData.z = uv.value("MaxX", 0.f);
			uvData.w = uv.value("MaxY", 0.f);
			m_vUV.push_back(uvData);
		}
	}
	if (pInData.contains("Anime"))
	{
		m_Track.clear();
		for (auto& t : pInData["Anime"])
		{
			UIKEYFRAME track;
			track.fTrackPosition = t.value("TrackPosition", 0.f);
			track.fAlpha = t.value("Alpha", 1.f);
			track.fAngle = t.value("Angle", 0.f);
			track.fSize = t.value("Size", 1.f);
			track.szEvent = t.value("Event", "");

			if (t.contains("Transloation"))
			{
				track.vTransloation.x = t["Transloation"].value("x", 0.f);
				track.vTransloation.y = t["Transloation"].value("y", 0.f);
			}

			m_Track.push_back(track);
		}
	}

	if (pInData.contains("Events"))
	{
		m_Events.clear();
		m_Events.resize(4);
		for (_int i = 0; i < 4; ++i)
		{
			UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
			string strEvent = pInData["Events"][i].get<string>();

			_wstring wstrLayer = AnsiToWString(pDesc->szName);
			_wstring wstrEvent = AnsiToWString(strEvent);
			m_Events[i] = CClientInstance::GetInstance()->Pop_UIEvent(wstrLayer, wstrEvent);
		}
	}

	m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	__super::Update_Rotation(0.f);

	if (pInData.contains("Children"))
	{
		for (auto& child : pInData["Children"])
		{
			string strClass = child.value("class", "");
			_wstring wstrClass = AnsiToWString(strClass);

			CUIObject::UIOBJECT_DESC UIDesc{};
			UIDesc.szName = "";
			UIDesc.iUIType = 0;
			UIDesc.vLocalSize = { 1.f, 1.f };
			UIDesc.fDepth = 0;
			UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

			CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), wstrClass.c_str(), &UIDesc));

			if (pChild == nullptr)
			{
				MSG_BOX(TEXT("자식 클론 생성 실패"));
				return E_FAIL;
			}
			if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
				return E_FAIL;

			pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
			m_Children.push_back(pChild);
		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);
	m_iState = ENUM_CLASS(UISTATE::ENABLE);

	for (auto pChild : m_Children)
	{
		string strName = pChild->Get_Name();
		if (strName == "Logo_List_Text")
		{
			m_pText = static_cast<CUI_TextBox*>(pChild);
			Safe_AddRef(m_pText);
		}
		else if (strName == "Logo_List_Deco_L")
		{
			m_pDecoL = static_cast<CUI_Default_Tex*>(pChild);
			Safe_AddRef(m_pDecoL);
		}
		else if (strName == "Logo_List_Deco_R")
		{
			m_pDecoR = static_cast<CUI_Default_Tex*>(pChild);
			Safe_AddRef(m_pDecoR);
		}
	}
	m_pDecoL->Set_ShaderPass(0);
	m_pDecoR->Set_ShaderPass(0);
	return S_OK;
}

HRESULT CUI_Logo_List::Ready_Componet()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Log_Tex"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_FX_Mask"),
		TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom), nullptr)))
		return E_FAIL;
	return S_OK;
}

CUI_Logo_List* CUI_Logo_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_Logo_List* pInstance = new CUI_Logo_List(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Logo_List"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Logo_List::Clone(void* pArg)
{
	CUI_Logo_List* pInstance = new CUI_Logo_List(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Logo_List"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Logo_List::Free()
{
	__super::Free();

	Safe_Release(m_pText);
	Safe_Release(m_pDecoL);
	Safe_Release(m_pDecoR);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
