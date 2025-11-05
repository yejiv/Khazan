#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDecal final : public CGameObject
{
private:
	CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal(const CDecal& Prototype);
	virtual ~CDecal() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize_Clone(void* pArg);
	virtual void			Priority_Update(_float fTimeDelta);
	virtual void			Update(_float fTimeDelta);
	virtual void			Late_Update(_float fTimeDelta);
	virtual HRESULT			Render();
	virtual void			Reset() override;

public:
	_float					Get_Opacity() { return m_fOpacity; }
	
	DECAL_DESC				Get_Desc() { return m_Desc; }
	void					Set_Desc(DECAL_DESC Desc);
	
	_uint					Get_TextureIndex() { return m_iTextureIndex; }
	void					Set_TextureIndex(_uint iIndex) { m_iTextureIndex = iIndex; }

	_uint					Get_RandomSeed() { return m_iRandSeed; }
	void					Set_RandomSeed(_uint iSeed) { m_iRandSeed = iSeed; }

private:
	_float					m_fTimeAcc = {};
	_float					m_fOpacity = {};
	_uint					m_iTextureIndex = {};
	_uint					m_iRandSeed = {};

	DECAL_DESC				m_Desc = {};

public:
	static CDecal*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END