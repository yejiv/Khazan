#pragma once
#include "UI_Button.h"
#include "Editor_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END


NS_BEGIN(Editor)

class CEdit_Button final : public CUI_Button
{
private:
	CEdit_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Button(const CEdit_Button& Prototype);
	virtual ~CEdit_Button() = default;

//public:
//	void						Set_LocalPos(const _float3& vPos) { m_vLocalPos = vPos; }
//	void						Set_LocalSize(const _float3& vSize) { m_vLocalSize = vSize; }
//	_float3						Get_LocalPos() const { return m_vLocalPos; }
//	_float3						Get_LocalSize() const { return m_vLocalSize; }

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

private:
	HRESULT						Ready_Components();

private:
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

public:
	static CEdit_Button*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

NS_END;