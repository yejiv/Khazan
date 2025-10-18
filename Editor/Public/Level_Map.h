#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Engine)
class CTransform;
NS_END

NS_BEGIN(Editor)

class CLevel_Map final : public CLevel
{
private:
	enum class PROP_SPECIES { OBJECT, STATIC, ANIMATED, INTERACTIVE, DESTRUCTIBLE, END };
	enum class MAPEDIT_MAP { HEINMACH, STORMPASS, THECREVICE, EMBARS, END };
	enum class FIX_OBJECT { SCALE_DETAIL, FIX_ALL, SCALE, ROTATION, POSITION, END };

private:
	CLevel_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Map() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Defaults();
	HRESULT Ready_Default_Lights();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);

	HRESULT Ready_Layer_Prop_Static(const _wstring& strLayerTag);

private:
	class CProp_Static* m_pProp_Static = { nullptr };

	MAPEDIT_MAP m_eMapType = { MAPEDIT_MAP::HEINMACH };

private:
	// JSON으로부터 읽어와서 Prototype 세팅
	HRESULT Add_Prototypes_FromJson();
	HRESULT Convert_Json_To_Data();

	// 맵 오브젝트 배치하면서 vector에 개별로 push_back 한거 nullptr 시 정리용
	void Clear_List();

	void Select_Fix_Object(_float fTimeDelta);
	void Select_Fix_Instance(_float fTimeDelta);
	void Select_Add_LightPoint(_float fTimeDelta);

#pragma region 변수
private:
#pragma region Object 수정 변수

	_uint m_iInstObjectCnt = {};			// InstanceID 부여용 ( 아직 미수정 )
	_uint m_iMapObjectCnt = {};				// ObjectID 부여용 ( ++로 부여 )

	vector<class CProp*> m_ObjectList;		// 오브젝트 리스트 ( 수정 편하게 할려고 )

	CProp* m_pFixPropObj = { nullptr };		// 피킹 시 받아올거 오브젝트 리스트 참고해서
	CTransform* m_pFixTransformCom = { nullptr };		// 픽스할 오브젝트의 트랜스폼

	FIX_OBJECT m_eFixType = { FIX_OBJECT::END };		// 픽스 타입

	_char m_szModelName[MAX_PATH] = {};

	_float3 m_vFixScale = {};
	_float3 m_vFixRotation = {};
	_float3 m_vFixPosition = {};

#pragma endregion

#pragma region LIGHT 수정 변수

	LIGHT_DESC m_LightDesc = {};				// 추가할 조명 구조체

	_char m_szLightTag[MAX_PATH] = {};			// 추가할 조명 태그
	string m_strLightTag = {};					// 변환용

	vector<string> m_LightTags;					// 조명 태그 모음 ( ADD 한 애들 )
	_int m_iLightTagIndex = {};					// 조명 벡터 인덱스

	_bool m_isAddLight = { false };				// 조명 추가 시 활성화
	_bool m_isFixLight = { false };				// 조명 수정 시 활성화
	_bool m_isFindFixLight = { false };			// FIX 할거 찾으면 활성화

	LIGHT_DESC m_FixLightDesc = {};				// 수정할 조명 구조체

	_char m_szFixLightTag[MAX_PATH] = {};		// 수정할 조명 태그
	string m_strFixLightTag = {};				// 변환용

	_bool m_isAddLightPoint = { false };		// 피킹 조명 위치용 BOOL
	_float3 m_vLightPoint = {};					// 피킹 조명 위치용 XMFLOAT3

#pragma endregion

#pragma region ImGui 윈도우 창

	_bool m_isMainWindow = { true };

	_bool m_isJsonWindow = { false };

	_bool m_isCustomJsonWindow = { false };

	_bool m_isPrototypeWindow = { false };

	_bool m_isObjectWindow = { false };

	_bool m_isPropWindow[ENUM_CLASS(PROP_SPECIES::END)] = { false, false, false, false,false };

	_bool m_isFixObjectWindow = { false };

	_bool m_isLightSettingWindow = { false };

#pragma endregion

#pragma region ImGui > MainWindow 관련 변수

	_bool m_isInformation = { false };

