#pragma once
#include "Client_Defines.h"
#include "Sequence_Interface.h"

NS_BEGIN(Engine)
class CGameInstance;
class ISeqInstance;
NS_END

NS_BEGIN(Client)

class CSequence_HeinMach_Field : public ISeqInstance
{
private:
    CSequence_HeinMach_Field(class CCamera_Compre* pCamera);
    virtual ~CSequence_HeinMach_Field() = default;

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

private:
    enum class STATE { Idle, Playing, Paused, End };
    STATE  m_State{ STATE::Idle };
    SEQ_ID m_Id{};
    _float  m_fTime{ 0.f };

    _bool   m_isCameraStart = { false };
    _bool   m_isFieldName = { false };

    class CCamera_Compre* m_pCamera_Compre = { nullptr };
    class CGameInstance* m_pGameInstance = { nullptr };

public:
    static CSequence_HeinMach_Field* Create(class CCamera_Compre* pCamera);
    virtual void Free() override;
};

NS_END