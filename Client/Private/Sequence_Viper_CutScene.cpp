#include "Sequence_Viper_CutScene.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Obelisk.h"

CSequence_Viper_CutScene::CSequence_Viper_CutScene()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_Viper_CutScene::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{


    return S_OK;
}

void CSequence_Viper_CutScene::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;

    if (m_fTime > 0.f && !m_isFadeOut)
    {

    }

    if (m_fTime >= 3.f)
        m_isEnd = true;
}

void CSequence_Viper_CutScene::Pause()
{

}

void CSequence_Viper_CutScene::Resume()
{

}

void CSequence_Viper_CutScene::StopImmediate()
{

}

void CSequence_Viper_CutScene::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Viper_CutScene::IsEnd() const
{
    return m_isEnd;
}

CSequence_Viper_CutScene* CSequence_Viper_CutScene::Create()
{
    return new CSequence_Viper_CutScene();
}

void CSequence_Viper_CutScene::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

}
