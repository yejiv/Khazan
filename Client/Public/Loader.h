#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 로고, 게임플레이 -> 객체, 리소스들, 사운드, 쉐이더 ->자원준비를 거치는 과정 => 로딩 */

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CLoader final : public CBase
{
public:
	struct CoInitGuard {
		HRESULT hr = S_OK;
		CoInitGuard(DWORD opt = COINIT_MULTITHREADED) { hr = CoInitializeEx(nullptr, opt); }
		~CoInitGuard() { if (SUCCEEDED(hr)) CoUninitialize(); }
	};
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	_bool AllReady(const std::vector<std::future<HRESULT>>& futures);
	void Update();
	HRESULT Loading();

	_bool isFinished() {
		return m_isFinished;
	}

	void Show_LoadingText() {
		SetWindowText(g_hWnd, m_szLoadingText);
	}

private:
	ID3D11Device*					m_pDevice = { nullptr }; 
	ID3D11DeviceContext*			m_pContext = { nullptr };
	LEVEL							m_eNextLevelID = { LEVEL::END };
	_tchar							m_szLoadingText[MAX_PATH] = {};
	CGameInstance*					m_pGameInstance = { nullptr };

	vector<future<HRESULT>> m_futures;
	//_bool m_isFinished = false;
	//atomic<_bool> m_isFinished = { false };

	HANDLE							m_hThread = {};
	_bool							m_isFinished = { false };
	CRITICAL_SECTION				m_CriticalSection = {};
private:
	HRESULT Loading_For_Title_Level();

	HRESULT Loading_For_Stage1_Level();
	HRESULT Loading_For_Stage1_Texture();
	HRESULT Loading_For_Stage1_Model();
	HRESULT Loading_For_Stage1_Shader();
	HRESULT Loading_For_Stage1_GameObject();

private:
	// 모델 : 맵 오브젝트 Dat 로 프로토타입 불러오는 함수 ( 1. 프로토타입 데이터 파일 명 | 2. 레벨 | 3. 맵 ( 안넣으면 폴더 X ) )
	HRESULT Loading_Prototype_MapObject_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 메쉬 인스턴싱 : 맵 오브젝트 Dat 로 프로토타입 불러오는 함수 ( 1. 프로토타입 데이터 파일 명 | 2. 레벨 | 3. 맵 ( 안넣으면 폴더 X ) )
	HRESULT Loading_Prototype_MapObject_Inst_From_DAT(const _tchar* pPrototypeDataFileName, LEVEL eLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END


