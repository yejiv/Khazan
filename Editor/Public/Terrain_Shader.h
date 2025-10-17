#pragma once

#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
NS_END

NS_BEGIN(Editor)

class CTerrain_Shader final : public CGameObject
{
private:
	CTerrain_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain_Shader(const CTerrain_Shader& Prototype);
	virtual ~CTerrain_Shader() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTerrain_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END