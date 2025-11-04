#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CTarget_BrutalAttack final : public CUI_Panel
{
private:
	CTarget_BrutalAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTarget_BrutalAttack(const CTarget_BrutalAttack& Prototype);
	virtual ~CTarget_BrutalAttack() = default;

public:
	void								Setting_BrutalAttack(const _float4* pTargetPos, _float fDelayTime = 1.f, _float2 vOffset = { 0.f, 0.f });
	void								Off_BrutalAttack();
public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

	virtual void						Reset() override;
private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	class CBrutalAttack_Progress*		m_pProgress = { nullptr };
	class CBrutalAttack_Point*			m_pPointBg = { nullptr };
	class CBrutalAttack_Point*			m_pPoint	= { nullptr };
	class CBrutalAttack_Point*			m_pPointfront = { nullptr };

	const _float4*						m_pTargetPos = { nullptr };
	
	_float								m_fTime = {10};
	_float								m_fMaxTime = {10};

private:
	HRESULT								Ready_Prototype();
	HRESULT								Ready_Component();
	void								Update_WorldPos();

	HRESULT								Ready_GameObject();

public:
	static CTarget_BrutalAttack*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END