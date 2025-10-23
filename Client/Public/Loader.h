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

	vector<future<any>> m_futures;
	atomic<_bool> m_isFinished = { false };
	atomic<int> m_progress = { 0 };

	mutex	m_CommitMutex;
	vector<function<void()>> m_Commits;

private:
	void EnqueueCommit(function<void()> fn) {
		lock_guard<mutex> lg(m_CommitMutex);
		m_Commits.emplace_back(move(fn));
	}
	void FlushCommits() {
		vector<function<void()>> local;
		{
			lock_guard<mutex> lg(m_CommitMutex);
			local.swap(m_Commits);
		}
		for (auto& fn : local) fn();
	}

private:
	HRESULT Loading_For_Title_Level();

	HRESULT Loading_For_Stage1_Level();
	HRESULT Loading_For_Stage1_Texture();
	HRESULT Loading_For_Stage1_Model();
	HRESULT Loading_For_Stage1_Shader();
	HRESULT Loading_For_Stage1_GameObject();

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