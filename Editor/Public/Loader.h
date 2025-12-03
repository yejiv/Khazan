#pragma once

#include "Editor_Defines.h"
#include "Base.h"

/* 로고, 게임플레이 -> 객체, 리소스들, 사운드, 쉐이더 ->자원준비를 거치는 과정 => 로딩 */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

	/* 세마포어 , 뮤텍스, 크리티컬섹션 */

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();

	_bool isFinished() {
		return m_isFinished;
	}

	void Show_LoadingText() {
		SetWindowText(g_hWnd, m_szLoadingText);
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL							m_eNextLevelID = { LEVEL::END };
	HANDLE							m_hThread = {};
	_tchar							m_szLoadingText[MAX_PATH] = {};
	_bool							m_isFinished = { false };
	CGameInstance* m_pGameInstance = { nullptr };
	CRITICAL_SECTION				m_CriticalSection = {};



private:
	HRESULT Loading_For_Editor_Level();
	HRESULT Loading_For_Map_Level();
	HRESULT Loading_For_Animation_Level();
	HRESULT Loading_For_Effect_Level();
	HRESULT Loading_For_UI_Level();
	HRESULT Loading_For_Shader_Level();
	HRESULT Loading_For_Camera_Level();
	HRESULT Loading_For_AI_Level();

private:
	// 모델 : 맵 오브젝트 Dat 로 프로토 타입 불러오는 함수 ( 1. 프로토타입 데이터 파일 명 | 2. 레벨 | 3. 맵 ( 안넣으면 폴더 X ) )
	HRESULT Loading_Prototype_MapObject_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 메쉬 인스턴스 : 맵 오브젝트 Dat 로 프로토 타입 불러오는 함수 ( 1. 프로토타입 데이터 파일 명 | 2. 레벨 | 3. 맵 ( 안넣으면 폴더 X ) )
	HRESULT Loading_Prototype_MapObject_Inst_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END