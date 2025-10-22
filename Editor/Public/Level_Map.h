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
	HRESULT Ready_Layer_Khazan(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Preview(const _wstring& strLayerTag);

private:
	// 맵 오브젝트 배치하면서 vector에 개별로 push_back 한거 nullptr 시 정리용
	void Clear_List();

	void Test_Player_Move(_float fTimeDelta);
	void Select_Fix_Object(_float fTimeDelta);
	void Select_Fix_Instance(_float fTimeDelta);
	void Select_Add_LightPoint(_float fTimeDelta);

#pragma region 변수
private:
#pragma region Object 수정 변수

	_uint m_iInstObjectCnt = {};			// InstanceID 부여용 ( 아직 미수정 )
	_uint m_iMapObjectCnt = {};				// ObjectID 부여용 ( ++로 부여 )

	vector<class CProp*> m_ObjectList;		// 오브젝트 리스트 ( 수정 편하게 할려고 )
	_int m_iObjectListIndex = {};			// 오브젝트 리스트 인덱스

	CProp* m_pFixPropObj = { nullptr };					// 피킹 시 받아올거 오브젝트 리스트 참고해서
	CTransform* m_pFixTransformCom = { nullptr };		// 픽스할 오브젝트의 트랜스폼

	FIX_OBJECT m_eFixType = { FIX_OBJECT::END };		// 픽스 타입

	_float3 m_vFixScale = {};			// GUI에서의 Fix할 오브젝트 행렬 편집용
	_float3 m_vFixRotation = {};		// GUI에서의 Fix할 오브젝트 행렬 편집용
	_float3 m_vFixPosition = {};		// GUI에서의 Fix할 오브젝트 행렬 편집용

	_matrix m_FixBaseMatrix = { XMMatrixIdentity() };		// 피킹, Fix 오브젝트의 원본 행렬 ( 유지 )
	_matrix m_FixWorldMatrix = {};							// 피킹, Fix 오브젝트의 월드 행렬
	
	_float m_fAddScale = { 0.005f };						// 오브젝트 Add 시 스케일 지정

	_bool m_isCameraPosAdd = { false };						// 카메라 위치에 오브젝트 생성할지 말지
	_float m_fAddPositionY = { 0.f };						// 오브젝트 높이 위치 Off-Set

	MAPOBJECT_PROPERTIES m_AddObjectProperties = {};		// 추가할 오브젝트의 속성 부여

	_bool m_isCheckRender = { false };
	MAPOBJECT_PROPERTIES m_RenderProperties = {};

#pragma endregion

#pragma region OBJECT SAVE, LOAD 변수

	_char m_szMapInfoFilePath[MAX_PATH] = { "../../Client/Bin/Data/Map/MapData/" };		// 맵 데이터 파일 Default 경로 "../../Client/Bin/Data/Map/MapData/"
	string m_strMapInfoFilePath = {};													// 맵 데이터 파일 Default 경로 string 용

	_char m_szMapInfoFileName[MAX_PATH] = {};				// 오브젝트 배치 정보 파일 저장, 로드할 파일 이름 ( ex : "HeinMach"_타입들.dat )
	string m_strMapInfoFileName = {};						// 오브젝트 배치 정보 파일 저장, 로드할 파일 이름 string 용

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

	_bool m_isMainWindow = { true };			// Map Editor 메인 윈도우

	_bool m_isPrototypeWindow = { false };		// 등록된 프로토타입 리스트 윈도우

	_bool m_isObjectWindow = { false };			// 오브젝트들 리스트 윈도우

	_bool m_isFixObjectWindow = { false };		// Fix 할 오브젝트 윈도우

	_bool m_isLightSettingWindow = { false };	// 조명 세팅 윈도우

	_bool m_isSaveObjectWindow = { false };		// 맵 데이터 세이브 윈도우

	_bool m_isLoadObjectWindow = { false };		// 맵 데이터 로드 윈도우
	_bool m_isLoaded = { false };				// 맵 데이터 로드됐는지 여부

#pragma endregion

#pragma region ImGui > MainWindow 관련 변수

	_bool m_isInformation = { false };

#pragma endregion

#pragma region FBX Convert 에 사용

	_char m_szFolderName[MAX_PATH] = {};									// 폴더 경로로 Prototype 등록, FBX to DAT 변환하는 변수

	_char m_szDataSavePath[MAX_PATH] = { "../../Client/Bin/Data/Map/" };	// .dat 추출용 폴더 경로 : "../../Client/Bin/Data/Map/"
	string m_strDataSavePath = {};											// .dat 추출용 폴더 경로 string 용

#pragma endregion

#pragma region PROTOTYPE LIST 용

	vector<string> m_Prototypes_Obj;					// Prototype 목록 ( Object 용 모델 )
	_int m_iIndex_PrtObj = {};							// Prototype Object 용 인덱스

	_char m_szSearchPrototypeName[MAX_PATH] = {};		// 등록된 프로토 타입 태그 검색용
	_char m_szSearchObjectName[MAX_PATH] = {};			// 등록된 오브젝트 모델 검색용

#pragma endregion

#pragma endregion

private:
	// MapEditor Init 단 ImGui Widget 생성
	HRESULT Ready_DefaultImGui_For_MapTool();

	// MapEditor Default 윈도우
	HRESULT Ready_Main_Window();
	// MapEditor 등록된 프로토 타입 리스트 윈도우
	HRESULT Ready_Prototype_List_Window();
	// MapEditor 맵 오브젝트 Fix 윈도우
	HRESULT Ready_Prop_Fix_Window();
	// MapEditor 맵 오브젝트 리스트 윈도우
	HRESULT Ready_Prop_List_Window();
	// MapEditor Light 세팅 윈도우
	HRESULT Ready_Light_Window();
	// MapEditor Object Save, Load 윈도우
	HRESULT Ready_Object_SaveLoad_Window();

private:
	// 폴더에 있는 .fbx 파일들 전부 순회하면서 지정한 경로 ( Bin/Data/Map/ ) 에 모델폴더/모델 생성 해주는 함수
	void Fbxs_Convert_To_Dat(const _char* pFolderName);

	// Bin/Data/Map/Prop/ 에 있는 지정한 폴더에 있는 .dat 파일들을 순회하면서 Prototype에 등록해주는 함수
	void Add_Prototype_ByFolder(const _char* pFolderName, _bool isAnim = false);

	// 모델이 있는 경로를 찾아주는 함수
	string Find_ModelPath(const string& strModelName, const string& strFileExtern);

#pragma region 파일 입출력

private:
	// CLoader 에서 사용할 바이너리 파일
	_bool Prototypes_Save_Binary();

	// 특정 레벨에서 사용할 오브젝트 바이너리 파일
	_bool Objects_Save_Binary();

	// 특정 레벨에서 사용할 라이트 바이너리 파일
	_bool Lights_Save_Binary();

	// MapEditor에서 프로토 타입 불러오기
	_bool Prototypes_Load_Binary();
	// MapEditor에서 오브젝트 불러오기
	_bool Objects_Load_Binary();
	// MapEditor에서 조명 불러오기
	_bool Lights_Load_Binary();

#pragma endregion

private:
	void MapEditor_Close_Windows();

public:
	static CLevel_Map* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
