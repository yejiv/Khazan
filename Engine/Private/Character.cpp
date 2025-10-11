#include "EnginePch.h"
//#include "Character.h"
//#include "GameInstance.h"
//#include "Transform.h"
//#include "RigidBody.h"
//
//// ===== 유틸 =====
//static inline _float Dot(const _float3& a, const _float3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
//static inline _float3 Sub(const _float3& a, const _float3& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
//static inline _float3 Add(const _float3& a, const _float3& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
//static inline _float3 Mul(const _float3& a, _float s) { return { a.x * s, a.y * s, a.z * s }; }
//
//static inline _float3 SlideOnPlane(const _float3& v, const _float3& n)
//{
//	// v - dot(v,n) * n
//	const _float d = Dot(v, n);
//	return { v.x - d * n.x, v.y - d * n.y, v.z - d * n.z };
//}
//
//CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//	: CComponent { pDevice, pContext }
//    , m_pGameInstance { CGameInstance::GetInstance() }
//{
//    Safe_AddRef(m_pGameInstance);
//
//}
//
//CCharacter::CCharacter(const CCharacter& Prototype)
//	: CComponent{ Prototype }
//{
//
//}
//
//HRESULT CCharacter::Initialize_Prototype()
//{
//
//	return S_OK;
//}
//
//HRESULT CCharacter::Initialize_Clone(void* pArg)
//{
//	CHARACTER_DESC* pDesc = static_cast<CHARACTER_DESC*>(pArg);
//	if (!pDesc || !pDesc->pBody) return E_FAIL;
//
//	m_fMaxSlopeDeg = pDesc->fMaxSlopeDeg;
//	m_fStepOffset = pDesc->fStepOffset;
//	m_fGroundSnap = pDesc->fGroundSnap;
//	m_fGravity = pDesc->fGravity;
//
//	// 초기 동기화(Transform 포즈 → Kinematic Body)
//	_float4x4 WorldMatrix{};
//	XMStoreFloat4x4(&WorldMatrix, pDesc->pTransform->Get_WorldMatrix());
//	pDesc->pBody->Sync_Kinematic(WorldMatrix);
//	return S_OK;
//}
//void CCharacter::Update(_float fTimeDelta, CRigidBody* pBody, CTransform* pTransform)
//{
//	Update_Kinematic(fTimeDelta, pBody, pTransform);
//}
//void CCharacter::Update_Kinematic(_float fTimeDelta, CRigidBody* pBody, CTransform* pTransform)
//{
//    // 1) 입력 기반 목표 속도에서 시작
//    _float3 vVel = m_vDesired;
//
//    // 2) 중력/점프/접지 스냅
//    if (m_bGrounded)
//    {
//        if (m_bWantJump)
//        {
//            vVel.y = m_fJumpV0;
//            m_bWantJump = false;
//            m_bGrounded = false;
//        }
//        else
//        {
//            // 살짝 끌어붙이기(스냅)
//            vVel.y = -m_fGroundSnap / fTimeDelta;
//        }
//    }
//    else
//    {
//        vVel.y -= m_fGravity * fTimeDelta;
//    }
//
//    // 3) 현재 위치와 타겟 위치
//    _float3 vPos = { pTransform->Get_State(STATE::POSITION).m128_f32[0], pTransform->Get_State(STATE::POSITION).m128_f32[1], pTransform->Get_State(STATE::POSITION).m128_f32[2] };
//    _float3 vTarget = Add(vPos, Mul(vVel, fTimeDelta));
//
//    //// 4) 바닥 감지(간단 RayCast ↓). 실제로는 스텝/경사 각도 고려 스윕 권장
//    //{
//    //    // Ray: 발목 근처에서 아래로
//    //    const _float rayLen = 0.5f + m_fGroundSnap; // 필요시 캡슐 반지름/반높이 고려
//    //    RRayCast ray(RVec3(vPos.x, vPos.y, vPos.z), Vec3(0, -1, 0), rayLen);
//
//    //    RayCastResult hit;
//    //    PhysicsSystem* ps = m_pBody ? m_pBody->Get_OwnerWorldPhysicsSystem() : nullptr; // 없으면 아래 분기
//    //    // 만약 Get_OwnerWorldPhysicsSystem 같은 헬퍼가 없다면, CRigidBody::m_pPhysics 를 공개 Getter로 만들어 쓰자.
//    //    PhysicsSystem* pPhysics = nullptr;
//    //    {
//    //        // CRigidBody에 PhysicsSystem* Getter가 있다고 가정:
//    //        // pPhysics = m_pBody->Get_PhysicsSystem();
//    //    }
//
//    //    if (!pPhysics)
//    //    {
//    //        // 임시: 바닥 있다고 가정 (툰업 전 스텁)
//    //        m_bGrounded = true;
//    //    }
//    //    else
//    //    {
//    //        // 충돌 레이어 필터는 네 엔진 정책으로 교체
//    //        // 여기서는 전부 히트하는 기본 쿼리 사용
//    //        m_bGrounded = pPhysics->GetNarrowPhaseQuery().CastRay(ray, hit);
//    //    }
//
//    //    if (m_bGrounded && vVel.y <= 0.f)
//    //    {
//    //        // 슬로프 노멀을 얻으려면 RayCastResult에서 서페이스 노멀 쿼리가 필요.
//    //        // NarrowPhaseQuery의 CastRay는 히트만 주므로, 실제 프로젝트에선 RayCastWithCollector 등으로 노멀을 받거나
//    //        // 캐릭터 캡슐 스위프에서 Contact normal을 얻는 쪽으로 가는 게 좋아.
//    //        // 여기선 간단히 상향 노멀(0,1,0)로 슬라이드만.
//    //        const _float3 n = { 0,1,0 };
//
//    //        // 경사 제한(간단 버전): n과 Up의 각도 검사로 처리 가능. 여기선 생략.
//    //        vVel = SlideOnPlane(vVel, n);
//    //        vTarget = Add(vPos, Mul(vVel, fTimeDelta));
//    //    }
//    //}
//
//    // 5) 최종 포즈를 Transform에 반영
//    pTransform->Set_State(STATE::POSITION, XMVectorSet(vTarget.x, vTarget.y, vTarget.z, 1.f));
//
//    // 6) Kinematic Body에 반영
//    _float4x4 WorldMatrix{};
//    XMStoreFloat4x4(&WorldMatrix, pTransform->Get_WorldMatrix());
//    pBody->Sync_Kinematic(WorldMatrix);
//
//    // 7) 상태 저장
//    m_vActual = vVel;
//}
//
//CCharacter* CCharacter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//	CCharacter* pInstance = new CCharacter(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX(TEXT("Failed to Created : CCharacter"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CComponent* CCharacter::Clone(void* pArg)
//{
//	CCharacter* pInstance = new CCharacter(*this);
//
//	if (FAILED(pInstance->Initialize_Clone(pArg)))
//	{
//		MSG_BOX(TEXT("Failed to Cloned : CCharacter"));
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//
//void CCharacter::Free()
//{
//	__super::Free();
//    Safe_Release(m_pGameInstance);
//}
