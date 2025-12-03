#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

enum class  CUTSCENE_STATE { RUN, JUMP = 52, LAND = 54, ROAR1 = 23, ROAR2 = 22, END };

class CAS_CutScene_Yetuga final : public CAI_State
{
private:
    CAS_CutScene_Yetuga();
    virtual ~CAS_CutScene_Yetuga() = default;

public:
    virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
    virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
    virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;


public:
    void                                YetugaScene_Jump(class CYetuga* pYetuga);
    void                                YetugaScene_Land(class CYetuga* pYetuga);
    void                                YetugaScene_Roar1(class CYetuga* pYetuga);
    void                                YetugaScene_Roar2(class CYetuga* pYetuga);


private:
    void                                Change_CutSceneState(CUTSCENE_STATE eNextState, class CModel* pModel, class CYetuga* pViper);


private:
    CUTSCENE_STATE                      m_eState = { CUTSCENE_STATE::END };

public:
    static CAS_CutScene_Yetuga*         Create();
    virtual void				        Free() override;

};

NS_END