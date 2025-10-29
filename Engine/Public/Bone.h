#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	CBone(const CBone& Prototype);
	virtual ~CBone() = default;

public:
	HRESULT Initialize(BONE_DATA& data);
	void Update_CombinedTransformationMatrix(const _float4x4& PreTransformMatrix, const vector<CBone*>& Bones);

	/* Info */
public:
	_matrix		Get_CombinedTransformationMatrix() const { return XMLoadFloat4x4(&m_CombinedTransformationMatrix); }
	_float4x4*	Get_CombinedTransformationMatrixPtr() { return &m_CombinedTransformationMatrix; }
	void		Set_CombinedTransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_CombinedTransformationMatrix, Matrix); }

	_matrix		Get_TransformationMatrix() const { return XMLoadFloat4x4(&m_TransformationMatrix); }
	_float4x4*	Get_TransformationMatrixPtr() { return &m_TransformationMatrix; }
	void		Set_TransformationMatrix(_fmatrix Matrix) { XMStoreFloat4x4(&m_TransformationMatrix, Matrix); }

	_bool			Compare_Name(const _wstring& pName) { return pName == m_strName; }
	_bool			Compare_Name(const _tchar* pName) { return static_cast<_wstring>(pName) == m_strName; }
	_bool			Compare_Name(const _char* pName) { return CharToWString(pName) == m_strName;}
	_bool			Contains_Name(const _wstring& pName) { return m_strName.find(pName) != _wstring::npos; }
	_bool			Contains_Name(const _tchar* pName) { return m_strName.find(static_cast<_wstring>(pName)) != _wstring::npos; }
	_bool			Contains_Name(const _char* pName) { return 	m_strName.find(CharToWString(pName)) != _wstring::npos;}

private:
	_wstring			m_strName = {};
	_float4x4			m_TransformationMatrix = {};
	_float4x4			m_CombinedTransformationMatrix = {};

	_int				m_iParentBoneIndex = { -1 };

public:
	static CBone* Create(BONE_DATA& data);
	CBone* Clone();
	virtual void Free() override;
};

NS_END
