#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Map final : public CLevel
{
private:
	enum class PROP_SPECIES { STATIC, ANIMATED, INTERACTIVE, DESTRUCTIBLE, END };
	enum class MAPEDIT_MAP { HEINMACH, STORMPASS, THECREVICE, EMBARS, END };

	typedef struct tagJsonMapData
	{
		string strModelName{};

		_uint iNumInstances{};
		vector<_float3> vInstancePos{};
		vector<_float3> vInstanceScale{};
		vector<_float4> vQuaternion{};

	}JSON_MAP_DATA;

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
	_char m_szJsonDefaultPath[MAX_PATH] = { "../../Client/Bin/Resources/Models/Prop/Json/" };
	_char m_szJsonFolderPath[ENUM_CLASS(MAPEDIT_MAP::END)][MAX_PATH] = { "HeinMach/", "StormPass/", "TheCrevice/", "Embars/" };
	MAPEDIT_MAP m_eMapType = { MAPEDIT_MAP::HEINMACH };
	_bool m_isJsonOpened = { false };
	_bool m_isJsonConverted = { false };
#pragma endregion

#pragma region STATIC WINDOW
	vector<string> m_JsonFiles;
	_int m_iJsonFilesIndex = {};

	vector<JSON_MAP_DATA> m_JsonList;
	_int m_iJsonListIndex = {};
#pragma endregion

private:
	class CProp_Static* m_pProp_Static = { nullptr };

private:
	HRESULT Ready_DefaultImGui_For_MapTool();

	HRESULT Ready_Main_Window();

	HRESULT Ready_Prop_Edit_Window();

	HRESULT Ready_Json_Edit_Window();

	HRESULT Ready_Json_List_Window();

	void Get_Directory_Files();
#pragma endregion

public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END