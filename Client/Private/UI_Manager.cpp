#include "UI_Manager.h"
#include "GameInstance.h"
#include "Atlas_RenderGroup.h"
#include "UI_Layer.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice{ pDevice }
	, m_pDeviceContext{ pDeviceContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	for (_int i = 0; i < 10; ++i)
	{
		CAtlas_RenderGroup::ATLASGROUP_DESC Desc = {};
		Desc.fDepth = (_float)i;
		Desc.iShdaerPass = 0;
		CAtlas_RenderGroup* pRenderGroup = static_cast<CAtlas_RenderGroup*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_AtlasRenderGroup"), &Desc));

		if (pRenderGroup == nullptr)
			return E_FAIL;

		m_pAtlasRenderGroup.push_back(pRenderGroup);
	}
	return S_OK;
}

HRESULT CUI_Manager::UI_UpdateSwitch(const _wstring& strUITag, void* pArg)
{
	return S_OK;
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

HRESULT CUI_Manager::Load_UIData(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iPrototypeLevelID, const _tchar* pTextureFilePath)
{
	return S_OK;
}

UITYPE CUI_Manager::UIType_StringToEnum(string szUIType)
{
	return UITYPE();
}

_float CUI_Manager::Convert_LocalToWinSize(_float fPos, _float fWinSize)
{
	return _float();
}

HRESULT CUI_Manager::Add_UIInstance(_matrix Worldmat, _float fDepth, _float4 vUV, string szTexTag, _uint iShaderPass)
{
	return E_NOTIMPL;
}

void CUI_Manager::Add_AtlasToRenderer(_float fTimeDelta)
{
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
	return E_NOTIMPL;
}

_uint CUI_Manager::TexTag_Maping(string szTextag)
{
	if ("Prototype_Component_Atlas_Base" == szTextag)
		return 0;
	else if ("Prototype_Component_Atlas_Main" == szTextag)
		return 1;

	return -1;
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
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);

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
