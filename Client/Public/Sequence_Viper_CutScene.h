#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)
class CGameInstance;
class CTransform;
class CCharacterVirtual;
class ISeqInstance;
NS_END

NS_BEGIN(Client)

class CSequence_Viper_CutScene : public ISeqInstance
{
private:
    CSequence_Viper_CutScene();
    virtual ~CSequence_Viper_CutScene() = default;

public:
    HRESULT Initialize(const SEQ_REQ_PLAY_DESC& tDesc) override;
    void    Update(_float fTimeDelta) override;

public:
    void    Pause() override;
    void    Resume() override;
    void    StopImmediate() override;
    void    Jump(_float fTime) override;
    _bool   IsEnd() const override;
    SEQ_ID  GetId() const override { return m_Id; }

public:
    void Push_Obelisk(class CObelisk* pObelisk);

private:
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };

    _bool   m_isFadeIn = { false };    
    
    _bool   m_isEnd = { false };

    class CGameInstance* m_pGameInstance = { nullptr };

public:
    static CSequence_Viper_CutScene* Create();
    virtual void Free() override;
};

NS_END