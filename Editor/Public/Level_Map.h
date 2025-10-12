#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Map final : public CLevel
{
private:
	enum class PROP_SPECIES { STATIC, ANIMATED, INTERACTIVE, DESTRUCTIBLE, END };

private:
	CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Map() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);

	HRESULT Ready_Layer_Prop_Static(const _wstring& strLayerTag);

#pragma region ImGui
private:
#pragma region MAIN WINDOW
	_bool m_isMainWindow = { true };

	_bool m_isJsonWindow = { false };

	_bool m_isPropWindow[ENUM_CLASS(PROP_SPECIES::END)] = { false, false, false,false };

	_bool m_isLightSettingWindow = { false };
#pragma endregion

#pragma region JSON
	JSON m_Json = {};
	_char m_szJsonFile[MAX_PATH] = {};
	_bool m_isJsonOpened = { false };
#pragma endregion

#pragma region STATIC WINDOW
	vector<string> m_StaticModels;
	_int m_iStatIndex = {};
#pragma endregion

private:
	class CProp_Static* m_pProp_Static = { nullptr };

private:
	HRESULT Ready_DefaultImGui_For_MapTool();

	HRESULT Ready_Main_Window();

	HRESULT Ready_Prop_Edit_Window();

	HRESULT Ready_Json_Edit_Window();
#pragma endregion

public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END