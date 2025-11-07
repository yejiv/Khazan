#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CDecal_Manager final : public CBase
{
private:
	CDecal_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDecal_Manager() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);
	HRESULT						Render();
	HRESULT						Spawn_Decal(const _wstring& strPoolTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, const DECAL_DESC& Desc);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	list<class CDecal*>			m_Decals;

	class CShader*				m_pShader = {};
	class CVIBuffer_Cube*		m_pVIBuffer = {};
	class CTexture*				m_pTexture[ENUM_CLASS(DECALTYPE::END)] = { nullptr };

private:
	HRESULT						Ready_Components();

public:
	static CDecal_Manager*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END