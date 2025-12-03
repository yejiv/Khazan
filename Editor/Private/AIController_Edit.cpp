//#include "AIController_Edit.h"
//#include "Perception.h"
//#include "BehaviorTree.h"
//#include "GameInstance.h"
//#include "GameObject.h"
//
//CAIController_Edit::CAIController_Edit()
//{
//}
//
//HRESULT CAIController_Edit::Initialize(CGameObject* pOwner)
//{
//    return S_OK;
//}
//
//void CAIController_Edit::Update(CGameObject* pOwner, _float fTimeDelta)
//{
//}
//
////HRESULT CAIController_Edit::Initialize(CGameObject* pOwner)
////{
////    string BasePath = "../../Client/Bin/Data/Monster/";
////
////    Load_Perception(BasePath +  "/Perception/" + "Yetuga_Perception.json");
////    Load_BlackBoard(pOwner,BasePath +"/BlackBoard/" + "Yetuga.json");
////    Load_BehaviorTree(BasePath + "/BehaviorTree/" + "Yetuga3.json");
////    return S_OK;
////}
////
////
////
////void CAIController_Edit::Update(CGameObject* pOwner, _float fTimeDelta)
////{
////    //m_pPerception->Update(pOwner, fTimeDelta);
////
////    m_pBT->Update();
////  /*  _bool isAttack = true;
////    isAttack = m_pBB->Get_Value<_bool>(m_strMonstertag, "isAttack");
////    _float fAttackRange = m_pBB->Get_Value<_float>(m_strMonstertag, "AttackRange");*/
////}
////
////HRESULT CAIController_Edit::Load_Perception(const string& FilePath)
////{
////    ifstream ifs(FilePath);
////    if (!ifs.is_open())
////        return E_FAIL;
////
////    JSON j;
////    ifs >> j;
////
////    for (auto& Data : j)
////    {
////        AIPERCEPTION_DATA Desc;
////        Desc.strMonsterType = Data["strMonsterType"].get<string>();
////        Desc.strFileName = Data["strFileName"].get<string>();
////        Desc.CallbackTags = Data["CallbackTags"].get<vector<string>>();
////        Desc.fRadius = Data["fRadius"].get<_float>();
////        Desc.fFov = Data["fFov"].get<_float>();
////        Desc.fLoseSightTime = Data["fLoseSightTime"].get<_float>();
////        Desc.fCheckInterval = Data["fCheckInterval"].get<_float>();
////        Desc.fFovCos = Data["fFovCos"].get<_float>();
////        Desc.isRequireLineOfSight = Data["isRequireLineOfSight"].get<_bool>();
////        Desc.fHeightOffset = Data["fHeightOffset"].get<_float>();
////
////
////        if (FAILED(Ready_Perception(Desc)))
////            return E_FAIL;
////    }
////    return S_OK;
////}
////
////HRESULT CAIController_Edit::Load_BlackBoard(class CGameObject* pOwner,const string& FilePath)
////{
////    if (FAILED(Ready_BlackBoard(pOwner)))
////        return E_FAIL;
////
////    ifstream ifs(FilePath);
////    if (!ifs.is_open())
////        return E_FAIL;
////
////    JSON j;
////    ifs >> j;
////
////    for (auto& Data : j)
////    {
////        AIBLACKBOARD_DATA Desc;
////        Desc.strMonsterType = Data["strMonsterType"].get<string>();
////
////        for (auto& [key, value] : Data["Values"].items())
////        {
////            if (value.is_boolean())
////                m_pBB->Set_Value(Desc.strMonsterType, key, value.get<_bool>());
////            else if (value.is_number_float())
////                m_pBB->Set_Value(Desc.strMonsterType, key, value.get<_float>());
////            else if (value.is_number_integer())
////                m_pBB->Set_Value(Desc.strMonsterType, key, static_cast<_uint>(value.get<int>()));
////            else if (value.is_array() && value.size() == 3)
////            {
////                _float3 vec = { value[0].get<_float>(), value[1].get<_float>(), value[2].get<_float>() };
////                m_pBB->Set_Value(Desc.strMonsterType, key, vec);
////            }
////            else if (value.is_string())
////            {
////                //m_pGameInstance->Find_GameObject(ENUM_CLASS());
////                //m_strMonstertag = value.get<string>();
////            }
////        }
////        m_strMonstertag = Desc.strMonsterType;
////    }
////
////    return S_OK;
////}
////
////HRESULT CAIController_Edit::Load_BehaviorTree(const string& FilePath)
////{
////    ifstream ifs(FilePath);
////    if (!ifs.is_open())
////        return E_FAIL;
////
////    JSON j;
////    ifs >> j;
////
////    AI_BTDATA BTDesc{};
////    BTDesc.MonsterType = j["MonsterType"].get<string>();
////
////    if (j.contains("RootNode"))
////    {
////        if (FAILED(LoadBTNode(j["RootNode"], BTDesc.RootNode)))
////            return E_FAIL;
////    }
////    else
////    {
////        if (j.contains("Root"))
////        {
////            if (FAILED(LoadBTNode(j["Root"], BTDesc.RootNode)))
////                return E_FAIL;
////        }
////        else
////            return E_FAIL;
////        
////    }
////
////    CBTNode* pRoot = CreateBTNode(BTDesc.RootNode);
////    if (pRoot == nullptr)
////        return E_FAIL;
////
////    m_BTDesc = BTDesc;
////
////    
////    m_pBT = CBehaviorTree::Create();
////    m_pBT->Set_Root(pRoot);
////    m_pBT->Set_BlackBoard(m_pBB);
////
////    return S_OK;
////}
////
////HRESULT CAIController_Edit::LoadBTNode(const JSON& j, AIBTNODE_DATA& Node)
////{
////    Node.strNodeName = j["NodeName"].get<string>();
////    Node.strNodeType = j["NodeType"].get<string>();
////    Node.strSubtype = j["SubType"].get<string>();
////
////    if (j.contains("CoolDownTime"))     
////        Node.fCoolDownTime = j["CoolDownTime"].get<_float>();
////    if (j.contains("iRepeatCount"))       
////        Node.iRepeatCount = j["RepeatCount"].get<_uint>();
////    if (j.contains("WaitTime"))          
////        Node.fWaitTime = j["WaitTime"].get<_float>();
////    if (j.contains("CallbackFunction")) 
////        Node.strCallbackFunction = j["CallbackFunction"].get<string>();
////
////    if (j.contains("Children"))
////    {
////        for (auto& childJson : j["Children"])
////        {
////            AIBTNODE_DATA childNode;
////            if (FAILED(LoadBTNode(childJson, childNode)))
////                return E_FAIL;
////
////            //Node.Children.push_back(childNode);
////        }
////    }
////
////    return S_OK;
////}
////
////CBTNode* CAIController_Edit::CreateBTNode(const AIBTNODE_DATA& NodeData)
////{
////    if (NodeData.strNodeType == "Leaf")
////    {
////        if (NodeData.strSubtype == "Wait")
////            return CWait_Node::Create(m_strMonstertag, NodeData.strNodeName, NodeData.fWaitTime);
////        else if (NodeData.strSubtype == "Condition")
////            return CCondition_Node::Create(GetCallbackCondition(NodeData.strCallbackFunction));
////        else if (NodeData.strSubtype == "Acition")
////            return CAction_Node::Create(GetCallbackAction(NodeData.strCallbackFunction)
////                ,GetCallbackTeminate(NodeData.strCallbackFunction));
////    }
////
////    else if (NodeData.strNodeType == "Decorator")
////    {
////        CDecorator_Node* pDecorator = { nullptr };
////
////        if (NodeData.strSubtype == "Inverter")
////            pDecorator = CInverter_Node::Create();
////        else if (NodeData.strSubtype == "Repeater")
////            pDecorator = CRepeater_Node::Create(NodeData.iRepeatCount);
////        else if (NodeData.strSubtype == "CoolDown")
////            pDecorator = CCoolDown_Node::Create(m_strMonstertag,NodeData.strNodeName,NodeData.fCoolDownTime);
////
////        if (!NodeData.Children.empty())
////            //pDecorator->Set_Child(CreateBTNode(NodeData.Children[0]));
////
////        return pDecorator;
////    }
////
////    else if (NodeData.strNodeType == "Composite")
////    {
////        CComposite_Node* pComposite = { nullptr };
////
////        if (NodeData.strSubtype == "Selector")
////            pComposite = CSelector_Node::Create();
////        else if (NodeData.strSubtype == "Sequence")
////            pComposite = CSequence_Node::Create();
////            
////        for (auto& pChild : NodeData.Children)
////            //pComposite->Add_Child(CreateBTNode(pChild));
////
////
////        return pComposite;
////    }
////
////    return nullptr;
////}
////
////HRESULT CAIController_Edit::Ready_Perception(const AIPERCEPTION_DATA& Desc)
////{
////    m_pPerception = CPerception::Create(Desc,0);
////    if (nullptr == m_pPerception)
////        return E_FAIL;
////
////    return S_OK;
////}
////
////HRESULT CAIController_Edit::Ready_BlackBoard(CGameObject* pOwner)
////{
////    m_pBB = m_pGameInstance->Get_BlackBoard();
////    if (nullptr == m_pBB)
////        return E_FAIL;
////
////    m_pBB->Set_Value(m_strMonstertag, "Owner", pOwner);
////
////    return S_OK;
////}
////
////HRESULT CAIController_Edit::Ready_BehaviorTree()
////{
////
////    return S_OK;
////}
////
////CONDITION CAIController_Edit::GetCallbackCondition(const string& name)
////{
////
////    return CONDITION();
////}
////
////ACTION CAIController_Edit::GetCallbackAction(const string& name)
////{
////
////    return ACTION();
////}
////
////TERMINATE CAIController_Edit::GetCallbackTeminate(const string& name)
////{
////    return TERMINATE();
////}
//
//
//
//CAIController_Edit* CAIController_Edit::Create(CGameObject* pOwner)
//{
//    CAIController_Edit* pInstance = new CAIController_Edit();
//    if (FAILED(pInstance->Initialize(pOwner)))
//    {
//        Safe_Release(pInstance);
//        MSG_BOX(TEXT("Failed Create: CAIController_Edit"));
//    }
//    return pInstance;
//}
//
//void CAIController_Edit::Free()
//{
//    __super::Free();
//}
