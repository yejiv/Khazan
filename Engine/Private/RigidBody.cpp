#include "RigidBody.h"
#include "GameInstance.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CRigidBody::CRigidBody(const CRigidBody& Prototype)
    : CComponent{ Prototype }
{
}

HRESULT CRigidBody::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRigidBody::Initialize_Clone(void* pArg)
{
    
    return S_OK;
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CRigidBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
    CRigidBody* pInstance = new CRigidBody(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CRigidBody"));
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CRigidBody::Free()
{
    __super::Free();

}
