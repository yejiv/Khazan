#pragma once
#include "Client_Defines.h"
#include "Khazan_Spear_Animation.h"

NS_BEGIN(Client)

class CKhazan_GS_Anim_Damaged final : public CKhazan_Spear_Animation
{
private:
    CKhazan_GS_Anim_Damaged();
    virtual ~CKhazan_GS_Anim_Damaged() = default;

public:
    HRESULT			Initialize_Prototype();
    void            Enter() override;
    void            Continue(_float fTimeDelta) override;
    void            Exit() override;

    void            Force_DamagedNormal(_bool isWeapon, _uint iDir);
    void            Force_DamagedStrong(_bool isWeapon, _uint iDir);

    //void            Force_DamagedAir();
    void            Clear_Damaged() { m_isDamaged = false; }

    _bool           Is_Damaged() const { return m_isDamaged; }

private:
    _bool           m_isDamaged = { false };
    DIRECTION_INFO  m_eDir;

public:
    static CKhazan_GS_Anim_Damaged* Create();
    virtual void Free() override;



 

};


NS_END
