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

    //if (FAILED(Ready_Effect()))
    //    return E_FAIL;

    if (FAILED(Ready_Guide()))
        return E_FAIL;

    if (FAILED(Ready_Collision()))
        return E_FAIL;

    m_strName = "Interaction_Item";

    return S_OK;
}

void CInteraction_Item::Priority_Update(_float fTimeDelta)
{
    //m_pEffect->Priority_Update(fTimeDelta);
}

void CInteraction_Item::Update(_float fTimeDelta)
{
    //m_pEffect->Update(fTimeDelta);
    Item_Check();
}

void CInteraction_Item::Late_Update(_float fTimeDelta)
{
   //m_pEffect->Late_Update(fTimeDelta);
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
}

void CInteraction_Item::Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
    {
        m_pGuide->Update_Visible(true);
        m_isGuideVisible = true;
    }
}

void CInteraction_Item::Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc)
{
}

void CInteraction_Item::Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc)
{
    if (iOtherObjectLayer == ENUM_CLASS(COLLISION_LAYER::PLAYER))
    {
        m_pGuide->Update_Visible(false);
        m_isGuideVisible = false;
    }
}

HRESULT CInteraction_Item::Ready_Components()
{

    return S_OK;
}

HRESULT CInteraction_Item::Bind_ShaderResources()
{

    return S_OK;
}

HRESULT CInteraction_Item::Ready_Effect()
{
    m_pEffect = dynamic_cast<CEffect_Prefab*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::HEINMACH), TEXT("ITEM_FX")));

    if (m_pEffect)
        m_pEffect->ResetChildren();
    return S_OK;
}

HRESULT CInteraction_Item::Ready_Guide()
{
    m_pGuide = static_cast<CInteraction_Guide*>(m_pGameInstance->Pop_PoolObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Pool_Key_Guide")));
    CHECK_NULLPTR(m_pGuide, E_FAIL);

    m_pGuide->Setting_Guide(CInteraction_Guide::GUIDE_TYPE::DEFAULT, m_pTransformCom->Get_WorldMatrixPtr(), _float2(0.f, 10.f), TEXT("가동"), 1.f);

    m_pGameInstance->Push_PoolObject_ToLayer(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Layer_UI"), m_pGuide);

    m_pGuide->Update_Visible(false);

    return S_OK;
}

HRESULT CInteraction_Item::Ready_Collision()
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
    BodyDesc.pCollisionDesc = &m_tCollisionDesc;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Body"),
        TEXT("Com_Body"), reinterpret_cast<CComponent**>(&m_pBodyCom), &BodyDesc)))
        return E_FAIL;
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
        }
    }
}

void CInteraction_Item::Reset()
{
    if (m_pGuide == nullptr) {

        Ready_Guide();            
    }

    if (m_pEffect == nullptr)
    {
        Ready_Effect();
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
}
