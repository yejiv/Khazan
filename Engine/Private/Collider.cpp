#include "EnginePch.h"
#include "Collider.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
    , m_pGameInstance { CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);

}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
	, m_isColl { Prototype.m_isColl}
{

}

HRESULT CCollider::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCollider::Initialize_Clone(void* pArg)
{
    auto* pDesc = static_cast<COLLIDER_DESC*>(pArg);

    m_fStandingHeight = pDesc->fStandingHeight;
    m_fStandingRadius = pDesc->fStandingRadius;
    m_fCrouchingHeight = pDesc->fCrouchingHeight;
    m_fCrouchingRadius = pDesc->fCrouchingRadius;
    m_fInnerShapeFraction = pDesc->fInnerShapeFraction;
    m_bCrouching = pDesc->bStartCrouch;
    m_bUseInner = pDesc->bUseInner;
    m_eCollider_Shape = pDesc->eShape;

    BuildShapes();
    SelectActiveShape();

    //// Body 생성 (Kinematic 권장: 네 Transform이 주도)
    //auto& BI = m_pGameInstance->Get_BodyInterface();
    //BodyCreationSettings bcs(
    //    m_ShActive,
    //    RVec3::sZero(), Quat::sIdentity(),
    //    EMotionType::Kinematic,
    //    (ObjectLayer)pDesc->uObjectLayer
    //);
    //bcs.mFriction = 0.0f;
    //bcs.mRestitution = 0.0f;

    //if (Body* body = BI.CreateBody(bcs)) {
    //    m_BodyID = body->GetID();
    //    BI.AddBody(m_BodyID, EActivation::DontActivate);
    //}
    //else {
    //    return E_FAIL;
    //}

    return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	//m_pBounding->Update(WorldMatrix);
}

_bool CCollider::Intersect(CCollider* pTarget)
{
	//return m_isColl = m_pBounding->Intersect(pTarget->m_eType, pTarget->m_pBounding);

    return false;
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	/*m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);	

	m_pBatch->Begin();

	m_pBounding->Render(m_pBatch, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

	m_pBatch->End();*/

	return S_OK;
}

void CCollider::BuildShapes()
{
    auto MakeTranslated = [](const ShapeRefC& sh, float yOff)->ShapeRefC {
        RotatedTranslatedShapeSettings rts(Vec3(0, yOff, 0), Quat::sIdentity(), sh);
        return rts.Create().Get();
        };

    auto MakeCapsule = [](float height, float radius)->ShapeRefC {
        float half = 0.5f * height;
        return ShapeRefC(new CapsuleShape(half, radius));
        };
    auto MakeCylinder = [](float height, float radius)->ShapeRefC {
        float half = 0.5f * height;
        return ShapeRefC(new CylinderShape(half, radius));
        };
    auto MakeBox = [](float height, float radius)->ShapeRefC {
        // half extents
        Vec3 he(radius, 0.5f * height + radius, radius);
        return ShapeRefC(new BoxShape(he));
        };

    auto Make4 = [&](auto makeShape, float h0, float r0, float h1, float r1)
        {
            float y0 = 0.5f * h0 + r0;
            float y1 = 0.5f * h1 + r1;
            m_ShStanding = MakeTranslated(makeShape(h0, r0), y0);
            m_ShCrouching = MakeTranslated(makeShape(h1, r1), y1);
            m_ShInnerStanding = MakeTranslated(makeShape(m_fInnerShapeFraction * h0,
                m_fInnerShapeFraction * r0),
                m_fInnerShapeFraction * y0);
            m_ShInnerCrouching = MakeTranslated(makeShape(m_fInnerShapeFraction * h1,
                m_fInnerShapeFraction * r1),
                m_fInnerShapeFraction * y1);
        };

    switch (m_eCollider_Shape)
    {
    case COLLIDER_SHAPE::CAPSULE:
        Make4(MakeCapsule, m_fStandingHeight, m_fStandingRadius,
            m_fCrouchingHeight, m_fCrouchingRadius);
        break;
    case COLLIDER_SHAPE::CYLINDER:
        Make4(MakeCylinder, m_fStandingHeight, m_fStandingRadius,
            m_fCrouchingHeight, m_fCrouchingRadius);
        break;
    case COLLIDER_SHAPE::BOX:
        Make4(MakeBox, m_fStandingHeight, m_fStandingRadius,
            m_fCrouchingHeight, m_fCrouchingRadius);
        break;
    case COLLIDER_SHAPE::COMPOUND:
    {
        // 예시: 캡슐+박스를 좌우로 합친 복합체. 발바닥 기준 오프셋 동일 개념.
        StaticCompoundShapeSettings st, cr, ist, icr;

        { // standing
            float hs = 0.5f * m_fStandingHeight, rs = m_fStandingRadius;
            float y = hs + rs;
            st.AddShape(Vec3(-0.3f, y, 0), Quat::sIdentity(), new CapsuleShape(hs, rs));
            st.AddShape(Vec3(0.3f, y, 0), Quat::sIdentity(), new BoxShape(Vec3(rs, hs + rs, rs)));
            m_ShStanding = st.Create().Get();
        }
        { // crouching
            float hc = 0.5f * m_fCrouchingHeight, rc = m_fCrouchingRadius;
            float y = hc + rc;
            cr.AddShape(Vec3(-0.3f, y, 0), Quat::sIdentity(), new CapsuleShape(hc, rc));
            cr.AddShape(Vec3(0.3f, y, 0), Quat::sIdentity(), new BoxShape(Vec3(rc, hc + rc, rc)));
            m_ShCrouching = cr.Create().Get();
        }
        { // inner standing
            float hs = 0.5f * (m_fInnerShapeFraction * m_fStandingHeight);
            float rs = m_fInnerShapeFraction * m_fStandingRadius;
            float y = hs + rs;
            ist.AddShape(Vec3(-0.3f, y, 0), Quat::sIdentity(), new CapsuleShape(hs, rs));
            ist.AddShape(Vec3(0.3f, y, 0), Quat::sIdentity(), new BoxShape(Vec3(rs, hs + rs, rs)));
            m_ShInnerStanding = ist.Create().Get();
        }
        { // inner crouching
            float hc = 0.5f * (m_fInnerShapeFraction * m_fCrouchingHeight);
            float rc = m_fInnerShapeFraction * m_fCrouchingRadius;
            float y = hc + rc;
            icr.AddShape(Vec3(-0.3f, y, 0), Quat::sIdentity(), new CapsuleShape(hc, rc));
            icr.AddShape(Vec3(0.3f, y, 0), Quat::sIdentity(), new BoxShape(Vec3(rc, hc + rc, rc)));
            m_ShInnerCrouching = icr.Create().Get();
        }
    } break;
    }
}

void CCollider::SelectActiveShape()
{
}

void CCollider::ApplyActiveShapeToBody()
{
}

void CCollider::SetCrouch(bool v)
{
}

void CCollider::UseInner(bool v)
{
}

#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCollider::Free()
{
	__super::Free();
    Safe_Release(m_pGameInstance);
}
