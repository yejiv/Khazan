#pragma once
#include "AI_Controller.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CAI_Controller_Viper final : public CAI_Controller
{
private:
    CAI_Controller_Viper();
    virtual ~CAI_Controller_Viper() = default;

    typedef struct tagSkillCoolDown
    {
        _float fMax = 0.f;
        _float fRemain = 0.f;
        _bool isReady = true;
        
        void Init(_float fMaxCoolDown)
        {
            fMax = fMaxCoolDown;
            fRemain = 0.f;
            isReady = true;
        }

        void Update(_float fTimeDelta)
        {
            if (!isReady)
            {
                fRemain -= fTimeDelta;
                if (fRemain <= 0.f)
                {
                    fRemain = 0.f;
                    isReady = true;
                }
            }
        }

        void Consume()
        {
            isReady = false;
            fRemain = fMax;
        }

        _float Get_Score() const { return isReady ? 1.f : 0.f; }

    }SKILLCOOLDOWN;

public:
    virtual void                Set_ControllerActivate(_bool isToggle) { m_isActiveController = isToggle; }
    void                        Set_CutSceneFinished() { m_is2PhaseCutSceneFinished = true; }
public:
    virtual HRESULT				Initialize(class CCreature* pOwner);
    virtual void				Update(CGameObject* pOwner, _float fTimeDelta) override;

    virtual HRESULT				Ready_Perception(class CGameObject* pOwner, const AIPERCEPTION_DATA& Desc) override;
    virtual HRESULT				Ready_BlackBoard(class CGameObject* pOwner) override;
    HRESULT						Ready_BehaviorTree(class CGameObject* pOwner) override;

    virtual CONDITION			GetCallbackCondition(class CGameObject* pOwner, const string& name) override;
    virtual ACTION				GetCallbackAction(CGameObject* pOwner, const string& name) override;
    virtual TERMINATE			GetCallbackTeminate(CGameObject* pOwner, const string& name) override;
    virtual INTERRUPTCONDITION	GetCallbackInterruptCondition(CGameObject* pOwner, const string& name) override;
    virtual PERCEPTIONCALLBACK	GetCallBackPerception(class CGameObject* pOwner, const string& name) override;

private:
    HRESULT                     Ready_CoolDown();
    void                        Set_ViperBerserkerStart_ShaderSettings();

private:
    _bool                                   m_isActiveController = { false };
    unordered_map<string, SKILLCOOLDOWN>    m_SkillCoolDowns;
    _float                                  m_fAnimSpeed = {};
    _float                                  m_fCurrentTime = {};
    _bool                                   m_is2PhaseCutSceneFinished = { false };

public:
    static CAI_Controller_Viper*    Create(class CCreature* pOwner);
    virtual void					Free() override;
};

NS_END