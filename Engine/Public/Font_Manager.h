#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;
		
public:
	HRESULT							Initialize();

	HRESULT							Font_Load(const _wstring& strFontTag, const _char* pFontFilePath, _uint iWidth, _uint iHeight);
	HRESULT							Font_Load_Data(const _char* pFontFilePath);
	HRESULT							Draw_Text(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign);
	HRESULT							Draw_TextBox(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, _float fMaxWidth, _float fOffsetHeight, const _float4& vColor, TEXT_ALIGN eAlign);

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	FT_Library						m_FT;
	
	unordered_map<_wstring,
		class CFont_Face*>			m_Fonts;

	class CFont_Renderer*			m_pRenderer = nullptr;

private:
	class CFont_Face*				Find_Font(const _wstring& strFontTag);

public:
	static CFont_Manager*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void					Free() override;
};

NS_END