#include "Editor_Bone.h"

CEditor_Bone::CEditor_Bone()
{
}

CEditor_Bone::CEditor_Bone(const CEditor_Bone& Prototype)

: m_TransformationMatrix{ Prototype.m_TransformationMatrix }
, m_CombinedTransformationMatrix{ Prototype.m_CombinedTransformationMatrix }
, m_iParentBoneIndex{ Prototype.m_iParentBoneIndex }
, m_Bone_Data{ Prototype.m_Bone_Data }
{
}

HRESULT CEditor_Bone::Initialize(const aiNode* pAINode, _int iParentBoneIndex)
{
	strcpy_s(m_szName, pAINode->mName.data);

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	/* assimp가 주는 모든행렬들은 column - major 형식임.전치행렬 해줘야함!!!!!! */
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	/*  m_CombinedTransformationMatrix는 매 프레임마다 부모의 컴바인행렬을 곱해줘서 실제 위치를 계산 */
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_iParentBoneIndex = iParentBoneIndex;

	m_Bone_Data.iParentBoneIndex = m_iParentBoneIndex;
	m_Bone_Data.strName = string(m_szName);
	memcpy(&m_Bone_Data.transformationMatrix, &m_TransformationMatrix, sizeof(_float4x4));

	//string message = "Bone Name : " + string(m_szName) + "\n";
	//OutputDebugStringA(message.c_str());

	return S_OK;
}

void CEditor_Bone::Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CEditor_Bone*>& Bones)
{
	if (-1 == m_iParentBoneIndex)
	{
		/* 루트에 Pre행렬 곱하면 모든 뼈에 적용됨 */
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&PreTransformMatrix) * XMLoadFloat4x4(&m_TransformationMatrix));
		return;
	}

	/* 같은 클래스의 인스턴스끼리는 private 멤버에 접근이 가능함. */
	XMStoreFloat4x4(&m_CombinedTransformationMatrix,
		XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));

}

CEditor_Bone* CEditor_Bone::Create(const aiNode* pAINode, _int iParentBoneIndex)
{
	CEditor_Bone* pInstance = new CEditor_Bone();

	if (FAILED(pInstance->Initialize(pAINode, iParentBoneIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CEditor_Bone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEditor_Bone* CEditor_Bone::Clone()
{
	return new CEditor_Bone(*this);
}

void CEditor_Bone::Free()
{
	__super::Free();


}