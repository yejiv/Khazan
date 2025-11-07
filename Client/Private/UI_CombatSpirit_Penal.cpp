#include "UI_CombatSpirit_Penal.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_ComBatSpirit_Gauge.h"
#include "UI_ComBatSpirit_Slot.h"

CUI_CombatSpirit_Penal::CUI_CombatSpirit_Penal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Panel{ pDevice, pContext }
{
}

CUI_CombatSpirit_Penal::CUI_CombatSpirit_Penal(const CUI_CombatSpirit_Penal& Prototype)
	: CUI_Panel(Prototype)
{
}

HRESULT CUI_CombatSpirit_Penal::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;

	if(FAILED(Ready_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CombatSpirit_Penal::Initialize_Clone(void* pArg)
{
	m_iMaxSlotNum = 10;
	m_fOffsetX = 26.f;
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_CombatSpirit_Penal::Priority_Update(_float fTimeDelta)
{
	for (_int i = 0; i < m_iCulSlotNum; ++i)
		m_pSlot[i]->Priority_Update(fTimeDelta);
}

void CUI_CombatSpirit_Penal::Update(_float fTimeDelta)
{
	//if (m_pGameInstance->Key_Down(DIK_U))
	//{
	//	m_fCulGaugeValue = 0;
	//}
	//if (m_pGameInstance->Key_Down(DIK_I, INPUT_TYPE::UI))
	//{
	//	m_iCulSlotNum++;
	//	if (m_iCulSlotNum > m_iMaxSlotNum)
	//		m_iCulSlotNum = 0;
	//}

	if (m_fCulGaugeValue > m_iCulSlotNum)
		m_fCulGaugeValue = m_iCulSlotNum;
	else if(m_fCulGaugeValue < m_iCulSlotNum)
		m_fCulGaugeValue += fTimeDelta;

	if (m_iCulSlotNum > m_iMaxSlotNum)
		m_iCulSlotNum = m_iMaxSlotNum;

	for (_int i = 0; i < m_iCulSlotNum; ++i)
		m_pSlot[i]->Update(fTimeDelta);

}

void CUI_CombatSpirit_Penal::Late_Update(_float fTimeDelta)
{
	for (_int i = 0; i < m_iCulSlotNum; ++i)
	{
		//1 <=1.5 2
		if(i + 1 <= m_fCulGaugeValue)
			m_pSlot[i]->Update_Gauge(1.f);
		else if(i < m_fCulGaugeValue && i + 1 > m_fCulGaugeValue)
			m_pSlot[i]->Update_Gauge(m_fCulGaugeValue - floorf(m_fCulGaugeValue));
		else
			m_pSlot[i]->Update_Gauge(0.f);

		m_pSlot[i]->Update_Pos(m_vWorldPos, m_fOffsetX, m_iCulSlotNum);
		m_pSlot[i]->Late_Update(fTimeDelta);
	}
}

HRESULT CUI_CombatSpirit_Penal::Render()
{
	return S_OK;
}

HRESULT CUI_CombatSpirit_Penal::Ready_Prototype()
{
	if(FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_CombatSpirit_Slot"),
		CUI_ComBatSpirit_Slot::Create(m_pDevice, m_pContext, m_iLevel))))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_CombatSpirit_Gauge"),
		CUI_CombatSpirit_Gauge::Create(m_pDevice, m_pContext, m_iLevel))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_Hud_ComBatSpirit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Hud/State/T_Hud_Gauge_CombatSpirit_0%d.png"), 4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_CombatSpirit_Penal::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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
			string strClass = child.value("class", "");
			_wstring wstrClass = AnsiToWString(strClass);

			CUI_Slot::UISLOTDESC UIDesc{};
			UIDesc.szName = "";
			UIDesc.iUIType = 0;
			UIDesc.vLocalSize = { 1.f, 1.f };
			UIDesc.fDepth = 0;
			UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };
			
			for (_int i = 0; i < m_iMaxSlotNum; ++i)
			{
				UIDesc.iIndex = i;

				CUI_ComBatSpirit_Slot* pChild = static_cast<CUI_ComBatSpirit_Slot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

				if (pChild == nullptr)
				{
					MSG_BOX(TEXT("자식 클론 생성 실패"));
					return E_FAIL;
				}
				if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
					return E_FAIL;

				pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
				m_pSlot.push_back(pChild);
				
				m_Children.push_back(pChild);
				Safe_AddRef(pChild);
			}

		}
	}

	__super::Update_Transform(nullptr, m_vLocalPos);

	return S_OK;
}

CUI_CombatSpirit_Penal* CUI_CombatSpirit_Penal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CUI_CombatSpirit_Penal* pInstance = new CUI_CombatSpirit_Penal(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX(TEXT("Failed Created : CUI_CombatSpirit_Penal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_CombatSpirit_Penal::Clone(void* pArg)
{
	CUI_CombatSpirit_Penal* pInstance = new CUI_CombatSpirit_Penal(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_CombatSpirit_Penal"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_CombatSpirit_Penal::Free()
{
	__super::Free();
	for (auto& pChild : m_pSlot)
	{
		if (pChild != nullptr)
		{
			Safe_Release(pChild);
		}
	}
	m_pSlot.clear();
}
