#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CFont_Renderer final : public CBase
{
private:
	CFont_Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Renderer() = default;

public:
	HRESULT						Initialize();
	HRESULT						DrawText(class CFont_Face* pFont, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign);
	HRESULT						DrawTextBox(class CFont_Face* pFont, const _wstring& strText, _float fX, _float fY, _float fMaxWidth, _float fOffsetHeight, const _float4& vColor, TEXT_ALIGN eAlign);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	ID3D11Buffer*				m_pCB = { nullptr };
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };
	ID3D11SamplerState*			m_pSampler = { nullptr };

private:
	_float2						Offset_Align(TEXT_ALIGN eAlign, _float2 vSize);
public:
	static CFont_Renderer*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END