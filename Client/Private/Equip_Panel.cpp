#include "Equip_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
CEquip_Panel::CEquip_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CEquip_Panel::CEquip_Panel(const CEquip_Panel& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CEquip_Panel::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CEquip_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEquip_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CEquip_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CEquip_Panel::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CEquip_Panel::Render()
{
	return S_OK;
}

HRESULT CEquip_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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

	m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
	__super::Update_Rotation(0.f);

	if (pInData.contains("Children"))
	{
		for (auto& child : pInData["Children"])
		{
			string strName = child.value("name", "");

			if ("EQUIP_Label" == strName)
			{
				_float2 vPos = { child["LocalPos"].value("x", 0.f), child["LocalPos"].value("y", 0.f) };
				for (_int i = 0; i < 5; ++i)
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
					if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
						return E_FAIL;

					pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
					m_Children.push_back(pChild);

					static_cast<CUI_TextBox*>(pChild)->Update_PosY(i, vPos, 155.f);
					static_cast<CUI_TextBox*>(pChild)->Set_Text(AnsiToWString(MapPing_Label(i)));

				}
			}
		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);
	return S_OK;
}

string CEquip_Panel::MapPing_Label(_int iIndex)
{
	switch (iIndex)
	{
	case 0:	return "무기";
	case 1:	return "방어구";
	case 2:	return "장신구";
	case 3:	return "숏컷 아이템";
	case 4:	return "명계의 기운";
	}

	return "";
}

CEquip_Panel* CEquip_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CEquip_Panel* pInstance = new CEquip_Panel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CEquip_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEquip_Panel::Clone(void* pArg)
{
	CEquip_Panel* pInstance = new CEquip_Panel(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CEquip_Panel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEquip_Panel::Free()
{
	__super::Free();
}
