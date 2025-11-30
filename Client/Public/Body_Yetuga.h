#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBody;
NS_END


NS_BEGIN(Client)

class CBody_Yetuga final : public CPartObject
{
public:
	typedef struct tagBodyDesc : public PARTOBJECT_DESC
	{
		CTransform* pOwnerTransform = { nullptr };
		class CYetuga*	pOwner = { nullptr };

	}BODY_DESC;

public:
	_float3					Get_BonePoint(const _char* BoneName);
	_float4*				Get_BonePointEX(const _char* BoneName);
    _float4*                Get_BonePointVFX(const _char* BoneName);
	_matrix					Get_BoneMatrix(const _char* pBoneName);

	void					Set_OnAttackCollision(_bool isToggle) { m_isOnAttackCollision = isToggle; }
    void                    Set_AttackCollision_Back(_bool isToggle) { m_isOnAttackCollision_Back = isToggle; }
private:
	CBody_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Yetuga(const CBody_Yetuga& Prototype);
	virtual ~CBody_Yetuga() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize_Clone(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;
    
    // Shader
    void                    Set_EnableEdge(_bool isEnable) { m_isEnableEdge = isEnable; }


private:
	HRESULT					Ready_Components();
    HRESULT				    Ready_AnimationEvent();
	HRESULT					Bind_ShaderResources();
	void					Carculate_Matrix(_float fTimeDelta);
    void                    Carculate_BakckMatrix(_float fTimeDelta);

private:
	HRESULT					Ready_Colliders();

private:
	class CYetuga*			m_pOwner = { nullptr };

private:
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CTransform*				m_pOwnerTransform = { nullptr };

private:
	CBody*					m_pRH_BodyCom = { nullptr }; // 오른손
	CBody*					m_pLH_BodyCom = { nullptr }; // 왼손
	CBody*					m_pBack_BodyCom = { nullptr }; // 등

private:
	_float3					m_vThrowPoint = {};
	_float4					m_vLockOnPoint = {};
    _float4                 m_pVFXBonePoint = {};

private:
	_float4x4				m_RightHandMatrix = {};
	_float4x4				m_LeftHandMatrix = {};
	_float4x4				m_BackMatrix = {};
	_float4x4				m_HeadMatrix = {};

	_bool					m_isOnAttackCollision = { false };
	_bool					m_isOnAttackCollision_Back = { false };
    _float4x4*              m_pLockOnBoneMatrix = { nullptr };
    // Shader
    _bool                   m_isEnableEdge = { true };

public:
	static CBody_Yetuga*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END