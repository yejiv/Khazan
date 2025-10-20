#include "UI_ScrollBar.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_ScrollBar::CUI_ScrollBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_ScrollBar::CUI_ScrollBar(const CUI_ScrollBar& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT CUI_ScrollBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ScrollBar::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_ScrollBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_ScrollBar::Update(_float fTimeDelta)
{
}

void CUI_ScrollBar::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_ScrollBar::Render()
{
	return S_OK;
}

HRESULT CUI_ScrollBar::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
	m_szName = pInData.value("name", "");
	string strTexType = pInData.value("TexType", "");

	if ("Atlas" == strTexType)
	{
		string strTexTag = pInData.value("TexTag", "");

		m_iTexPass = CClientInstance::GetInstance()->UI_TexTag_Maping(strTexTag);
		if (m_iTexPass == -1)
			return E_FAIL;
	}

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
		m_Events.resize(3);
		for (_int i = 0; i < 2; ++i)
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
	__super::Update_Transform(nullptr, m_vLocalPos);

	return S_OK;
}

void CUI_ScrollBar::Free()
{
	__super::Free();
	m_Events.clear();
}
