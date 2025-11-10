#include "AI_Controller.h"
#include "BlackBoard.h"
#include "BehaviorTree.h"
#include "StateMachine.h"
#include "Perception.h"
#include "GameObject.h"
#include "GameInstance.h"

CAI_Controller::CAI_Controller()
    :m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

void CAI_Controller::AI_MoveTo(CGameObject* pOwner, CGameObject* pTarget, _float fLimit, _float fSpeedPerSec , _float fTimeDelta)
{
    CTransform* pTargetTransform = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
    CTransform* pOwnerTransform = static_cast<CTransform*>(pOwner->Get_Component(TEXT("Com_Transform")));
    _vector vTargetPos = pTargetTransform->Get_State(STATE::POSITION);

    pOwnerTransform->LookAt(vTargetPos);
    pOwnerTransform->AI_Chase(vTargetPos, fTimeDelta, fSpeedPerSec ,fLimit);
}

void CAI_Controller::AI_ApplyDamage(CGameObject* pAttacker, _float fDamage, _uint iHitreaction, _float fValidTime)
{
    if (nullptr == m_pPerception)
        return;
    
    STIMULUS DamageStim{};
    DamageStim.bSensed = true;
    DamageStim.eType = SENSETYPE::DAMAGE;
    DamageStim.fStrength = fDamage;
    DamageStim.fTimeStamp = m_pPerception->Get_CurrentTime();
    _vector vTempLocation = static_cast<CTransform*>(pAttacker->Get_Component(TEXT("Com_Transform")))->Get_State(STATE::POSITION);
    XMStoreFloat3(&DamageStim.vLocation, vTempLocation);
    DamageStim.iDamageType = iHitreaction;
    DamageStim.fVaildTime = fValidTime;

    m_pPerception->Notify_Damage(pAttacker,DamageStim);
    //m_pBT->Notify_Event(BTEVENT::HIT);

}

void CAI_Controller::AI_React_Collision(COLLISION_DESC* pDesc, _uint iCollisionLayer ,CGameObject* pOwner)
{
    if (nullptr == m_pFSM)
        return;

    m_pFSM->OnCollision(pDesc, iCollisionLayer ,pOwner);

}

HRESULT CAI_Controller::Initialize(CGameObject* pOwner, string FileName)
{
    string BasePath = "../../Client/Bin/Data/Monster/";

    Load_Perception(pOwner,BasePath + "Perception/" + FileName + "/" + FileName + ".json");
    Load_BlackBoard(pOwner, BasePath + "BlackBoard/" + "/" + FileName + "/" + FileName + ".json");
    Load_BehaviorTree(pOwner,BasePath + "BehaviorTree/" + FileName + "/" + FileName + ".json");

    return S_OK;
}

void CAI_Controller::Update(class CGameObject* pOwner, _float fTimeDelta)
{

}

