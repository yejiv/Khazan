#include "Interaction_Item.h"
#include "GameInstance.h"
#include "Effect_Prefab.h"
#include "Interaction_Guide.h"
#include "Body.h"
#include "UI_Inven.h"
#include "ClientInstance.h"

CInteraction_Item::CInteraction_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteraction_Object{ pDevice, pContext }
{

}

CInteraction_Item::CInteraction_Item(const CInteraction_Item& Prototype)
    : CInteraction_Object{ Prototype }
{

}

HRESULT CInteraction_Item::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteraction_Item::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Effect(pArg)))
        return E_FAIL;  

    if (FAILED(Ready_Guide(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(100000.f, 100000.f, 100000.f, 0.f));

    if (FAILED(Ready_Collision(pArg)))
        return E_FAIL;

    m_strName = "Interaction_Item";

    m_NormalItemIndex.reserve(11);


    // Normal Item
    m_NormalItemIndex.push_back(1001);
    m_NormalItemIndex.push_back(1002);
    m_NormalItemIndex.push_back(1003);
    m_NormalItemIndex.push_back(1004);
    m_NormalItemIndex.push_back(3001);
    m_NormalItemIndex.push_back(3002);
    m_NormalItemIndex.push_back(3003);
    m_NormalItemIndex.push_back(3004);
    m_NormalItemIndex.push_back(3005);
    m_NormalItemIndex.push_back(3006);
    m_NormalItemIndex.push_back(3007);

    // Special Item
    m_SpecialItemIndex.emplace(TEXT("Precept"), 2001); // 호송 명령서
    m_SpecialItemIndex.emplace(TEXT("Record"), 2002); // 어느 병사의 기록
    m_SpecialItemIndex.emplace(TEXT("Handwriting"), 2003); // 도굴꾼의 수기
    m_SpecialItemIndex.emplace(TEXT("Report"), 2004); // 용족에 대한 보고서    

    return S_OK;
}

void CInteraction_Item::Priority_Update(_float fTimeDelta)
{
    m_pEffect->Priority_Update(fTimeDelta);
}

void CInteraction_Item::Update(_float fTimeDelta)
{
    m_pEffect->Update(fTimeDelta);
    Item_Check();
}

void CInteraction_Item::Late_Update(_float fTimeDelta)
{
   m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CInteraction_Item::Render()
{

    return S_OK;
}

void CInteraction_Item::Ready_Item(_uint iItemIndex, _vector vPos)
{
    m_iItemIndex = iItemIndex;
    m_pTransformCom->Set_State(STATE::POSITION, vPos);
    m_pBodyCom->Set_Pos(m_pTransformCom->Get_State(STATE::POSITION));    
    m_pEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION));

    m_isShow = true;
}

void CInteraction_Item::RandNormal_Item(_vector vPos)
{
    _int iRand = m_pGameInstance->Rand(0, 10);
    m_iItemIndex = m_NormalItemIndex[iRand];
    m_pTransformCom->Set_State(STATE::POSITION, vPos);
    m_pBodyCom->Set_Pos(m_pTransformCom->Get_State(STATE::POSITION));
    m_pEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION));

    m_isShow = true;
}

void CInteraction_Item::Special_Item(_wstring strNameTag, _vector vPos)
{
    auto iter = m_SpecialItemIndex.find(strNameTag);

    if (iter == m_SpecialItemIndex.end())
        return;

    m_pTransformCom->Set_State(STATE::POSITION, vPos);
    m_pBodyCom->Set_Pos(m_pTransformCom->Get_State(STATE::POSITION));
    m_pEffect->UpdatePosition(m_pTransformCom->Get_State(STATE::POSITION));

    m_isShow = true;
}

void CInteraction_Item::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (m_isShow)
    {
        if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
        {
            m_pGuide->Update_Visible(true);
            m_isGuideVisible = true;
        }
    }
    
}

void CInteraction_Item::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CInteraction_Item::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (m_isShow)
    {
        if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
        {
            m_pGuide->Update_Visible(false);
            m_isGuideVisible = false;
        }
    }
    
}

HRESULT CInteraction_Item::Ready_Components(void* pArg)
{

    return S_OK;
}

HRESULT CInteraction_Item::Bind_ShaderResources(void* pArg)
{

    return S_OK;
}

HRESULT CInteraction_Item::Ready_Effect(void* pArg)
{
    m_pEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevelIndex, TEXT("ITEM_FX")));

    if (m_pEffect)
        m_pEffect->ResetChildren();
    return S_OK;
}

HRESULT CInteraction_Item::Ready_Guide(void* pArg)
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::DEFAULT, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, 10.f), TEXT("획득"), 1.f);

    m_pGameInstance->Push_PoolObject_ToLayer(m_iLevelIndex, TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CInteraction_Item::Ready_Collision(void* pArg)
{
    CBody::BODY_BOXSHAPE_DESC BodyDesc{};
    BodyDesc.vExtent = _float3(0.5f, 0.5f, 0.5f);
    BodyDesc.bIsTrigger = true;
    BodyDesc.bStartActive = true;
    BodyDesc.eMotion = EMotionType::Kinematic;
    BodyDesc.eQuality = EMotionQuality::Discrete;
    BodyDesc.eShapeType = SHAPE::BOX;
    BodyDesc.fFriction = 0.8f;
    BodyDesc.fMass = 1.0f;
    BodyDesc.fRestitution = 0.0f;
    BodyDesc.iObjectLayer = ENUM_CLASS(COLLISION_LAYER::INTERACT_OBJECT);

    XMStoreFloat3(&BodyDesc.vPos, m_pTransformCom->Get_State(STATE::POSITION) - XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK)) * 1.f);
    BodyDesc.vPos.y += BodyDesc.vExtent.y;

    XMStoreFloat4(&BodyDesc.vQuat, m_pTransformCom->Get_Rotation_Quat());

    BodyDesc.vShapeOffset = _float3(0.f, 0.f, 0.f);
    m_tCollisionDesc.pGameObject = this;
    m_tCollisionDesc.isForceVaildation = true;
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;

    return true;
}

void CInteraction_Item::Item_Check()
{
    if (m_isGuideVisible && m_pGameInstance->Key_Down(DIK_F))
    {
        if (CClientInstance::GetInstance()->Item_Exist_ID(m_iItemIndex))
        {
            static_cast<CUI_Inven*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Inven")))->Add_Item(m_iItemIndex);

            m_isDead = true;
            m_pGuide->Set_IsDead(true);
            m_pGuide = nullptr;

            Safe_Release(m_pEffect);
            m_pEffect = nullptr;

            Safe_Release(m_pBodyCom);
            Remove_Component(TEXT("Com_Body"));

            m_isShow = false;
        }
    }
}

void CInteraction_Item::Reset()
{
    if (m_pGuide == nullptr) {

        Ready_Guide(nullptr);
    }

    if (m_pEffect == nullptr)
    {
        Ready_Effect(nullptr);
    }

    if (m_pBodyCom == nullptr)
    {
        Ready_Collision(nullptr);
    }
}

CInteraction_Item* CInteraction_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteraction_Item* pInstance = new CInteraction_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CInteraction_Item"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInteraction_Item::Clone(void* pArg)
{
    CInteraction_Item* pInstance = new CInteraction_Item(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CInteraction_Item"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteraction_Item::Free()
{
    __super::Free();
    if (m_pGuide)
    {
        //m_pGuide->Set_IsDead(true);
        m_pGuide = nullptr;
    }

    Safe_Release(m_pEffect);

}
