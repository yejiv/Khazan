#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Map final : public CLevel
{
private:
	enum class PROP_SPECIES { STATIC, ANIMATED, INTERACTIVE, DESTRUCTIBLE, END };
	enum class MAPEDIT_MAP { HEINMACH, STORMPASS, THECREVICE, EMBARS, END };

private:
	CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Map() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Defaults();
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);

	HRESULT Ready_Layer_Prop_Static(const _wstring& strLayerTag);

private:
	class CProp_Static* m_pProp_Static = { nullptr };

	MAPEDIT_MAP m_eMapType = { MAPEDIT_MAP::HEINMACH };

private:
	// 임시 테스트용 : 모델 or 모델 인스턴스 Prototype 생성 > GameObject 클론 ( json, filesystem 이용해서 )
	HRESULT Ready_Temp_Instances();
	HRESULT Ready_Temp_IndependentObjs();
	HRESULT Ready_Temp_All();

	// 임시 테스트용
	HRESULT Ready_Temp_Layers(const _wstring& strLayerTag);

#pragma region ImGui 변수들

	_bool m_isMainWindow = { true };

	_bool m_isJsonWindow = { false };

	_bool m_isCustomJsonWindow = { false };

	_bool m_isPropWindow[ENUM_CLASS(PROP_SPECIES::END)] = { false, false, false,false };

	_bool m_isLightSettingWindow = { false };

#pragma region ImGui > JSON 관련 변수

	_char m_szJsonPath[MAX_PATH] = { "../../Client/Bin/Resources/Models/Prop/Json/" };					// 오리지날 Json 기본 경로
	_char m_szJsonCustomPath[MAX_PATH] = { "../../Client/Bin/DataFiles/Map/" };		// 커 스 텀 Json 기본 경로

	_char m_szJsonFolderPath[ENUM_CLASS(MAPEDIT_MAP::END)][MAX_PATH] = { "HeinMach/", "StormPass/", "TheCrevice/", "Embars/" };		// 추출할 Json 폴더

#pragma region ORIGINAL JSON 용

	JSON m_Json = {};		// Json 정보 저장해놓을
	_char m_szJsonSaveName[MAX_PATH] = {};
	
	_bool m_isJsonExport = { false };

	_bool m_isJsonConverted = { false };
	vector<string> m_JsonFiles;
	_int m_iJsonFilesIndex = {};

	vector<JSON_MAP_DATA> m_JsonList;
	_int m_iJsonListIndex = {};

	_int m_iObjCnt = {};

#pragma endregion

#pragma region CUSTOM JSON 용

	JSON m_CustomJson = {};		// Json 정보 저장해놓을

	vector<JSON_MAP_DATA> m_CustomJsonList;
	_int m_iCustomJsonListIndex = {};

	_bool m_isCustomJsonLoaded = { false };

	map<const _wstring, _uint> m_CheckPrototypes;

#pragma endregion


#pragma endregion

#pragma region ImGui > FileSystem 관련 변수



#pragma endregion

private:
	HRESULT Ready_DefaultImGui_For_MapTool();

	HRESULT Ready_Main_Window();
	HRESULT Ready_Prop_Edit_Window();
	HRESULT Ready_CustomJson_Edit_Window();
	HRESULT Ready_CustomJson_List_Window();
	HRESULT Ready_Json_Edit_Window();
	HRESULT Ready_Json_List_Window();

	void Get_Directory_Files(const _char* pDirectoryPath);

	// 임시 테스트용
	string Find_ModelPath(const string& strModelName);

#pragma endregion

public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END