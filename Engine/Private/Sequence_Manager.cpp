#include "Sequence_Manager.h"
#include "GameInstance.h"

CSequence_Manager::CSequence_Manager()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_Manager::Initialize()
{
	Subscribe_REQ();

	return S_OK;
}

void CSequence_Manager::ProcessRequests()
{
	// EnqueueAdopt 
	while (!m_qAdopt.empty()) {
		auto [ISeq, desc] = m_qAdopt.front(); m_qAdopt.pop_front();
		AdoptAndPlay(ISeq, desc, false);
	}

	// Play
	while (!m_qPlay.empty())
	{
		auto tDesc = m_qPlay.front(); m_qPlay.pop_front();
		if (tDesc.tId.iInstance == 0) tDesc.tId.iInstance = m_iNextInstance++;
		SEQ_KEY tKey = _K(tDesc.tId);
		if (m_MapInstances.find(tKey) != m_MapInstances.end()) continue;

		ISeqInstance* pSeq = m_pFactory->Create(tDesc);
		if (!pSeq || FAILED(pSeq->Initialize(tDesc))) { Safe_Release(pSeq); continue; }

		m_MapInstances.emplace(tKey, pSeq);
		Emit_Started(tDesc.tId);
	}

	// Stop
	while (!m_qStop.empty())
	{
		auto tDesc = m_qStop.front(); m_qStop.pop_front();
		auto it = m_MapInstances.find(_K(tDesc.tId));
		if (it == m_MapInstances.end()) continue;
		it->second->StopImmediate();
		Emit_Ended(tDesc.tId, true);
		Safe_Release(it->second);
		m_MapInstances.erase(it);
	}

	// PAUSE
	while (!m_qPause.empty())
	{
		auto tDesc = m_qPause.front(); m_qPause.pop_front();
		auto it = m_MapInstances.find(_K(tDesc.tId));
		if (it != m_MapInstances.end()) it->second->Pause();
	}

	// RESUME
	while (!m_qResume.empty())
	{
		auto tDesc = m_qResume.front(); m_qResume.pop_front();
		auto it = m_MapInstances.find(_K(tDesc.tId));
		if (it != m_MapInstances.end()) it->second->Resume();
	}

	// JUMP
	while (!m_qJump.empty())
	{
		auto tDesc = m_qJump.front(); m_qJump.pop_front();
		auto it = m_MapInstances.find(_K(tDesc.tId));
		if (it == m_MapInstances.end()) continue;

		if (tDesc.pMarker) {
			// TODO: 마커명 → 시간 변환 (클라 시퀀스 내부/자산 로더와 협의)
		}
		else {
			it->second->Jump(tDesc.fTime);
		}
	}

}

void CSequence_Manager::Update(_float fTimeDelta)
{
	vector<SEQ_KEY> vecErase;
	vecErase.reserve(m_MapInstances.size());
	for (auto& kv : m_MapInstances)
	{
		ISeqInstance* pSeq = kv.second;
		pSeq->Update(fTimeDelta);
		if (pSeq->IsEnd())
		{
			Emit_Ended(pSeq->GetId(), false);
			vecErase.push_back(kv.first);
		}
	}

	for (auto& k : vecErase)
	{
		auto it = m_MapInstances.find(k);
		if (it == m_MapInstances.end()) continue;
		Safe_Release(it->second);
		m_MapInstances.erase(it);
	}
}

HRESULT CSequence_Manager::Play(const SEQ_REQ_PLAY_DESC& tDecs)
{
	SEQ_REQ_PLAY_DESC tPlayDesc = tDecs;

	if (tPlayDesc.tId.iInstance == 0) 
		tPlayDesc.tId.iInstance = m_iNextInstance++;

	SEQ_KEY k = _K(tPlayDesc.tId);
	if (m_MapInstances.find(k) != m_MapInstances.end()) 
		return E_FAIL;

	ISeqInstance* pSeq = m_pFactory->Create(tPlayDesc);
	if (!pSeq || FAILED(pSeq->Initialize(tPlayDesc))) 
	{ 
		Safe_Release(pSeq); 
		return E_FAIL; 
	}

	m_MapInstances.emplace(k, pSeq);
	Emit_Started(tPlayDesc.tId);

	return S_OK;
}

HRESULT CSequence_Manager::Stop(const SEQ_ID& tId, _bool isImmediate)
{
	static_cast<void>(isImmediate);
	auto it = m_MapInstances.find(_K(tId));

	if (it == m_MapInstances.end()) 
		return E_FAIL;

	it->second->StopImmediate();
	Emit_Ended(tId, true);
	Safe_Release(it->second);
	m_MapInstances.erase(it);

	return S_OK;
}

HRESULT CSequence_Manager::Pause(const SEQ_ID& tId)
{
	auto it = m_MapInstances.find(_K(tId));

	if (it == m_MapInstances.end()) 
		return E_FAIL;

	it->second->Pause();

	return S_OK;
}

HRESULT CSequence_Manager::Resume(const SEQ_ID& tId)
{
	auto it = m_MapInstances.find(_K(tId));

	if (it == m_MapInstances.end()) 
		return E_FAIL;

	it->second->Resume();
	return S_OK;
}

HRESULT CSequence_Manager::Jump(const SEQ_REQ_JUMP_DESC& tDesc)
{
	auto it = m_MapInstances.find(_K(tDesc.tId));

	if (it == m_MapInstances.end()) 
		return E_FAIL;

	if (tDesc.pMarker) {
		// TODO: 마커명 → 시간 변환
		return S_OK;
	}
	else {
		it->second->Jump(tDesc.fTime);
		return S_OK;
	}
}

