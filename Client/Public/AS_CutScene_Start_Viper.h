#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class  CUTSCENE_STATE { SIT = 77, LAND = 79, STAND = 80, END };

class CAS_CutScene_Start_Viper final : public CAI_State
{
private:
    CAS_CutScene_Start_Viper();
    virtual ~CAS_CutScene_Start_Viper() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


public:
    void                                ViperScene_Sit(class CViper* pViper);
    void                                ViperScene_Land(class CViper* pViper);
    void                                ViperScene_Roar(class CViper* pViper);
    


private:
    void                                Change_CutSceneState(CUTSCENE_STATE eNextState, class CModel* pModel, class CViper* pViper);


private:
    CUTSCENE_STATE                      m_eState = { CUTSCENE_STATE::END };

public:
    static CAS_CutScene_Start_Viper*    Create();
    virtual void				        Free() override;

};

NS_END