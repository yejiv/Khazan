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
	enum class FIX_OBJECT { FIX, END };

	typedef struct tagSavePrototype
	{
		MAPOBJECT_TYPE eType{};
		string strModelPath{};

	}SAVE_PROTOTYPE;

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
	HRESULT Ready_Layer_Khazan(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Preview(const _wstring& strLayerTag);

private:
	class CProp_Static* m_pProp_Static = { nullptr };

	MAPEDIT_MAP m_eMapType = { MAPEDIT_MAP::HEINMACH };

private:
	ID3D11DepthStencilView* m_pDSV_MapTool = { nullptr };

private:
	// 맵 오브젝트 배치하면서 vector에 개별로 push_back 한거 nullptr 시 정리용
	void Clear_List();

	void Test_Player_Move(_float fTimeDelta);
	void Select_Fix_Object(_float fTimeDelta);
	void Select_Fix_Instance(_float fTimeDelta);
	void Select_Add_LightPoint(_float fTimeDelta);

#pragma region 변수
private:
	_char m_szFolderName[MAX_PATH] = {};	// 폴더 경로로 Prototype 등록하는 변수

#pragma region Object 수정 변수

	_uint m_iInstObjectCnt = {};			// InstanceID 부여용 ( 아직 미수정 )
	_uint m_iMapObjectCnt = {};				// ObjectID 부여용 ( ++로 부여 )

	vector<class CProp*> m_ObjectList;		// 오브젝트 리스트 ( 수정 편하게 할려고 )
	_int m_iObjectListIndex = {};			// 오브젝트 리스트 인덱스

	_bool m_isSnow = { false };
	_bool m_isCollider = { false };
	_bool m_isBlend = { false };

	CProp* m_pFixPropObj = { nullptr };					// 피킹 시 받아올거 오브젝트 리스트 참고해서
	CTransform* m_pFixTransformCom = { nullptr };		// 픽스할 오브젝트의 트랜스폼

	FIX_OBJECT m_eFixType = { FIX_OBJECT::END };		// 픽스 타입

	_char m_szModelName[MAX_PATH] = {};

	_float3 m_vFixScale = {};			// ImGui 행렬 편집용
	_float3 m_vFixRotation = {};		// ImGui 행렬 편집용
	_float3 m_vFixPosition = {};		// ImGui 행렬 편집용

	_matrix m_FixBaseMatrix = { XMMatrixIdentity() };		// 피킹, Fix 오브젝트의 원본 행렬 ( 유지 )
	_matrix m_FixWorldMatrix = {};							// 피킹, Fix 오브젝트의 월드 행렬

	_uint m_iMapObjectShaderPass = { 2 };
	
	_float m_fAddScale = { 0.005f };

	_bool m_isCameraPosAdd = { false };
	_float m_fAddPositionY = { 0.f };

	MAPOBJECT_PROPERTIES m_AddObjectProperties = {};


#pragma endregion

#pragma region OBJECT SAVE, LOAD 변수

	_char m_szMapInfoFilePath[MAX_PATH] = { "../../Client/Bin/Data/Map/MapData/" };
	string m_strMapInfoFilePath = {};

	_char m_szMapInfoFileName[MAX_PATH] = {};				// 오브젝트 배치 정보 파일 저장, 로드할 파일 이름
	string m_strMapInfoFileName = {};

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

	_bool m_isPropWindow = { false };

	_bool m_isFixObjectWindow = { false };

	_bool m_isLightSettingWindow = { false };

	_bool m_isSaveObjectWindow = { false };

	_bool m_isLoadObjectWindow = { false };
	_bool m_isLoaded = { false };

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

#pragma region 변수 ( 아직 이름 못 붙임 )

	_int m_iObjCnt = {};						// 단일 객체 갯수 확인용

	map<const string, const string> m_CheckPrototypes;	// 중복 프로토타입 체크 및 리스트 출력용

	_char m_szDataSavePath[MAX_PATH] = { "../../Client/Bin/Data/Map/" };			// .dat 추출용 폴더 경로
	string m_strDataSavePath = {};

#pragma endregion

#pragma region PROTOTYPE LIST 용

	vector<string> m_Prototypes_Obj;					// Prototype 목록 ( Object 용 모델 )
	_int m_iIndex_PrtObj = {};							// Prototype Object 용 인덱스

	_char m_szSearchModelName[MAX_PATH] = {};
	_char m_szSearchObjectName[MAX_PATH] = {};

#pragma endregion

#pragma endregion

private:
	// MapEditor Init 단 ImGui Widget 생성
	HRESULT Ready_DefaultImGui_For_MapTool();

	// MapEditor Default 윈도우
	HRESULT Ready_Main_Window();
	// MapEditor Layer 수정 윈도우 ( 아직 기능 X )
	HRESULT Ready_Prop_Edit_Window();
	// MapEditor Light 세팅 윈도우
	HRESULT Ready_Light_Window();
	// MapEditor Object Save, Load 윈도우
	HRESULT Ready_Object_SaveLoad_Window();

	// Directory에 파일들 불러오는용 ( Json 한정 함수 )
	void Get_Directory_Files(const _char* pDirectoryPath);

	void Fbxs_Convert_To_Dat(const _char* pFolderName);

	void Add_Prototype_ByFolder(const _char* pFolderName, _bool isAnim = false);

	// 임시 테스트용
	string Find_ModelPath(const string& strModelName, const string& strFileExtern);

#pragma region 파일 입출력

private:
	// Loader 에서 사용할 바이너리 파일
	_bool Prototypes_Save_Binary();

	// Layer 에서 사용할 오브젝트 바이너리 파일
	_bool Objects_Save_Binary();

	// Layer 에서 사용할 라이트 바이너리 파일
	_bool Lights_Save_Binary();

	// MapEditor에서 불러오기
	_bool Prototypes_Load_Binary();
	_bool Objects_Load_Binary();
	_bool Lights_Load_Binary();

#pragma endregion

private:
	void MapEditor_Close_Windows();

public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