HRESULT CSequence_Manager::AdoptAndPlay(ISeqInstance* pSeq, SEQ_REQ_PLAY_DESC tDesc, _bool isInit)
{
	if (pSeq == nullptr)
		return E_FAIL;

	if (tDesc.tId.iInstance == 0)
		tDesc.tId.iInstance = m_iNextInstance++;

	auto key = _K(tDesc.tId);
	if (m_MapInstances.find(key) != m_MapInstances.end()) 
	{
		Safe_Release(pSeq);
		return E_FAIL;
	}

    if (!isInit)
    {
        if (FAILED(pSeq->Initialize(tDesc)))
        {
            Safe_Release(pSeq);
            return E_FAIL;
        }
    }
	    
	m_MapInstances.emplace(key, pSeq);

	Emit_Started(tDesc.tId);

	return S_OK;
}

void CSequence_Manager::EnqueueAdopt(ISeqInstance* pSeq, const SEQ_REQ_PLAY_DESC& tDesc)
{
	m_qAdopt.emplace_back(pSeq, tDesc);
}

void CSequence_Manager::Subscribe_REQ()
{
	m_SubscribeTokens.reserve(5);

	m_SubscribeTokens.push_back(
		m_pGameInstance->Subscribe_Event<SEQ_REQ_PLAY_DESC>(
			SEQ_REQ_PLAY,
			[this](const SEQ_REQ_PLAY_DESC& d) { m_qPlay.push_back(d); })
	);

	m_SubscribeTokens.push_back(
		m_pGameInstance->Subscribe_Event<SEQ_REQ_STOP_DESC>(
			SEQ_REQ_STOP,
			[this](const SEQ_REQ_STOP_DESC& d) { m_qStop.push_back(d); })
	);

	m_SubscribeTokens.push_back(
		m_pGameInstance->Subscribe_Event<SEQ_REQ_PAUSE_DESC>(
			SEQ_REQ_PAUSE,
			[this](const SEQ_REQ_PAUSE_DESC& d) { m_qPause.push_back(d); })
	);

	m_SubscribeTokens.push_back(
		m_pGameInstance->Subscribe_Event<SEQ_REQ_RESUME_DESC>(
			SEQ_REQ_RESUME,
			[this](const SEQ_REQ_RESUME_DESC& d) { m_qResume.push_back(d); })
	);

	m_SubscribeTokens.push_back(
		m_pGameInstance->Subscribe_Event<SEQ_REQ_JUMP_DESC>(
			SEQ_REQ_JUMP,
			[this](const SEQ_REQ_JUMP_DESC& d) { m_qJump.push_back(d); })
	);

	//// REQ_* 수신 → 내부 큐 적재
	//m_pGameInstance->Subscribe_Event<SEQ_REQ_PLAY_DESC>(SEQ_REQ_PLAY,
	//	[this](const SEQ_REQ_PLAY_DESC& d) { m_qPlay.push_back(d); });

	//m_pGameInstance->Subscribe_Event<SEQ_REQ_STOP_DESC>(SEQ_REQ_STOP,
	//	[this](const SEQ_REQ_STOP_DESC& d) { m_qStop.push_back(d); });

	//m_pGameInstance->Subscribe_Event<SEQ_REQ_PAUSE_DESC>(SEQ_REQ_PAUSE,
	//	[this](const SEQ_REQ_PAUSE_DESC& d) { m_qPause.push_back(d); });

	//m_pGameInstance->Subscribe_Event<SEQ_REQ_RESUME_DESC>(SEQ_REQ_RESUME,
	//	[this](const SEQ_REQ_RESUME_DESC& d) { m_qResume.push_back(d); });

	//m_pGameInstance->Subscribe_Event<SEQ_REQ_JUMP_DESC>(SEQ_REQ_JUMP,
	//	[this](const SEQ_REQ_JUMP_DESC& d) { m_qJump.push_back(d); });
}

void CSequence_Manager::Emit_Started(const SEQ_ID& tId)
{
	SEQ_SIG_COMMON sig{ tId };

	m_pGameInstance->Emit_Event<SEQ_SIG_COMMON>(SEQ_SIG_STARTED, sig);
}

void CSequence_Manager::Emit_Ended(const SEQ_ID& tId, _bool isInterrupted)
{
	SEQ_SIG_COMMON sig{ tId };
	m_pGameInstance->Emit_Event<SEQ_SIG_COMMON>(isInterrupted ? SEQ_SIG_INTERRUPTED : SEQ_SIG_ENDED, sig);
}

void CSequence_Manager::Clear()
{
    //for (_uint token : m_SubscribeTokens)
    //    m_pGameInstance->UnsubscribeAll_Event(token);
    //m_SubscribeTokens.clear();

    for (auto& it : m_qAdopt)
        Safe_Release(it.first);
    m_qAdopt.clear();


    for (auto& kv : m_MapInstances)
        Safe_Release(kv.second);
    m_MapInstances.clear();


    m_qPlay.clear();
    m_qStop.clear();
    m_qPause.clear();
    m_qResume.clear();
    m_qJump.clear();

    m_pFactory = nullptr;
}

CSequence_Manager* CSequence_Manager::Create()
{
	CSequence_Manager* pInstance = new CSequence_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CSequence_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Manager::Free()
{
	__super::Free();

	for (_uint token : m_SubscribeTokens)
		m_pGameInstance->UnsubscribeAll_Event(token);
	m_SubscribeTokens.clear();


	for (auto& it : m_qAdopt)
		Safe_Release(it.first); // ISeqInstance*
	m_qAdopt.clear();


	for (auto& kv : m_MapInstances)
		Safe_Release(kv.second);
	m_MapInstances.clear();


	m_qPlay.clear();
	m_qStop.clear();
	m_qPause.clear();
	m_qResume.clear();
	m_qJump.clear();

	m_pFactory = nullptr;

	Safe_Release(m_pGameInstance);
}
