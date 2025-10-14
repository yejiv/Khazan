#pragma once

#include "Editor_Defines.h"
#include "Prop.h"

// 정적 맵 오브젝트 ( 인스턴싱 O )

NS_BEGIN(Editor)

class CProp_Animated final : public CProp
{
public:
	typedef struct tagPropAnimatedDesc : public CProp::PROP_DESC
	{
		_bool isIndependentObject{ false };
		_vector vPosition{};
		_vector vRotation{};
		_vector vScale{};

	}PROP_ANIMATED_DESC;

	enum class SHADER_PASS { AB, BC, END };			// 수정 예정

private:
	CProp_Animated(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Animated(const CProp_Animated& Prototype);
	virtual ~CProp_Animated() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const _uint Get_NumInstances() const;

	// MapTool 에서만 사용
	void Add_Instance(MESH_INSTANCE_DATA InstanceData);

	// MapTool 에서만 사용
	void Fix_Instance(MESH_INSTANCE_DATA InstanceData, _uint InstanceIndex);

private:
	class CEditor_Model* m_pModelCom = { nullptr };

private:
	HRESULT Ready_Components(void* pArg);
	virtual HRESULT Bind_ShaderResources();

	HRESULT Bind_Materials(_uint iMeshIndex);

public:
	static CProp_Animated* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END