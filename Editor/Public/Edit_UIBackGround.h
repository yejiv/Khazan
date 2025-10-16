#pragma once
#include "Editor_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Rect;
class CShader;
NS_END

NS_BEGIN(Editor)
class CEdit_UIBackGround : public CUIObject
{
private:
	CEdit_UIBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_UIBackGround(const CUIObject& Prototype);
	virtual ~CEdit_UIBackGround() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	_float3&					Get_BackColor();

private:
	CShader*					m_pShaderCom = { nullptr };
	CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };
	_float3						m_vBackColor = {};

private:
	HRESULT						Ready_Component();

public:
	static CEdit_UIBackGround*  Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

NS_END




