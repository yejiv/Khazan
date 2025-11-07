#include "UI_Text.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Text::CUI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Text::CUI_Text(const CUI_Text& Prototype)
	:CUIObject(Prototype)
{
}

void CUI_Text::Set_Text(_wstring wstrText)
{
	m_wstrText = wstrText;
}

void CUI_Text::Setting_Text(const TEXTBOX_DESC& pDesc)
{
	m_bIsTextBox = pDesc.bIsTextBox;
	m_eTextAlign = pDesc.eTextAlign;
	m_fMaxWidth = pDesc.fMaxWidth;
	m_fOffsetHeight = pDesc.fOffsetHeight;
	m_iPivotX = pDesc.iPivotX;
	m_iPivotY = pDesc.iPivotY;
	m_wstrTexttag = pDesc.wstrTexttag;
	m_wstrText = pDesc.wstrText;
	m_vColor = pDesc.vColor;
}

void CUI_Text::Setting_Pivot(_int iPivotX, _int iPivotY)
{
	m_iPivotX = iPivotX;
	m_iPivotY = iPivotY;
}

HRESULT CUI_Text::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Text::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Text::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_Text::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Text::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Text::Render()
{
	return S_OK;
}

HRESULT CUI_Text::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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

	m_bIsTextBox = pInData.value("TextBox", false);
	_int iAlign = pInData.value("Align", ENUM_CLASS(TEXT_ALIGN::END));
	m_eTextAlign = static_cast<TEXT_ALIGN>(iAlign);

	m_fMaxWidth = pInData.value("Maxwidth", 0);
	m_fOffsetHeight = pInData.value("offsetHeight", 0);
	string FontTag = pInData.value("FontTag", "");
	m_wstrTexttag = AnsiToWString(FontTag);
	m_wstrText = AnsiToWString(pInData.value("Text", ""), CP_UTF8);;
	if (pInData.contains("iPivot"))
	{
		m_iPivotX = pInData["iPivot"].value("x", 0);
		m_iPivotY = pInData["iPivot"].value("y", 0);
	}
	m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	__super::Update_Rotation(0.f);
	__super::Update_Transform(nullptr, m_vLocalPos);

	return S_OK;
}

void CUI_Text::Free()
{
	__super::Free();
}
