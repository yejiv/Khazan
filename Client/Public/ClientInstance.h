#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "DB_Manager.h"

NS_BEGIN(Engine)
class CUIObject;
NS_END
NS_BEGIN(Client)

class CClientInstance final : public CBase
{
	DECLARE_SINGLETON(CClientInstance)

private:
	CClientInstance();
	virtual ~CClientInstance() = default;

#pragma region CLIENT
public:
	HRESULT Initialize(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update(_float fTimeDelta);
#pragma endregion

#pragma region DB_MANGER
	HRESULT						Load_Data(DATATYPE eType, const _tchar* pFilePath);
	template <typename T>
	const T* Get_Data(_uint iID) const;

#pragma endregion

#pragma region UI_MANGER
	//Event 관련
	HRESULT						Add_UIEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void()> Event);
	HRESULT						Add_UIParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag, std::function<void(void*)> Event);
	function<void()>			Pop_UIEvent(const _wstring& strLayerTag, const _wstring& strEventTag);
	function<void(void*)>		Pop_UIParamEvent(const _wstring& strLayerTag, const _wstring& strEventTag);
	HRESULT						Erase_UIEventLayer(const _wstring& strLayerTag);

	//UI JSON 로드
	HRESULT						Load_UIData(_uint iLayerLevelID, const _wstring& strLayerTag, _uint iPrototypeLevelID, const _tchar* pTextureFilePath);
	CUIObject*					Load_UIObject(_uint iPrototypeLevelID, const _tchar* pFilePath);
	
	_int						UIType_StringToEnum(string szUIType);

	//UI 랜더 그룹에 추가
	HRESULT						Add_UIRender(UI_RENDER_TYPE eRender, class CUIObject* pUIObject);

	//UI 관련 함수
	HRESULT						UI_UpdateSwitch(const _wstring& szRootUIName, void* pArg = nullptr);
	class CUIObject*			Get_RootUI(const _wstring& szRootUIName);
	
	//ETC
	_float4						Get_AtlasUV(const string pFrameName, _uint iTextureIndex);
#pragma endregion
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	
	class CUI_Manager*	m_pUI_Manager = { nullptr };
	CDB_Manager*		m_pDB_Manager = { nullptr };
public:
	virtual void Free() override;
};

template<typename T>
inline const T* CClientInstance::Get_Data(_uint iID) const
{
	return m_pDB_Manager->Get_Data<T>(iID);;
}
NS_END