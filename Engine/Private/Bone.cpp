
#include "Bone.h"

CBone::CBone()
{

}

CBone::CBone(const CBone& Prototype)
	: m_strName{ Prototype.m_strName }
	, m_TransformationMatrix{ Prototype.m_TransformationMatrix }
	, m_CombinedTransformationMatrix{ Prototype.m_CombinedTransformationMatrix }
	, m_iParentBoneIndex{ Prototype.m_iParentBoneIndex }
	, m_iChildBones { Prototype.m_iChildBones }
{
}

HRESULT CBone::Initialize(BONE_DATA& data)
{
	m_strName = AnsiToWString(data.strName);

	memcpy(&m_TransformationMatrix, &data.transformationMatrix, sizeof(_float4x4));

	/*  m_CombinedTransformationMatrix는 매 프레임마다 부모의 컴바인행렬을 곱해줘서 실제 위치를 계산 */
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = data.iParentBoneIndex;

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CBone*>& Bones)
{
	if (-1 == m_iParentBoneIndex)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&PreTransformMatrix) * XMLoadFloat4x4(&m_TransformationMatrix));
		return;
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&m_TransformationMatrix)* XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));


}

CBone* CBone::Create(BONE_DATA& data)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(data)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();


}
