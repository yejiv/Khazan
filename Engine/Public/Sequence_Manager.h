#pragma once
#include "Base.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)

class CSequence_Manager : public CBase
{
private:
	CSequence_Manager();
	virtual ~CSequence_Manager() = default;

public:
	HRESULT Initialize();
	void ProcessRequests(); // REQ* 큐 처리(생성 / 상태 변경)
	void Update(_float fTimeDelta);

	HRESULT Play(const SEQ_REQ_PLAY_DESC& tDecs);
	HRESULT Stop(const SEQ_ID& tId, _bool isImmediate);
	HRESULT Pause(const SEQ_ID& tId);
	HRESULT Resume(const SEQ_ID& tId);
	HRESULT Jump(const SEQ_REQ_JUMP_DESC& tDesc);

public:
	// 클라이언트가 생성한 인스턴스를 매니저가 소유/재생 시작
	HRESULT AdoptAndPlay(ISeqInstance* pSeq, SEQ_REQ_PLAY_DESC tDesc);
	// 다음 프레임 처리용 큐
	void    EnqueueAdopt(ISeqInstance* pSeq, const SEQ_REQ_PLAY_DESC& tDesc);

private:
	void Subscribe_REQ();
	void Emit_Started(const SEQ_ID& tId);
	void Emit_Ended(const SEQ_ID& tId, _bool isInterrupted);

public:
    void Clear();

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ISeqFactory* m_pFactory = { nullptr }; // 소유X(필요시 외부에서 해제)

	// 인스턴스 관리
	unordered_map<SEQ_KEY, ISeqInstance*, KeyHash, KeyEq> m_MapInstances;
	deque<std::pair<ISeqInstance*, SEQ_REQ_PLAY_DESC>> m_qAdopt;

	// 요청 큐
	deque<SEQ_REQ_PLAY_DESC>   m_qPlay;
	deque<SEQ_REQ_STOP_DESC>   m_qStop;
	deque<SEQ_REQ_PAUSE_DESC>  m_qPause;
	deque<SEQ_REQ_RESUME_DESC> m_qResume;
	deque<SEQ_REQ_JUMP_DESC>   m_qJump;

	_uint m_iNextInstance = 1; // 동시 재생 대비 자동 인스턴스 번호

	vector<_uint> m_SubscribeTokens;

public:
	static CSequence_Manager* Create();
	virtual void Free() override;

};

NS_END