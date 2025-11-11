//#include "Imp_Melee.h"
//#include "CharacterVirtual.h"
//#include "Body_Imp_Melee.h"
//#include "Imp_Sword.h"
//#include "AI_Controller_Imp_Melee.h"
//
//CImp_Melee::CImp_Melee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//    :CMonster{ pDevice,pContext }
//{
//}
//
//CImp_Melee::CImp_Melee(const CImp_Melee& Prototype)
//    :CMonster{ Prototype }
//{
//}
//
//_float4* CImp_Melee::Get_LockOnPosition()
//{
//    return nullptr;
//}
//
//HRESULT CImp_Melee::Initialize_Prototype()
//{
//    return S_OK;
//}
//
//HRESULT CImp_Melee::Initialize_Clone(void* pArg)
//{
//    if (FAILED(__super::Initialize_Clone(pArg)))
//        return E_FAIL;
//
//    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(513.f, -11.f, 225.f, 1.f));
//
//    if (FAILED(Ready_Components()))
//        return E_FAIL;
//
//    if (FAILED(Ready_PartObjects()))
//        return E_FAIL;
//
//
//    if (FAILED(Ready_AnimEvent()))
//        return E_FAIL;
//
//    m_pController = CAI_Controller_Imp_Melee::Create(this);
//    if (nullptr == m_pController)
//        return E_FAIL;
//
//
//    return S_OK;
//}
//
//void CImp_Melee::Priority_Update(_float fTimeDelta)
//{
//    CContainerObject::Priority_Update(fTimeDelta);
//}
//
//void CImp_Melee::Update(_float fTimeDelta)
//{
//    //m_pController->Update(this, fTimeDelta);
//
//    __super::Update(fTimeDelta);
//}
//
//void CImp_Melee::Late_Update(_float fTimeDelta)
//{
//    CContainerObject::Late_Update(fTimeDelta);
//}
//
//HRESULT CImp_Melee::Render()
//{
//    return S_OK;
//}
//
//void CImp_Melee::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
//{
//}
//
//void CImp_Melee::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal)
//{
//}
//
//void CImp_Melee::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer)
//{
//}
//
//HRESULT CImp_Melee::Ready_Components()
//{
//    CCharacterVirtual::CV_CAPSULESHAPE_DESC tCharVirDesc{};
//    _float3 vPos{};
//    _float4 vQuat{};
//
//    XMStoreFloat3(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
//    XMStoreFloat4(&vQuat, m_pTransformCom->Get_Rotation_Quat());
//    tCharVirDesc.eShapeType = SHAPE::CAPSULE;
//    tCharVirDesc.vPos = vPos;
//    tCharVirDesc.vQuat = vQuat;
//    tCharVirDesc.vShapeOffset = _float3(0.f, 2.1f, 0.f);
//    tCharVirDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::MONSTER);
//    tCharVirDesc.fRadius = 1.f;
//    tCharVirDesc.fHeight = 2.f;
//    tCharVirDesc.fMaxSlopeAngle = 45.f;
//
//    m_tCollisionDesc.pGameObject = this;
//    //pCollDesc.pInfo = ?? // 작성하기
//    tCharVirDesc.pCollisionDesc = &m_tCollisionDesc;
//
//    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_CharacterVirtual"),
//        TEXT("Com_CharacterVirtual"), reinterpret_cast<CComponent**>(&m_pCharVirCom), &tCharVirDesc)))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CImp_Melee::Ready_PartObjects()
//{
//    CBody_Imp_Melee::BODY_DESC BodyDesc{};
//    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//    BodyDesc.pOwnerTransform = m_pTransformCom;
//    BodyDesc.pOwner = this;
//
//    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Body"), &BodyDesc)))
//        return E_FAIL;
//
//    CPartObject* pBody = Find_PartObject(TEXT("Part_Body"));
//    if (nullptr == pBody)
//        return E_FAIL;
//
//    m_pBody = dynamic_cast<CBody_Imp_Melee*>(pBody);
//    Safe_AddRef(m_pBody);
//
//
//    CImp_Sword::WEAPON_DESC WeaponDesc{};
//    WeaponDesc.pOwner = this;
//    WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
//    WeaponDesc.pOwnerTransform = m_pTransformCom;
//    WeaponDesc.pSocketMatrix = m_pBody->Get_BoneMatrix_Ptr("Weapon_R");
//
//    if (FAILED(CContainerObject::Add_PartObject(TEXT("Part_Weapon"), ENUM_CLASS(LEVEL::HEINMACH), TEXT("Prototype_PartObject_Monster_Imp_Melee_Wand"), &WeaponDesc)))
//        return E_FAIL;
//
//    CPartObject* pWeapon = Find_PartObject(TEXT("Part_Weapon"));
//    if (nullptr == pWeapon)
//        return E_FAIL;
//
//    m_pWeapon = dynamic_cast<CImp_Sword*>(pWeapon);
//    if (nullptr == pWeapon)
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CImp_Melee::Ready_AnimEvent()
//{
//  /*  CModel* pModel = static_cast<CModel*>(m_pBody->Get_Component(TEXT("Com_Model")));
//    if (nullptr == pModel)
//        return E_FAIL;
//
//#pragma region MagicBall
//
//    pModel->Register_Event("CastSpell1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
//        Cast_MagicBall(0);
//        });
//    pModel->Register_Event("CastSpell1", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
//
//        });
//    pModel->Register_Event("CastSpell1", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
//
//        });
//
//    pModel->Register_Event("ShotSpell1", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
//        Shoot_MagicBall(0);
//        });
//
//
//    pModel->Register_Event("CastSpell2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
//        Cast_MagicBall(1);
//        });
//    pModel->Register_Event("CastSpell2", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
//
//        });
//    pModel->Register_Event("CastSpell2", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
//
//        });
//
//    pModel->Register_Event("ShotSpell2", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
//        Shoot_MagicBall(1);
//        });
//
//
//    pModel->Register_Event("CastSpell3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
//        Cast_MagicBall(2);
//        });
//    pModel->Register_Event("CastSpell3", ANIM_EVENT_TRIGGERTYPE::EXIT, [this]() {
//
//        });
//    pModel->Register_Event("CastSpell3", ANIM_EVENT_TRIGGERTYPE::CONTINUE, [this]() {
//
//        });
//
//    pModel->Register_Event("ShotSpell3", ANIM_EVENT_TRIGGERTYPE::ENTER, [this]() {
//        Shoot_MagicBall(2);
//        });
//
//
//
//
//#pragma endregion*/
//
//
//    return S_OK;
//}
//
//
//
//CImp_Melee* CImp_Melee::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//    CImp_Melee* pInstance = new CImp_Melee(pDevice, pContext);
//    if (FAILED(pInstance->Initialize_Prototype()))
//    {
//        Safe_Release(pInstance);
//        MSG_BOX(TEXT("Failed Create : CImp_Melee"));
//    }
//    return pInstance;
//}
//
//CGameObject* CImp_Melee::Clone(void* pArg)
//{
//    CImp_Melee* pInstance = new CImp_Melee(*this);
//    if (FAILED(pInstance->Initialize_Clone(pArg)))
//    {
//        Safe_Release(pInstance);
//        MSG_BOX(TEXT("Failed Clone : CImp_Melee"));
//    }
//    return pInstance;
//}
//
//void CImp_Melee::Free()
//{
//    Safe_Release(m_pBody);
//    Safe_Release(m_pWeapon);
//
//    __super::Free();
//}
