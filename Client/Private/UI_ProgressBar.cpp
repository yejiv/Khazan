#include "UI_ProgressBar.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CUI_ProgressBar::CUI_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{pDevice,pContext}
	, m_pClientInstance{ CClientInstance::GetInstance() }
{
	Safe_AddRef(m_pClientInstance);
}

CUI_ProgressBar::CUI_ProgressBar(const CUI_ProgressBar& Prototype)
	:CUIObject ( Prototype )
	, m_pClientInstance{ Prototype.m_pClientInstance }
{
	Safe_AddRef(m_pClientInstance);
}



HRESULT CUI_ProgressBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_ProgressBar::Initialize_Clone(void* pArg)
{
	PROGRESSBAR_DESC* pDesc = static_cast<PROGRESSBAR_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	//m_vOriginPos = _float3(pDesc->vLocalPos.x, pDesc->vLocalPos.y, 0.1f);
	//m_vOriginSize = pDesc->vLocalSize;
	//m_eDirection = pDesc->eDirection;
	//m_eMode = pDesc->eMode;
	//m_fLerpSpeed = 1.f;
	//m_fDisplayRate = 1.f;

	// Test
	m_fMaxValue = 100.f;

	return S_OK;
}

void CUI_ProgressBar::Priority_Update(_float fTimeDelta)
{
}

void CUI_ProgressBar::Update(_float fTimeDelta)
{
	if (!m_isChange)
		return;

	_float fTargetRate = Make_Rate(m_fCurrentValue, m_fMaxValue);

	m_fDisplayRate = Lerp(m_fDisplayRate, fTargetRate, fTimeDelta * m_fLerpSpeed);

	switch (m_eMode)
	{
	case BAR_MODE::REDUCE:
		switch (m_eDirection)
		{
		case BAR_DIRECTION::LEFT_TO_RIGHT:
			Reduce_LeftToRight(m_fDisplayRate);
			break;
		case BAR_DIRECTION::RIGHT_TO_LEFT:
			Reduce_RightToLeft(m_fDisplayRate);
			break;
		case BAR_DIRECTION::TOP_TO_BOTTOM:
			// 필요시 세로 버전 함수 추가
			break;
		case BAR_DIRECTION::BOTTOM_TO_TOP:
			// 필요시 세로 버전 함수 추가
			break;
		}
		break;

	case BAR_MODE::EXPAND:
		switch (m_eDirection)
		{
		case BAR_DIRECTION::LEFT_TO_RIGHT:
			Expand_LeftToRight(m_fDisplayRate);
			break;
		case BAR_DIRECTION::RIGHT_TO_LEFT:
			Expand_RightToLeft(m_fDisplayRate);
			break;
		case BAR_DIRECTION::TOP_TO_BOTTOM:
			// 필요시 세로 버전 함수 추가
			break;
		case BAR_DIRECTION::BOTTOM_TO_TOP:
			// 필요시 세로 버전 함수 추가
			break;
		}
		break;
	}

	// Lerp가 끝나면 Update를 막는다.
	if (fabs(m_fDisplayRate - fTargetRate) < 0.001f)
		m_isChange = false;

}

void CUI_ProgressBar::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_ProgressBar::Render()
{
	return S_OK;
}

HRESULT CUI_ProgressBar::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID)
{
	m_szName = pInData.value("name", "");
	string strTexType = pInData.value("TexType", "");

	if ("Atlas" == strTexType)
	{
		string strTexTag = pInData.value("TexTag", "");

		m_iTexPass = m_pClientInstance->UI_TexTag_Maping(strTexTag);
		if (m_iTexPass == -1)
			return E_FAIL;
	}

	string szType = pInData.value("type", "");
	m_iUIType = m_pClientInstance->UIType_StringToEnum(szType);

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

_float CUI_ProgressBar::Make_Rate(_float fSrc, _float fDst)
{

	if (fDst <= 0.f)
		return 0.f;

	_float fRate = fSrc / fDst;

	if (fRate <= 0.001f)
		fRate = 0.001f;
	if (fRate > 1.f)
		fRate = 1.f;

	return fRate;
}

void CUI_ProgressBar::Reduce_RightToLeft(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x <= 0.001f)
		m_vLocalSize.x = 0.001f;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x - (m_vOriginSize.x - m_vLocalSize.x) * 0.5f;
}

void CUI_ProgressBar::Reduce_LeftToRight(_float fRate)
{

	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x <= 0.001f)
		m_vLocalSize.x = 0.001f;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x + (m_vOriginSize.x - m_vLocalSize.x) * 0.5f;



}

void CUI_ProgressBar::Expand_RightToLeft(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x > m_vOriginSize.x)
		m_vLocalSize.x = m_vOriginSize.x;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x + (m_vOriginSize.x - m_vLocalSize.x) * 0.5f;
}

void CUI_ProgressBar::Expand_LeftToRight(_float fRate)
{
	m_vLocalSize.x = m_vOriginSize.x * fRate;

	if (m_vLocalSize.x > m_vOriginSize.x)
		m_vLocalSize.x = m_vOriginSize.x;

	m_pTransformCom->Scaling(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

	m_vLocalPos.x = m_vOriginPos.x - (m_vOriginSize.x - m_vLocalSize.x) * 0.5f;
}

void CUI_ProgressBar::Free()
{
	__super::Free();
	Safe_Release(m_pClientInstance);
}
