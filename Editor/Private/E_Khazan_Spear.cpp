#include "E_Khazan_Spear.h"
#include "E_Body_Khazan_Spear.h"

#include "GameInstance.h"

//#include "Khazan_Spear_ASManager.h"




CE_Khazan_Spear::CE_Khazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainerObject{ pDevice, pContext }
{
}

CE_Khazan_Spear::CE_Khazan_Spear(const CE_Khazan_Spear& Prototype)
    : CContainerObject{ Prototype }
{
}

HRESULT CE_Khazan_Spear::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CE_Khazan_Spear::Initialize_Clone(void* pArg)
{
    GAMEOBJECT_DESC Desc{};
    Desc.fRotationPerSec = XMConvertToRadians(180.f);
    Desc.fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_strName = "Khazan";

    m_pTransformCom->Scaling(_float3(3.f, 3.f, 3.f));
    m_pTransformCom->Rotation(XMConvertToRadians(0.f), XMConvertToRadians(180.f), XMConvertToRadians(0.f));
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, -20.f, 1.f));

    return S_OK;
}

void CE_Khazan_Spear::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CE_Khazan_Spear::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CE_Khazan_Spear::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CE_Khazan_Spear::Render()
{
    return S_OK;
}

HRESULT CE_Khazan_Spear::Ready_Components()
{
    return S_OK;
}


HRESULT CE_Khazan_Spear::Ready_PartObjects()
{
    CE_Body_Khazan_Spear::BODY_KHAZAN_SPEAR_DESC         BodyDesc{};

    BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

    BodyDesc.pParentTransform = m_pTransformCom;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), ENUM_CLASS(LEVEL::SHADER), TEXT("Prototype_GameObject_Body_Khazan_Spear"), &BodyDesc)))
        return E_FAIL;

    m_pBody = static_cast<CE_Body_Khazan_Spear*>(Find_PartObject(TEXT("Part_Body")));

    return S_OK;
}

CE_Khazan_Spear* CE_Khazan_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CE_Khazan_Spear* pInstance = new CE_Khazan_Spear(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CE_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CE_Khazan_Spear::Clone(void* pArg)
{
    CE_Khazan_Spear* pInstance = new CE_Khazan_Spear(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CE_Khazan_Spear"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CE_Khazan_Spear::Free()
{
    __super::Free();
}