#pragma endregion

#pragma region ImGui > JSON 관련 폴더 경로 및 파일 명

	_char m_szJsonPath[MAX_PATH] = { "../../Client/Bin/Resources/Map/Json/" };						// 오리지날 Json 기본 경로
	_char m_szJsonCustomPath[MAX_PATH] = { "../../Client/Bin/Resources/Map/Json/CustomJson/" };		// 커 스 텀 Json 기본 경로

	_char m_szJsonFolderPath[ENUM_CLASS(MAPEDIT_MAP::END)][MAX_PATH] = { "HeinMach/", "StormPass/", "TheCrevice/", "Embars/" };		// 추출할 Json 폴더

	_char m_szJsonSaveName[MAX_PATH] = {};		// Json 이름

#pragma endregion

#pragma region 추출한 JSON 있는 폴더의 JSON 목록

	vector<string> m_JsonFiles;					// JsonFile 이름 명 ( Combo에서 볼 Json 폴더 경로의 .json 파일들 )
	_int m_iJsonFilesIndex = {};				// ImGui::BeginListBox 용 인덱스 변수

#pragma endregion

#pragma region ORIGINAL JSON 용

	JSON m_Json = {};							// Original Json 정보 저장해놓을 JSON
	_bool m_isJsonExport = { false };			// 추출 됬는지 확인용

	_bool m_isJsonConverted = { false };		// 변환됬는지 확인 용

	vector<JSON_MAP_DATA> m_JsonList;			// Original Json 맵 데이터 용 벡터
	_int m_iJsonListIndex = {};					// ImGui::BeginListBox 용 인덱스 변수

	_int m_iObjCnt = {};						// 단일 객체 갯수 확인용

#pragma endregion

#pragma region CUSTOM JSON 용

	JSON m_CustomJson = {};								// Custom Json 정보 저장해놓을 JSON

	vector<JSON_MAP_DATA> m_CustomJsonList;				// Custom Json 맵 데이터 용 벡터
	_int m_iCustomJsonListIndex = {};					// ImGui::BeginListBox 용 인덱스 변수

	_bool m_isCustomJsonLoaded = { false };				// Custom Json 로드 됬는지 확인 용
	_bool m_isCustomJsonInfoList = { false };			// List Info 창 ON/OFF

	map<const string, const string> m_CheckPrototypes;	// 중복 프로토타입 체크 및 리스트 출력용

	_char m_szDatSavePath[MAX_PATH] = { "../../Client/Bin/Data/Map/" };			// .dat 추출용 폴더 경로
	string m_strDatSavePath = {};

#pragma endregion

#pragma region PROTOTYPE LIST 용

	vector<string> m_Prototypes_Inst;					// Prototype 목록 ( Instance 용 모델 )
	_int m_iIndex_PrtInst = {};							// Prototype Instance 용 인덱스

	vector<string> m_Prototypes_Obj;					// Prototype 목록 ( Object 용 모델 )
	_int m_iIndex_PrtObj = {};							// Prototype Object 용 인덱스

#pragma endregion

#pragma endregion

private:
	// MapEditor Init 단 ImGui Widget 생성
	HRESULT Ready_DefaultImGui_For_MapTool();

	// MapEditor Default 윈도우
	HRESULT Ready_Main_Window();
	// MapEditor Layer 수정 윈도우 ( 아직 기능 X )
	HRESULT Ready_Prop_Edit_Window();
	// MapEditor Custom Json 수정 윈도우
	HRESULT Ready_CustomJson_Edit_Window();
	// MapEditor Custom Json 리스트 윈도우
	HRESULT Ready_CustomJson_List_Window();
	// MapEditor Original Json 수정 윈도우
	HRESULT Ready_Json_Edit_Window();
	// MapEditor Original Json 리스트 윈도우
	HRESULT Ready_Json_List_Window();
	// MapEditor Light 세팅 윈도우
	HRESULT Ready_Light_Window();

	// Directory에 파일들 불러오는용 ( Json 한정 함수 )
	void Get_Directory_Files(const _char* pDirectoryPath);

	// 임시 테스트용
	string Find_FBX_ModelPath(const string& strModelName);

public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END