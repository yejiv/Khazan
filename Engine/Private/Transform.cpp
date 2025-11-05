#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "RigidBody.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
	
{

}


HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_Shader_Resource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

_vector CTransform::Get_Rotation_Quat()
{
	_matrix W = XMLoadFloat4x4(&m_WorldMatrix);

	_vector S, Q, T;

	if (!XMMatrixDecompose(&S, &Q, &T, W))
	{

		XMFLOAT4X4 m; XMStoreFloat4x4(&m, W);


		_vector r0 = XMVector3Normalize(XMVectorSet(m._11, m._12, m._13, 0.f));
		_vector r1 = XMVector3Normalize(XMVectorSet(m._21, m._22, m._23, 0.f));
		_vector r2 = XMVector3Normalize(XMVectorSet(m._31, m._32, m._33, 0.f));


		_matrix RotationMatrix(
			r0,
			r1,
			r2,
			XMVectorSet(0.f, 0.f, 0.f, 1.f)
		);

		Q = XMQuaternionRotationMatrix(RotationMatrix);
	}

	return Q;
}

void CTransform::Set_Quaternion(_vector vQuaternion)
{
	XMMATRIX W = XMLoadFloat4x4(&m_WorldMatrix);

	XMVECTOR S, Q_old, T;
	XMMatrixDecompose(&S, &Q_old, &T, W);

	XMVECTOR q = XMQuaternionNormalize(vQuaternion);

	XMMATRIX W_new = XMMatrixAffineTransformation(S, XMVectorZero(), q, T);
	XMStoreFloat4x4(&m_WorldMatrix, W_new);
}

void CTransform::Scale(_float3 vScale)
{
	Set_State(STATE::RIGHT, XMVector3Normalize(Get_State(STATE::RIGHT)) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(Get_State(STATE::UP)) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(Get_State(STATE::LOOK)) * vScale.z);
}

void CTransform::Scaling(_float3 vScale)
{
	Set_State(STATE::RIGHT, Get_State(STATE::RIGHT) * vScale.x);
	Set_State(STATE::UP, Get_State(STATE::UP) * vScale.y);
	Set_State(STATE::LOOK, Get_State(STATE::LOOK) * vScale.z);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);

}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);

}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);

}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);

}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));
	
	// XMVector3TransformNormal();

}

void CTransform::Rotation(_float fX, _float fY, _float fZ)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_vector		vQuaternion = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));


}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE::RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(STATE::UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(STATE::LOOK, XMVector4Transform(vLook, RotationMatrix));

}

void CTransform::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - Get_State(STATE::POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::LookAt_Lerp(_fvector vAt, _float fTimeDelta, _float fTurnSpeed)
{
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);
	vLook = XMVector3Normalize(XMVectorSetY(vLook, 0.f));
	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(vAt - vPosition, 0.f));

	_vector vNewLook = XMVector3Normalize(XMVectorLerp(vLook,vTargetDir,fTimeDelta * fTurnSpeed));
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vNewLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

	_float3 vScale = Get_Scaled();
	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vNewLook) * vScale.z);

}

void CTransform::Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(STATE::POSITION);
	_vector		vMoveDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vMoveDir));

	if(fDistance >= fLimit)
		vPosition += XMVector3Normalize(vMoveDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);

}

void CTransform::AI_Chase(_fvector vTargetPos, _float fTimeDelta, _float SpeedPerSec, _float fLimit)
{
	// 거리 제곱 기반으로 사용되는 추적 함수
	_vector vPosition = Get_State(STATE::POSITION);
	_vector vMoveDir = vTargetPos - vPosition;
	_float fDistSq = XMVectorGetX(XMVector3LengthSq(vMoveDir));
	if (fDistSq <= fLimit)
		return;

	//_vector vDirNorm = XMVector3Normalize(vMoveDir);
	// 목표까지 남은거리와 프레임당 이동거리를 비교해서 더 작은거리로 선택해서 이동
	// 오버 슈팅을 방지시킬 수 있음.
	//_float fMove = min(sqrt(fDistSq) - fLimit, m_fSpeedPerSec * fTimeDelta);
	//vPosition += vDirNorm * fMove;

	if (fDistSq >= fLimit)
		vPosition += XMVector3Normalize(vMoveDir) * SpeedPerSec * fTimeDelta;

	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Look_Dir(_fvector vDir)
{
	// 인자로 들어온 방향을 Look으로 설정 후 외적 == 인자 방향과 같은 방향을 쳐다보도록 회전

	_float3 vScaled = Get_Scaled();

	_vector		vLook = vDir;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vRight = {};

	if (XMVectorGetX(XMVector3Dot(vLook, vUp)) >= 0.999f)
		vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE::RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE::UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE::LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
