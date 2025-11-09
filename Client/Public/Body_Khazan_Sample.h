#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

#include "Player.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CXPBD;
class CBody;
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
	//void		Set_matSpearFX(_float4x4* mat) { m_pSpearFX_Matrix = mat; }
	//void		Set_matSpearWeaponR(_float4x4* mat) { m_pSpearWeaponR_Matrix = mat; }
	//void		Set_matSpearOffset(_matrix mat) { m_SpearOffset_Matrix = mat; }
	//void		Set_matWorldSpearBladeFX(_float4x4* mat) { m_SpearFX_WorldMatrix = mat; }
	//void		Set_matWorldSpearEndFX(_float4x4* mat) { m_SpearEndFX_WorldMatrix = mat; }

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


	/* 로컬  */
	_float4x4*	m_pSpearTip1_Matrix = { nullptr };		//창 날1
	_float4x4*	m_pSpearTip2_Matrix = { nullptr };		//창 날2
	_float4x4*	m_pWeaponR_Matrix   = { nullptr };		//손잡이
	_float4x4*	m_pSpearEnd1_Matrix = { nullptr };		//창 끝1
	_float4x4*	m_pSpearEnd2_Matrix = { nullptr };		//창 끝2

	/* 월드 (포인터형 아님!!) */
	_float4x4	m_pSpearTip1_MatrixW;		//창 날1
	_float4x4	m_pSpearTip2_MatrixW;		//창 날2
	_float4x4	m_pWeaponR_MatrixW;			//손잡이
	_float4x4	m_pSpearEnd1_MatrixW;		//창 끝1
	_float4x4	m_pSpearEnd2_MatrixW;		//창 끝2
	


	_float4x4*			m_SpearFX_WorldMatrix = { nullptr };
	_float4x4*			m_SpearEndFX_WorldMatrix = { nullptr };

	_uint* m_pParentState = { nullptr };
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

	class CXPBD* m_pXPBD = { nullptr };
	class CMeshTrail* m_pTrail = { nullptr };

	/* 디버그용 */
	CBody* m_pBody1 = { nullptr };
	CBody* m_pBody2 = { nullptr };
	CBody* m_pBody3 = { nullptr };
	CBody* m_pBody4 = { nullptr };
	CBody* m_pBody5 = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_AnimationEvent();

	HRESULT Ready_BonePhysics();
	HRESULT	Ready_Collider();
	void	Update_Collider(_float fTimeDelta);

	HRESULT Bind_ShaderResources();


	/*
	공격 진입~ 공격 끝날 때까지 ( 트리거 진입 , 범위, 탈출 이벤트 있음 )
	필요 시 함수 만들어서 사용하셔도 됩니다.

	LCTRL + Z	  : 달빛 베기
	LCTRL + X	  : 강공 세트
	LCTRL + C	  : 보름달
	LCTRL + V	  : 그림자베기
	LCTRL + B	  : 나선찌르기
	LCTRL + N	  : 나선 찌르기 소용돌이
	LCTRL + M	  : 강습
	LCTRL + G	  : 찰나베기
	LCTRL + H	  : 급소타격
	LCTRL + J	  : 그림자 참격
	LCTRL + K	  : 브루탈 어택?

	*/
private:
	void	Effect1_Enter();
	void	Effect1_Exit();
	void	Effect1_Continue();

	void	Effect2_Enter();		// z fast 1타		
	void	Effect2_Exit();
	void	Effect2_Continue();

	void	Effect3_Enter();
	void	Effect3_Exit();
	void	Effect3_Continue();

	void	Effect4_Enter();		//x  strong 1타
	void	Effect4_Exit();
	void	Effect4_Continue();

	void	Effect5_Enter();		//x  strong 2타
	void	Effect5_Exit();
	void	Effect5_Continue();

	void	Effect6_Enter();		//x  strong 3타
	void	Effect6_Exit();
	void	Effect6_Continue();

	void	Effect7_Enter();		//h 급소 타격
	void	Effect7_Exit();
	void	Effect7_Continue();

	void	Effect8_Enter();		//g 찰나 베기
	void	Effect8_Exit();
	void	Effect8_Continue();

	void	Effect9_Enter();		//m		강습
	void	Effect9_Exit();
	void	Effect9_Continue();

	void	Effect10_Enter();		// L  브루탈
	void	Effect10_Exit();
	void	Effect10_Continue();

	void	Effect11_Enter();		//c 보름달
	void	Effect11_Exit();
	void	Effect11_Continue();
	void    SpaceTime_SpearBlood();	//보름달 창 끝 피 이펙트 진입점

	void	Effect12_Enter();		//j  그림자 참격 1 타
	void	Effect12_Exit();
	void	Effect12_Continue();

	void	Effect13_Enter();		//j  그림자 참격 2 타
	void	Effect13_Exit();
	void	Effect13_Continue();

	void	Effect14_Enter();		//b 나선 찌르기
	void	Effect14_Exit();
	void	Effect14_Continue();

	void	Effect15_Enter();		//n 나선 찌르기소용돌이
	void	Effect15_Exit();
	void	Effect15_Continue();

private:
	inline void		Add_State(_uint i) { *m_pParentState |= i; }
	inline void		Toggle_State(_uint i) { *m_pParentState ^= i; }
	inline void		Remove_State(_uint i) { *m_pParentState &= ~i; }
	inline _bool	Has_State(_uint i) { return (*m_pParentState & i) != 0; }
	inline void		Clear_State() { *m_pParentState = 0; }
	inline _bool	Has_States();

public:
	static CBody_Khazan_Sample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
