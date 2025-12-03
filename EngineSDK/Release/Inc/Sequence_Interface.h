#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL ISeqInstance : public CBase
{
public:
    virtual HRESULT Initialize(const SEQ_REQ_PLAY_DESC& d) = 0;
    virtual void    Update(_float dt) = 0;
    virtual void    Pause() = 0;
    virtual void    Resume() = 0;
    virtual void    StopImmediate() = 0;
    virtual void    Jump(_float t) = 0;
    virtual _bool   IsEnd() const = 0;
    virtual SEQ_ID  GetId() const = 0;
};

// 클라이언트가 제공하는 팩토리 (자산/ID에 따라 ISeqInstance 생성)
class ENGINE_DLL ISeqFactory
{
public:
    virtual ~ISeqFactory() = default;
    virtual ISeqInstance* Create(const SEQ_REQ_PLAY_DESC& d) = 0;
};


NS_END