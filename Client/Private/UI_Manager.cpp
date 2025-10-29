#include "UI_Manager.h"
#include "GameInstance.h"
#include "Atlas_RenderGroup.h"
#include "UI_Layer.h"
#include "UIObject.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pDeviceContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;
	CAtlas_RenderGroup::ATLASGROUP_DESC Desc = {};
	for (_int i = 0; i < 10; ++i)
	{
		Desc.fDepth = (_float)i;
		Desc.iShdaerPass = 0;
		Desc.szName = "";
		Desc.iUIType = ENUM_CLASS(UITYPE::RENDER_GROUP);
	
		CAtlas_RenderGroup* pRenderGroup = static_cast<CAtlas_RenderGroup*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_AtlasRenderGroup"), &Desc));

		if (pRenderGroup == nullptr)
			return E_FAIL;

		m_pAtlasRenderGroup.push_back(pRenderGroup);
	}

	Desc.fDepth = 0;
	Desc.iShdaerPass = 1;
	Desc.szName = "";
	Desc.iUIType = ENUM_CLASS(UITYPE::RENDER_GROUP);

	m_pWorldRenderGroup = static_cast<CAtlas_RenderGroup*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_AtlasRenderGroup"), &Desc));

	if (m_pWorldRenderGroup == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Manager::UI_UpdateSwitch(const _wstring& szRootUIName, void* pArg)
{
	CUIObject* pRootUI = Find_RootUI(szRootUIName);

	if (pRootUI == nullptr)
	{
		_wstring Failed = TEXT("Failed to Update_Switch : ") + szRootUIName;
		MSG_BOX(Failed.c_str());
		return E_FAIL;
	}

	return pRootUI->Update_Switch(pArg);
}

CUIObject* CUI_Manager::Get_RootUI(const _wstring& szRootUIName)
{
	CUIObject* pRootUI = Find_RootUI(szRootUIName);
	if (!pRootUI)
		return nullptr;

	return pRootUI;
}

HRESULT CUI_Manager::Add_Event(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void()> Event)
{
	if (Event == nullptr)
		return E_FAIL;

	CUI_Layer* pLayer = Find_Layer(strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CUI_Layer::Create();
		pLayer->Add_Event(strEventTag, Event);
		m_pUILayers.emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_Event(strEventTag, Event);

	return S_OK;
}

HRESULT CUI_Manager::Add_ParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void(void*)> Event)
{
	if (Event == nullptr)
		return E_FAIL;

	CUI_Layer* pLayer = Find_Layer(strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CUI_Layer::Create();
		pLayer->Add_ParamEvent(strEventTag, Event);
		m_pUILayers.emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_ParamEvent(strEventTag, Event);

	return S_OK;
}

function<void()> CUI_Manager::Pop_Event(const _wstring& strLayerTag, const _wstring& strEventTag)
{
	CUI_Layer* pLayer = Find_Layer(strLayerTag);

	if (nullptr == pLayer)
		return nullptr;
	else
		return pLayer->Pop_Event(strEventTag);
}

function<void(void*)> CUI_Manager::Pop_ParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag)
{
	CUI_Layer* pLayer = Find_Layer(strLayerTag);

	if (nullptr == pLayer)
		return nullptr;
	else
		return pLayer->Pop_ParamEvent(strEventTag);
}

HRESULT CUI_Manager::Erase_EventLayer(const _wstring& strLayerTag)
{
	CUI_Layer* pLayer = Find_Layer(strLayerTag);

	if (nullptr == pLayer)
		return E_FAIL;

	Safe_Release(pLayer);

	return S_OK;
}

HRESULT CUI_Manager::Load_UIData(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iPrototypeLevelID, const _tchar* pFilePath)
{
	ifstream In(pFilePath);
	if (!In.is_open())
	{
		MSG_BOX(TEXT("UI JSON 파일 불러오기 실패"));
		In.close();
		return E_FAIL;
	}
	else
	{
		nlohmann::json jsonData;
		In >> jsonData;

		string strClass = jsonData.value("class", "");
		_wstring wstrClass = AnsiToWString(strClass);

		CUIObject::UIOBJECT_DESC UIDesc{};
		UIDesc.szName = jsonData.value("name", "");
		UIDesc.iUIType = 0;
		UIDesc.vLocalSize = { 1.f, 1.f };
		UIDesc.fDepth = 0;
		UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

		CUIObject* pRootUI = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

		if (pRootUI == nullptr)
		{
			MSG_BOX(TEXT("UI Load : 클론 실패"));
			return E_FAIL;
		}
		
		if (FAILED(pRootUI->Load_UI(jsonData, iPrototypeLevelID, &UIDesc)))
		{
			MSG_BOX(TEXT("UI Load : 데이터 로드 실패"));
			return E_FAIL;
		}

		m_pGameInstance->Push_GameObject_ToLayer(iLayerLevelID, strLayerTag, pRootUI);

		string strName = jsonData.value("name", "");
		_wstring wstrName = AnsiToWString(strName);

		m_pRootUI.emplace(wstrName.c_str(), pRootUI);
		Safe_AddRef(pRootUI);
	}

	return S_OK;
}

CUIObject* CUI_Manager::Load_UIObject(_uint iPrototypeLevelID, const _tchar* pFilePath)
{
	ifstream In(pFilePath);
	if (!In.is_open())
	{
		MSG_BOX(TEXT("UI JSON 파일 불러오기 실패"));
		In.close();
		return nullptr;
	}
	else
	{
		nlohmann::json jsonData;
		In >> jsonData;

		string strClass = jsonData.value("class", "");
		_wstring wstrClass = AnsiToWString(strClass);

		CUIObject::UIOBJECT_DESC UIDesc{};
		UIDesc.szName = jsonData.value("name", "");
		UIDesc.iUIType = 0;
		UIDesc.vLocalSize = { 1.f, 1.f };
		UIDesc.fDepth = 0;
		UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

		CUIObject* pRootUI = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

		if (pRootUI == nullptr)
		{
			MSG_BOX(TEXT("UI Load : 클론 실패"));
			return nullptr;
		}

		if (FAILED(pRootUI->Load_UI(jsonData, iPrototypeLevelID, &UIDesc)))
		{
			MSG_BOX(TEXT("UI Load : 데이터 로드 실패"));
			return nullptr;
		}

		return pRootUI;
	}
}

_int CUI_Manager::UIType_StringToEnum(string szUIType)
{
	UITYPE eUIType = {};

	if (szUIType == "PANEL")
		eUIType = UITYPE::PANEL;
	if (szUIType == "TAP")
		eUIType = UITYPE::TAP;
	if (szUIType == "BUTTON")
		eUIType = UITYPE::BUTTON;
	if (szUIType == "SLOT")
		eUIType = UITYPE::SLOT;
	if (szUIType == "PROGRESSBAR")
		eUIType = UITYPE::PROGRESSBAR;
	if (szUIType == "SCROLLBAR")
		eUIType = UITYPE::SCROLLBAR;
	if (szUIType == "TEXTURE")
		eUIType = UITYPE::TEXTURE;
	if (szUIType == "TEXT")
		eUIType = UITYPE::TEXT;

	return ENUM_CLASS(eUIType);
}

HRESULT CUI_Manager::Add_UIRender(UI_RENDER_TYPE eRender, CUIObject* pUIObject)
{
	if (pUIObject == nullptr)
		return E_FAIL;
	if (UI_RENDER_TYPE::ATLAS == eRender)
	{
		VTXINSTANCE_UI UIInstanceDesc = {};
		pUIObject->Get_Data(UIInstanceDesc);
		m_pAtlasRenderGroup[(_uint)pUIObject->Get_Depth()]->Add_UIInstance(&UIInstanceDesc);
	}
	else if (UI_RENDER_TYPE::WORLD == eRender)
	{
		VTXINSTANCE_UI UIInstanceDesc = {};
		pUIObject->Get_Data(UIInstanceDesc);
		m_pWorldRenderGroup->Add_UIInstance(&UIInstanceDesc);
	}

	else
	{
		m_pRenderUI.push_back(pUIObject);
		Safe_AddRef(m_pRenderUI.back());
	}
	return S_OK;
}

void CUI_Manager::UIObjectToRenderer()
{
	for (_int i = 0; i < (_int)m_pAtlasRenderGroup.size(); ++i)
	{
		m_pRenderUI.push_back(m_pAtlasRenderGroup[i]);
		Safe_AddRef(m_pAtlasRenderGroup[i]);
	}

	sort(m_pRenderUI.begin(), m_pRenderUI.end(), [](CUIObject* pSour, CUIObject* pDest) { return pSour->Get_Depth() > pDest->Get_Depth(); });

	m_pWorldRenderGroup->Add_Renderer();
	for (_int i = 0; i < (_int)m_pRenderUI.size(); ++i)
	{
		m_pRenderUI[i]->Add_Renderer();
		Safe_Release(m_pRenderUI[i]);
	}
	m_pRenderUI.clear();
}

CUI_Layer* CUI_Manager::Find_Layer(const _wstring& strLayerTag)
{
	auto iter = m_pUILayers.find(strLayerTag);

	if (iter == m_pUILayers.end())
		return nullptr;

	return iter->second;
}

CUIObject* CUI_Manager::Find_RootUI(const _wstring& strUITag)
{
	auto iter = m_pRootUI.find(strUITag);

	if (iter == m_pRootUI.end())
		return nullptr;

	return iter->second;
}

HRESULT CUI_Manager::Ready_Prototype()
{
	/* Prototype_Component_VIBuffer_Instance_UI */
	CVIBuffer_Instance_UI::INSTANCE_DESC UIInstanceDesc = {};
	UIInstanceDesc.iNumInstance = 100;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Instance_UI"),
		CVIBuffer_Instance_UI::Create(m_pDevice, m_pContext, &UIInstanceDesc))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxPosTex_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_Atlas"),
		CTexture_Atlas::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Atlas/Atlas_%d.json"), 4))))
		return E_FAIL;

	//GameObject_AtlasRenderGroup
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_AtlasRenderGroup"),
		CAtlas_RenderGroup::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

_float4 CUI_Manager::Get_AtlasUV(const string pFrameName, _uint iTextureIndex)
{
	return m_pAtlasRenderGroup[0]->Get_AtlasUV(pFrameName, iTextureIndex);
}

CUI_Manager* CUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Manager* pInstance = new CUI_Manager(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pWorldRenderGroup);

	for (auto UIObject : m_pRenderUI)
		Safe_Release(UIObject);
	m_pRenderUI.clear();

	for (auto Root : m_pRootUI)
		Safe_Release(Root.second);
	m_pRootUI.clear();

	for (auto Layer : m_pUILayers)
		Safe_Release(Layer.second);
	m_pUILayers.clear();

	for (auto RenderGroup : m_pAtlasRenderGroup)
		Safe_Release(RenderGroup);
	m_pAtlasRenderGroup.clear();
}
