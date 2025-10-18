#include "UI_Frame.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_Frame::CUI_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CUI_Frame::CUI_Frame(const CUI_Frame& Prototype)
	:CUIObject(Prototype)
{
}

HRESULT CUI_Frame::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_Frame::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Frame::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_Frame::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Frame::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CUI_Frame::Render()
{
	return S_OK;
}

HRESULT CUI_Frame::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID)
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

	m_iShaderPass = pInData.value("shaderPass", -1);
	if (m_iShaderPass == -1)
		return E_FAIL;

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
	if (pInData.contains("UV"))
	{
		m_vUVMinMax.clear();
		for (auto& uv : pInData["UV"])
		{
			_float4 uvData;
			uvData.x = uv.value("MinX", 0.f);
			uvData.y = uv.value("MinY", 0.f);
			uvData.z = uv.value("MaxX", 0.f);
			uvData.w = uv.value("MaxY", 0.f);
			m_vUVMinMax.push_back(uvData);
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
	m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });

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

			CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

			if (pChild == nullptr)
			{
				MSG_BOX(TEXT("자식 클론 생성 실패"));
				return E_FAIL;
			}
			if (pChild->Load_UI(child, iPrototypeLevelID))
				return E_FAIL;

			pChild->Insert_Bubble([this]() {this->Bubble_EventCall(); });
			m_Children.push_back(pChild);
		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);
	return S_OK;
}

void CUI_Frame::Free()
{
	__super::Free();
}
