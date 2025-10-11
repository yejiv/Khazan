#include "EnginePch.h"
//#include "Collider.h"
//#include "GameInstance.h"
//
//CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//	: CComponent { pDevice, pContext }
//    , m_pGameInstance { CGameInstance::GetInstance() }
//{
//    Safe_AddRef(m_pGameInstance);
//
//}
//
//CCollider::CCollider(const CCollider& Prototype)
//	: CComponent{ Prototype }
//{
//
//}
//
//HRESULT CCollider::Initialize_Prototype()
//{
//
//	return S_OK;
//}
//
//HRESULT CCollider::Initialize_Clone(void* pArg)
//{
//    if (!pArg)
//        return E_FAIL;
//
//    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
//
//    m_iObjectLayer = pDesc->iObjectLayer;
//    return Build_Shape(*pDesc);
//}
//
//HRESULT CCollider::Build_Shape(const COLLIDER_DESC& pDesc)
//{
//    switch (pDesc.eShape)
//    {
//    case COLLIDER_SHAPE::BOX:
//        m_pShape = new BoxShape(Vec3(pDesc.vHalfExtents.x, pDesc.vHalfExtents.y, pDesc.vHalfExtents.z));
//        break;
//    case COLLIDER_SHAPE::SPHERE:
//        m_pShape = new SphereShape(pDesc.fRadius);
//        break;
//    case COLLIDER_SHAPE::CAPSULE:
//        m_pShape = new CapsuleShape(pDesc.fHalfHeight, pDesc.fRadius);
//        break;
//    default: 
//        return E_FAIL;
//    }
//    return m_pShape ? S_OK : E_FAIL;
//}
//
//CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
//{
//	CCollider* pInstance = new CCollider(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX(TEXT("Failed to Created : CCollider"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CComponent* CCollider::Clone(void* pArg)
//{
//	CCollider* pInstance = new CCollider(*this);
//
//	if (FAILED(pInstance->Initialize_Clone(pArg)))
//	{
//		MSG_BOX(TEXT("Failed to Cloned : CCollider"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//
//void CCollider::Free()
//{
//	__super::Free();
//    Safe_Release(m_pGameInstance);
//}
