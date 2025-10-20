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

    CSelector_Node* pRootNode = CSelector_Node::Create();
    Set_Root(pRootNode);

#pragma region ATTACK

    
#pragma endregion

#pragma region CHASE

    CSequence_Node* pMoveSequence = CSequence_Node::Create();

    CAction_Node* pMoveCondition = CAction_Node::Create([pOwner](CBlackBoard* BB) {
        if (BB->Get_Value<_bool>("Yetuga", "isDead")) return BTNODESTATE::FAILURE;
        if (!BB->Get_Value<_bool>("Yetuga", "isDetected")) return BTNODESTATE::FAILURE;

        _float fDist = BB->Get_Value<_float>("Yetuga", "TargetDist");
        cout << "TargetDist : "<< fDist << endl;

        // 추적 가능 거리
        if (fDist != 0 && fDist <= BB->Get_Value<_float>("Yetuga", "ChaseRange"))
        {
            cout << "MoveCondition Success" << endl;
            return BTNODESTATE::SUCCESS;
        }

        return BTNODESTATE::FAILURE;

        }, nullptr

    );

    CAction_Node* pMoveAction = CAction_Node::Create([pOwner](CBlackBoard* BB) {

        cout << "MoveAction!!!!!" << endl;
        pOwner->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::MOVE));
        return BTNODESTATE::RUNNING;

        },
        // TERMINATE
        [pOwner](CBlackBoard* BB, BTNODESTATE eState) {

            if (BTNODESTATE::SUCCESS == eState || BTNODESTATE::FAILURE == eState)
                // 이동 종료 처리
            {
                _int a = 10;
            }
        }
    );

    pMoveSequence->Add_Child(pMoveCondition);
    pMoveSequence->Add_Child(pMoveAction);

#pragma endregion

#pragma region IDLE

    CAction_Node* pIdleAction = CAction_Node::Create([pOwner](CBlackBoard* BB) {
        if (BB->Get_Value<_bool>("Yetuga", "isDead")) return BTNODESTATE::FAILURE;

        pOwner->Get_Controller()->Get_State_Machine()->Change_State(ENUM_CLASS(YETUGA_STATE::IDLE));
        return BTNODESTATE::RUNNING;

        }, [pOwner](CBlackBoard* BB, BTNODESTATE eState) {


            }
        );

#pragma endregion

    pRootNode->Add_Child(pMoveSequence);
    pRootNode->Add_Child(pIdleAction);

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
