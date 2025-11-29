#pragma once
#include "Client_Defines.h"
#include "Khazan_Spear_Animation.h"

NS_BEGIN(Client)

class CKhazan_GS_Anim_Interaction final:  public CKhazan_Spear_Animation
{

private:
    CKhazan_GS_Anim_Interaction();
    virtual ~CKhazan_GS_Anim_Interaction() = default;


public:
    HRESULT			Initialize_Prototype();

public:
    void Enter() override;
    void Continue(_float fTimeDelta) override;
    void Exit() override;

public:
    _bool            Try_Lantern(_bool isEquip);
    _bool            Try_TobStone(_bool isWeapon);
    _bool            Try_DamagedTS_Before(_bool isWeapon);
    _bool            Try_DamagedTS_After(_bool isWeapon);
    _bool            Try_BoxOpen(_bool isUsedSet);
    _bool            Try_Lachryma();
    _bool            Try_Heal();
    _bool            Try_Lever(_bool isUsedSet);
    _bool            Try_Statue(_bool isUsedSet);
    _bool            Try_IronGate(_bool isUsedSet);
    _bool            Try_UnLockGear(_bool isUsedSet);
    _bool            Try_GiantGate(_bool isUsedSet);

public:
    _bool           Is_Interaction() const { return m_isInteraction; }
    

private:
    class CClientInstance*  m_pClientInstance = { nullptr };
    PLAYER_DATA*            m_pPlayerData = { nullptr };


    _bool           m_isInteraction = { false };
    //_bool           m_isReserve = { false };

    


public:
    static CKhazan_GS_Anim_Interaction* Create();
    virtual void Free() override;
};

NS_END