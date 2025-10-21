#include "BT_Yetuga.h"
#include "Yetuga.h"
#include "Selector_Node.h"
#include "Sequence_Node.h"
#include "Action_Node.h"
#include "BlackBoard.h"
#include "AI_Controller.h"
#include "FSM_Yetuga.h"

CBT_Yetuga::CBT_Yetuga()
{

}

HRESULT CBT_Yetuga::Initialize(void* pArg)
{
    CYetuga* pOwner = static_cast<CYetuga*>(pArg);
    if (nullptr == pOwner)
        return E_FAIL;

    // 루트 셀렉터 노드 (최상위)
    CSelector_Node* pRoot = CSelector_Node::Create();
    Set_Root(pRoot);
    
#pragma region ATTACK

    CSequence_Node* pAttackSequence = CSequence_Node::Create();

    CAction_Node* pAttackCondition = CAction_Node::Create(
        [pOwner](CBlackBoard* BB)
        {
            cout << "Attack Conditon " << endl;

            if (BB->Get_Value<_bool>("Yetuga", "isDead")) return BTNODESTATE::FAILURE;

            _float fDist = BB->Get_Value<_float>("Yetuga", "TargetDist");
            if (fDist != 0 && pOwner->AI_IsReadyCoolDown("Attack") && fDist < BB->Get_Value<_float>("Yetuga", "AttackRange"))
            {
                cout << "AttackConditionSuccess" << endl;
                return BTNODESTATE::SUCCESS;
            }
            else
            {
                return BTNODESTATE::FAILURE;
            }

        }, nullptr);

    CAction_Node* pAttackAction = CAction_Node::Create(
        [pOwner](CBlackBoard* BB)
        {
            if (!BB->Get_Value<_bool>("Yetuga", "isAttack"))
            {
                BB->Set_Value<_bool>("Yetuga", "isAttack", true);
                BB->Set_Value<_bool>("Yetuga", "isAttackFinished", false);

                pOwner->AI_Set_CoolDown("Attack", BB->Get_Value<_float>("Yetuga", "AttackCoolDown"));
                pOwner->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::ATTACK), pOwner);

                return BTNODESTATE::RUNNING;

            }
            if (BB->Get_Value<_bool>("Yetuga", "isAttackFinished"))
            {
                return BTNODESTATE::SUCCESS;
            }
            // 아직 애니메이션 진행 중
            return BTNODESTATE::RUNNING;
        },
        [pOwner](CBlackBoard* BB, BTNODESTATE eState)
        {
            if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
            {
                BB->Set_Value<_bool>("Yetuga", "isAttack", false);
                BB->Set_Value<_bool>("Yetuga", "isAttackFinished", false);

                pOwner->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE), pOwner);
            }
        }
    );


    pAttackSequence->Add_Child(pAttackCondition);
    pAttackSequence->Add_Child(pAttackAction);

#pragma endregion


#pragma region CHASE

    CSequence_Node* pChaseSequence = CSequence_Node::Create();

    // 감지된 상태 && 거리 범위 내
    CAction_Node* pMoveCondition = CAction_Node::Create(
        [pOwner](CBlackBoard* BB)
        {
            if (BB->Get_Value<_bool>("Yetuga", "isDead")) return BTNODESTATE::FAILURE;
            cout << "Chase Condition" << endl;
            if (!BB->Get_Value<_bool>("Yetuga", "isDetected")) return BTNODESTATE::FAILURE;

            _float fDist = BB->Get_Value<_float>("Yetuga", "TargetDist");

            if (fDist >= BB->Get_Value<_float>("Yetuga","AttackRange") 
                && fDist != 0 && fDist <= BB->Get_Value<_float>("Yetuga", "ChaseRange"))
            {
                return BTNODESTATE::SUCCESS;
            }
            return BTNODESTATE::FAILURE;
        },
        nullptr
    ); 

    // MOVE 상태 진입
    CAction_Node* pMoveAction = CAction_Node::Create(
        [pOwner](CBlackBoard* BB)
        {
            if (BB->Get_Value<_float>("Yetuga", "TargetDist") <= BB->Get_Value<_float>("Yetuga", "AttackRange"))
                return BTNODESTATE::SUCCESS;

            pOwner->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::MOVE),pOwner);
            return BTNODESTATE::RUNNING;
        },
        [pOwner](CBlackBoard* BB, BTNODESTATE eState)
        {
            if (eState == BTNODESTATE::SUCCESS || eState == BTNODESTATE::FAILURE)
            {
                cout << " MoveAction Terminated" << endl;
            }
        }
    );

    pChaseSequence->Add_Child(pMoveCondition);
    pChaseSequence->Add_Child(pMoveAction);

#pragma endregion

#pragma region IDLE 

    CAction_Node * pIdleAction = CAction_Node::Create(
        [pOwner](CBlackBoard* BB)
        {
            if (BB->Get_Value<_bool>("Yetuga", "isDead")) return BTNODESTATE::FAILURE;
            if (BB->Get_Value<_bool>("Yetuga", "isDetected")) return BTNODESTATE::FAILURE;

            pOwner->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE),pOwner);
            return BTNODESTATE::RUNNING;
        },
        nullptr
    );
#pragma endregion

    // 루트에 추가
    pRoot->Add_Child(pAttackSequence);
    pRoot->Add_Child(pChaseSequence);
    pRoot->Add_Child(pIdleAction);

    return S_OK;
}


CBT_Yetuga* CBT_Yetuga::Create(void* pArg)
{
    CBT_Yetuga* pInstance = new CBT_Yetuga();
    if (FAILED(pInstance->Initialize(pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CBT_Yetuga"));
    }

    return pInstance;
}

void CBT_Yetuga::Free()
{
    __super::Free();
}
