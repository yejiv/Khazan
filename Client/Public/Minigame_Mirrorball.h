#pragma once
#include "Base.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CMinigame_Mirrorball final : public CBase
{
private:
    enum class COLORSTATE { RED, GREEN, BLUE, END};
public:
    CMinigame_Mirrorball();
    virtual ~CMinigame_Mirrorball() = default;

public:
    void                            Start_Mirrorball();
    void                            End_Mirrorball();
    void                            Update(_float fTimeDelta);
    
private:
    CGameInstance*                  m_pGameInstance = { nullptr };
    _bool                           m_isStart = { false };
    COLORSTATE                      m_eState = { COLORSTATE::END };
private:
    void           Light_Red();
    void           Light_Blue();
    void           Light_Green();
public:
    static CMinigame_Mirrorball*    Create();
    virtual void                    Free() override;
};
NS_END

