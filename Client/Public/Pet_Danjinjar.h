#pragma once
#include "Monster.h"
#include "Client_Defines.h"
#include "BlackBoard.h"

NS_BEGIN(Engine)
class CModel;
NS_END

NS_BEGIN(Client)

class CPet_Danjinjar final : public CMonster
{
public:
    enum class MONSTATE { IDLE, MOVE, DAMAGE,TP, END };

    typedef struct TagMonData {
        //Anim
        _int                iAnimIndex = {};
        _bool               isAnimFinash = { false };
        _bool               isBland = { false };
        _float              fQuat = {};

        _float              fDecreaseAlpha = {};
        _float              fEdgeWidth = {};
        _float4             fEdgeColor = {};

        //BT
        _bool               isTP = { false };
        _bool               isTPDanjin = { false };
        _bool               isDamage = { false };
        _bool               isIdle = { false };
        CPet_Danjinjar*     pOwner = { nullptr };
    }MONDATA;

private:
    CPet_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CPet_Danjinjar(const CPet_Danjinjar& Prototype);
    virtual ~CPet_Danjinjar() = default;

public:
    void                            isTalk(_bool isTalk, _int iIndex = -1);
    MONDATA& Get_Data();
    void                            LockOnLerp(_float fTimeDetla, _float fSpeed);
    void                            LockOn();
    void                            Player_TP();
    void                            LookAt_Lerp(_float fTimeDelta);
    _bool                           Check_Ranage(string strKey);
    _bool                           Check_Ranage(_float fRange);
    TARGET_DIR                      Get_DIR();

public:
    virtual HRESULT					Initialize_Prototype(_int iLevel);
    virtual HRESULT					Initialize_Clone(void* pArg) override;
    virtual void					Priority_Update(_float fTimeDelta) override;
    virtual void					Update(_float fTimeDelta) override;
    virtual void					Late_Update(_float fTimeDelta) override;
    virtual void				    Take_Damage(_float fDamage, HITREACTION eHitreaction, CGameObject* pGameObject = nullptr) override;

public:
    virtual void                    Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void                    Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void                    Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    class CBody_Pet_Danjinjar*      m_pBody = { nullptr };
    class CBlackBoard*              m_pBlackBoard = { nullptr };
    class CUI_Talk_Danjinjar*       m_pTalk = { nullptr };
    MONDATA                         m_Data = {};

    _float                          m_fTimeDelta = {};

private:
    HRESULT                         Ready_Prototype();

    HRESULT                         Ready_ETC();
    HRESULT							Ready_Components();
    HRESULT							Ready_PartObjects();
    HRESULT							Ready_AnimEvent();

    HRESULT							Ready_MonData();

    void                            State_Change(_bool isStart);
public:
    static CPet_Danjinjar*        Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iLevel);
    virtual CGameObject*            Clone(void* pArg) override;
    virtual void					Free() override;

};

NS_END