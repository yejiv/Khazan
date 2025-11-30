#include "Interaction_Object.h"
#include "GameInstance.h"
#include "Body.h"

CInteraction_Object::CInteraction_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{

}

CInteraction_Object::CInteraction_Object(const CInteraction_Object& Prototype)
    : CGameObject{ Prototype }
{

}

HRESULT CInteraction_Object::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteraction_Object::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_isPool = true;    

    return S_OK;
}

void CInteraction_Object::Priority_Update(_float fTimeDelta)
{
}

void CInteraction_Object::Update(_float fTimeDelta)
{
}

void CInteraction_Object::Late_Update(_float fTimeDelta)
{
}

HRESULT CInteraction_Object::Render()
{
    return S_OK;
}

HRESULT CInteraction_Object::Ready_Components()
{

    return S_OK;
}

HRESULT CInteraction_Object::Bind_ShaderResources()
{
    return S_OK;
}

CInteraction_Object* CInteraction_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInteraction_Object* pInstance = new CInteraction_Object(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CInteraction_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInteraction_Object::Clone(void* pArg)
{
    CInteraction_Object* pInstance = new CInteraction_Object(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CInteraction_Object"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInteraction_Object::Free()
{
    __super::Free();

    Safe_Release(m_pBodyCom);
}
