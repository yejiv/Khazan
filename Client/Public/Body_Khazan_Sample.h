#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CBoneChainPhysic;
NS_END

NS_BEGIN(Client)

class CBody_Khazan_Sample final : public CPartObject
{
public:
	typedef struct tagBodyKhazanSampleDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint* pState = { nullptr };
		class CTransform* pParentTransform = { nullptr };

	}BODY_KHAZAN_SAMPLE_DESC;

private:
	CBody_Khazan_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Khazan_Sample(const CBody_Khazan_Sample& Prototype);
	virtual ~CBody_Khazan_Sample() = default;

public:
	_float4x4*  Get_BoneMatrix(const _char* pBoneName);
	void		Set_matSpearFX(_float4x4* mat) { m_pSpearFX_Matrix = mat; }
	void		Set_matSpearOffset(_matrix mat) { m_SpearOffset_Matrix = mat; }

public:
	OUTLINE_CONFIG Get_OutlineConfig() { return m_OutlineConfig; }
	void Set_OutlineConfig(OUTLINE_CONFIG Config)
	{
		m_OutlineConfig.vColor = Config.vColor;
		m_OutlineConfig.fSize = Config.fSize;
	}


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_Outline() override;

public:
	CModel* Get_Model() { return m_pModelCom; }

private:
	class CTransform*	m_pParentTransform = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };

	_float4x4*			m_pSpearFX_Matrix = { nullptr };
	_matrix				m_SpearOffset_Matrix = {};

	_uint*				m_pParentState = { nullptr };
	_uint				m_iCurState = {  };

	
	_bool				m_isFinishedAnimation = { false };
	_bool				m_isSetAnimation = { false };
	_uint				m_iCurSetAnimIndex = { 0 };

	const _uint			m_iSetAnimation[3] = { 3,2,1 };

	void Set_EnableEmissive(_bool isEnable) { m_isEnableEmissive = isEnable; }
	void Set_EnableBloom(_bool isEnable) { m_isEnableBloom = isEnable; }

	void Set_EmissiveIntensity(_float fIntensity) { m_fEmissiveIntensity = fIntensity; }

	_float m_fEmissiveIntensity = {};
	_bool m_isEnableEmissive = {};
	_bool m_isEnableBloom = {};
	OUTLINE_CONFIG m_OutlineConfig = { _float3(1.f, 0.f, 1.f), 0.001f, 0.f, 0.f };

	CBoneChainPhysic* m_pBoneChain = { nullptr };
	_bool m_isBoneChainEnabled = true;

	vector<vector<int>> m_HairChains; 


private:
	HRESULT Ready_Components();
	HRESULT Ready_AnimationEvent();

	HRESULT Ready_BonePhysics();

	HRESULT Bind_ShaderResources();


	/*
	공격 진입~ 공격 끝날 때까지 ( 트리거 진입 , 범위, 탈출 이벤트 다 있음 )
	필요 시 함수 만들어서 사용하셔도 됩니다.

	- 단일 찌르기 공격 -
	Effect1 : Fast

	- 연속 세트 찌르기 공격 -
	Effect2	: Set   1 번째 공격
	Effect3 : Set	2 번째 공격
	Effect4 : Set	3 번째 공격
	Effect5 : Set	4 번째 공격
	Effect6 : Set	5 번째 공격
	Effect7 : Set	6 번째 공격
	*/
private:
	void	Effect1_Enter();
	void	Effect1_Exit();
	void	Effect1_Continue();
	void	Effect2();
	void	Effect3();
	void	Effect4();
	void	Effect5();
	void	Effect6();
	void	Effect7_Enter();
	void	Effect7_Exit();
	void	Effect7_Continue();
	

private:
	inline void		Add_State(_uint i) { *m_pParentState |= i; }
	inline void		Toggle_State(_uint i) { *m_pParentState ^= i; }
	inline void		Remove_State(_uint i) {* m_pParentState &= ~i; }
	inline _bool	Has_State(_uint i) { return (*m_pParentState & i) != 0; }
	inline void		Clear_State() { *m_pParentState = 0; }
	inline _bool	Has_States();

public:
	static CBody_Khazan_Sample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
