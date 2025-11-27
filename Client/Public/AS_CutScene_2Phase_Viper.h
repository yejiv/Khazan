#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class  P2CUTSCENE_STATE { DOWN = 73, PICKUP = 74, INJURE = 75, HEMPTYSIS = 76, CONGESTION = 4, HANDDN = 5, END };

class CAS_CutScene_2Phase_Viper final : public CAI_State
{
private:
    CAS_CutScene_2Phase_Viper();
    virtual ~CAS_CutScene_2Phase_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


private:
    void                                Change_CutSceneState(P2CUTSCENE_STATE eNextState, class CModel* pModel, class CViper* pViper);


private:
    P2CUTSCENE_STATE                    m_eState = { P2CUTSCENE_STATE::END };

public:
    static CAS_CutScene_2Phase_Viper*   Create();
    virtual void				        Free() override;

};

NS_END