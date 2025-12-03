#pragma once
#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Editor)

class CEditor_Bone final :public CBase
{
private:
	CEditor_Bone();
	CEditor_Bone(const CEditor_Bone& Prototype);
	virtual ~CEditor_Bone() = default;

public:
	HRESULT Initialize(const aiNode* pAINode, _int iParentBoneIndex);
	void Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CEditor_Bone*>& Bones);

	_bool Compare_Name(const _char* pName) { return !strcmp(pName, m_szName); }

	_matrix Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	_float4x4* Get_CombinedTransformationMatrixPtr() { return &m_CombinedTransformationMatrix; }
	void Set_CombinedTransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_CombinedTransformationMatrix, Matrix); }

	_matrix Get_TransformationMatrix() const { return XMLoadFloat4x4(&m_TransformationMatrix); }
	_float4x4* Get_TransformationMatrixPtr() { return &m_TransformationMatrix; }
	void Set_TransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_TransformationMatrix, Matrix); }

public:
	void	Get_Data(BONE_DATA& data) { data = m_Bone_Data; }
	const _char*	Get_Name() {return m_szName; }

private:
	_char				m_szName							[MAX_PATH]					 = {};
	_float4x4			m_TransformationMatrix					= {};
	_float4x4			m_CombinedTransformationMatrix				 = {};
	_int				m_iParentBoneIndex								=	 { -1 };

	BONE_DATA			m_Bone_Data												{};

public:
	static CEditor_Bone* Create(const aiNode* pAINode, _int iParentBoneIndex);
	CEditor_Bone* Clone();
	virtual void Free() override;
};

NS_END