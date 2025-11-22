#pragma once
#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CUIObject;
NS_END

NS_BEGIN(Client)
class CUI_Layer;
class CAtlas_RenderGroup;

class CUI_Manager final : public CBase
{
private:
	CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CUI_Manager() = default;

public:
	HRESULT						Initialize();
	void						UIObjectToRenderer();

	//Fade 관련
	void						Fade_In(function<void()> FadeEvent);
	void						Fade_Out(function<void()> FadeEvent);
	_bool						Fade_End();

	//UI 관련
	CUIObject*					Get_RootUI(const _wstring& szRootUIName);
	HRESULT						UI_UpdateSwitch(const _wstring& szRootUIName, void* pArg);
	HRESULT						Add_RootUI(const _wstring& szRootUIName, CUIObject* pUIObject);
    HRESULT						Release_RootUI(const _wstring& szRootUIName);

	//Event 관련
	HRESULT						Add_Event(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void()> Event);
	HRESULT						Add_ParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void(void*)> Event);
	function<void()>			Pop_Event(const _wstring& strLayerTag, const _wstring& strEventTag);
	function<void(void*)>		Pop_ParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag);
	HRESULT						Erase_EventLayer(const _wstring& strLayerTag);

	//로드
	HRESULT						Load_UIData(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iPrototypeLevelID, const _tchar* pFilePath);
	CUIObject*					Load_UIObject(_uint iPrototypeLevelID, const _tchar* pFilePath);

	//랜더 관련
	HRESULT						Add_UIRender(UI_RENDER_TYPE eRender, CUIObject* pUIObject);
	
	//ETC
	_int						UIType_StringToEnum(string szUIType);
	_float4						Get_AtlasUV(const string pFrameName, _uint iTextureIndex);
private:
	class CGameInstance*		m_pGameInstance = { nullptr };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	unordered_map<_wstring,
		CUI_Layer*>				m_pUILayers;
	vector<CAtlas_RenderGroup*> m_pAtlasRenderGroup;
	CAtlas_RenderGroup*			m_pWorldRenderGroup;

	vector<CUIObject*>			m_pRenderUI;
	unordered_map<_wstring,
		CUIObject*>				m_pRootUI;

	class CUI_Fade*				m_pFadeUI = { nullptr };
private:
	CUI_Layer*					Find_Layer(const _wstring& strLayerTag);
	CUIObject*					Find_RootUI(const _wstring& strUITag);

	HRESULT						Ready_Prototype();


public:
	static CUI_Manager*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void				Free() override;
};

NS_END