HRESULT CAI_Controller::Load_Perception(CGameObject* pOwner, const string& FilePath)
{
    ifstream ifs(FilePath);
    if (!ifs.is_open())
        return E_FAIL;

    JSON j;
    ifs >> j;

    for (auto& Data : j)
    {
        AIPERCEPTION_DATA Desc;
        Desc.strMonsterType = Data["strMonsterType"].get<string>();
        Desc.strFileName = Data["strFileName"].get<string>();
        Desc.CallbackTags = Data["CallbackTags"].get<vector<string>>();
        Desc.fRadius = Data["fRadius"].get<_float>();
        Desc.fFov = Data["fFov"].get<_float>();
        Desc.fLoseSightTime = Data["fLoseSightTime"].get<_float>();
        Desc.fCheckInterval = Data["fCheckInterval"].get<_float>();
        Desc.fFovCos = Data["fFovCos"].get<_float>();
        Desc.isRequireLineOfSight = Data["isRequireLineOfSight"].get<_bool>();
        Desc.fHeightOffset = Data["fHeightOffset"].get<_float>();

    
        if (FAILED(Ready_Perception(pOwner,Desc)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CAI_Controller::Load_BlackBoard(CGameObject* pOwner, const string& FilePath)
{
    if (FAILED(Ready_BlackBoard(pOwner)))
        return E_FAIL;

    ifstream ifs(FilePath);
    if (!ifs.is_open())
        return E_FAIL;

    JSON j;
    ifs >> j;

    for (auto& Data : j)
    {
        AIBLACKBOARD_DATA Desc;
        Desc.strMonsterType = Data["strMonsterType"].get<string>();

        for (auto& [key, value] : Data["Values"].items())
        {
            if (value.is_boolean())
                m_pBB->Set_Value(Desc.strMonsterType, key, value.get<_bool>());
            else if (value.is_number_float())
                m_pBB->Set_Value(Desc.strMonsterType, key, value.get<_float>());
            else if (value.is_number_integer())
                m_pBB->Set_Value(Desc.strMonsterType, key, static_cast<_uint>(value.get<int>()));
            else if (value.is_array() && value.size() == 3)
            {
                _float3 vec = { value[0].get<_float>(), value[1].get<_float>(), value[2].get<_float>() };
                m_pBB->Set_Value(Desc.strMonsterType, key, vec);
            }
            else if (value.is_string())
            {
                //m_pGameInstance->Find_GameObject(ENUM_CLASS());
                //m_strMonstertag = value.get<string>();
            }
        }
        m_strMonstertag = Desc.strMonsterType;
    }

    return S_OK;
}

HRESULT CAI_Controller::Load_BehaviorTree(CGameObject* pOwner,const string& FilePath)
{
    ifstream ifs(FilePath);
    if (!ifs.is_open())
        return E_FAIL;

    JSON j;
    ifs >> j;

    AI_BTDATA BTDesc{};
    BTDesc.MonsterType = j["MonsterType"].get<string>();

    if (j.contains("RootNode"))
    {
        if (FAILED(LoadBTNode(j["RootNode"], BTDesc.RootNode)))
            return E_FAIL;
    }
    else
    {
        if (j.contains("Root"))
        {
            if (FAILED(LoadBTNode(j["Root"], BTDesc.RootNode)))
                return E_FAIL;
        }
        else
            return E_FAIL;

    }

    CBTNode* pRoot = CreateBTNode(pOwner,BTDesc.RootNode);
    if (pRoot == nullptr)
        return E_FAIL;

    m_BTDesc = BTDesc;


    m_pBT = CBehaviorTree::Create();
    m_pBT->Set_Root(pRoot);
    m_pBT->Set_BlackBoard(m_pBB);

    return S_OK;
}

HRESULT CAI_Controller::LoadBTNode(const JSON& j, AIBTNODE_DATA& Node)
{
    Node.strNodeName = j["NodeName"].get<string>();
    Node.strNodeType = j["NodeType"].get<string>();
    Node.strSubtype = j["SubType"].get<string>();

    if (j.contains("CoolDownTime"))
        Node.fCoolDownTime = j["CoolDownTime"].get<_float>();
    if (j.contains("iRepeatCount"))
        Node.iRepeatCount = j["RepeatCount"].get<_uint>();
    if (j.contains("WaitTime"))
        Node.fWaitTime = j["WaitTime"].get<_float>();
    if (j.contains("Callback"))
        Node.strCallbackFunction = j["Callback"].get<string>();


    if (j.contains("Children"))
    {
        for (auto& childJson : j["Children"])
        {
            AIBTNODE_DATA* pChild = new AIBTNODE_DATA();
            if (FAILED(LoadBTNode(childJson, *pChild)))
            {
                delete pChild;
                return E_FAIL;
            }
             //부모 포인터 설정
            pChild->Parent = &Node;
            Node.Children.push_back(pChild);
        }
    }



    return S_OK;
}

CBTNode* CAI_Controller::CreateBTNode(CGameObject* pOwner, const AIBTNODE_DATA& NodeData)
{
    if (NodeData.strNodeType == "Leaf")
    {
        if (NodeData.strSubtype == "Wait")
            return CWait_Node::Create(m_strMonstertag, "CurrentTime", NodeData.fWaitTime);
        else if (NodeData.strSubtype == "Condition")
            return CCondition_Node::Create(GetCallbackCondition(pOwner,NodeData.strCallbackFunction));
        else if (NodeData.strSubtype == "Action")
            return CAction_Node::Create(GetCallbackAction(pOwner,NodeData.strCallbackFunction)
                , GetCallbackTeminate(pOwner,NodeData.strCallbackFunction));
    }

    else if (NodeData.strNodeType == "Decorator")
    {
        CDecorator_Node* pDecorator = { nullptr };

        if (NodeData.strSubtype == "Inverter")
            pDecorator = CInverter_Node::Create();
        else if (NodeData.strSubtype == "Repeater")
            pDecorator = CRepeater_Node::Create(NodeData.iRepeatCount);
        else if (NodeData.strSubtype == "CoolDown")
            pDecorator = CCoolDown_Node::Create(m_strMonstertag,"CurrentTime", NodeData.fCoolDownTime);

        if (!NodeData.Children.empty())
            pDecorator->Set_Child(CreateBTNode(pOwner,*NodeData.Children[0]));

        return pDecorator;
    }

    else if (NodeData.strNodeType == "Composite")
    {
        CComposite_Node* pComposite = { nullptr };

        if (NodeData.strSubtype == "Selector")
            pComposite = CSelector_Node::Create();
        else if (NodeData.strSubtype == "Sequence")
            pComposite = CSequence_Node::Create();

        else if (NodeData.strSubtype == "InterruptibleSelector")
            pComposite = CInterruptibleSelector_Node::Create(GetCallbackInterruptCondition(pOwner,NodeData.strCallbackFunction));

        for (auto& pChild : NodeData.Children)
            pComposite->Add_Child(CreateBTNode(pOwner,*pChild));


        return pComposite;
    }

    return nullptr;
}

HRESULT CAI_Controller::Ready_Perception(CGameObject* pOwner, const AIPERCEPTION_DATA& Desc)
{
    return S_OK;
}

HRESULT CAI_Controller::Ready_BlackBoard(CGameObject* pOwner)
{
    return S_OK;
}

HRESULT CAI_Controller::Ready_BehaviorTree(CGameObject* pOwner)
{
    return S_OK;
}


PERCEPTIONCALLBACK CAI_Controller::GetCallBackPerception(CGameObject* pOwner, const string& name)
{
    return PERCEPTIONCALLBACK();
}



CONDITION CAI_Controller::GetCallbackCondition(CGameObject* pOwner,const string& name)
{

    return CONDITION();
}

ACTION CAI_Controller::GetCallbackAction(CGameObject* pOwner, const string& name)
{

    return ACTION();
}

TERMINATE CAI_Controller::GetCallbackTeminate(CGameObject* pOwner, const string& name)
{
    return TERMINATE();
}

INTERRUPTCONDITION CAI_Controller::GetCallbackInterruptCondition(CGameObject* pOwner, const string& name)
{
    return INTERRUPTCONDITION();
}


void CAI_Controller::Release_BTNode(AIBTNODE_DATA* pRootNodeData)
{
    if (nullptr == pRootNodeData)
        return;

    for (auto& pChild : pRootNodeData->Children)
    {
        Release_BTNode(pChild);
        delete pChild;
    }
    pRootNodeData->Children.clear();
}

void CAI_Controller::Free()
{
    __super::Free();

  
    Release_BTNode(&m_BTDesc.RootNode);

    Safe_Release(m_pBB);
    Safe_Release(m_pBT);
    Safe_Release(m_pFSM);
    Safe_Release(m_pPerception);
    Safe_Release(m_pGameInstance);

}
