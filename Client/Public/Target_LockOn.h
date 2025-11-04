#pragma once
#include "UI_Texture.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CTarget_LockOn final : public CUI_Texture
{
private:
	CTarget_LockOn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTarget_LockOn(const CTarget_LockOn& Prototype);
	virtual ~CTarget_LockOn() = default;

public:
	void								LockOn(const _float4* pTargetPos, _float2 vOffset = {0.f, 0.f});
	void								LockOff();

public:
	virtual HRESULT						Initialize_Prototype(_uint iLevel);
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	CShader*							m_pShaderCom = { nullptr };
	CTexture*							m_pTextureCom = { nullptr };
	CVIBuffer_Rect*						m_pVIBufferCom = { nullptr };

	const _float4*						m_pTargetPos = { nullptr };
	_float								m_fDelta = {};
private:
	HRESULT								Ready_Prototype();
	HRESULT								Ready_Component();

	void								Update_WorldPos();
	void								BrutalAttack_Check(const EVENT_LOCKON_VISIBLE& e);

public:
	static CTarget_LockOn*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END