#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CFont_Face final : public CBase
{
private:
	CFont_Face(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Face() = default;

public:
	HRESULT						Initialize(FT_Library FTLib, const _char* pFontFilePath, _uint iWidth, _uint iHeight);
	_float2						ComputeTextSize(const _wstring& strText);
    void                        ComputeMaxBearingY(const _wstring& strText, _int& OutMaxBearingY, _int& OutMaxBottom);

	const GLYPH_INFO*			GetGlyph(_tchar ch);
	ID3D11ShaderResourceView*	GetSRV() const { return m_pSRV; }

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	ID3D11Texture2D*			m_pTexture = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;

	FT_Face						m_Face;

	vector<_uchar>				m_Buffer;
	unordered_map<_tchar,
		GLYPH_INFO>				m_Glyphs;

	_uint						m_iWidth = {};
	_uint						m_iHeight = {};

	_uint						m_iTexWidth = { 2048 };
	_uint						m_iTexHeight = { 2048 };

	_uint						m_iPenX = {};
	_uint						m_iPenY = {};
	_uint						m_iRowH = {};

	list<_tchar>				m_LRUList;
	unordered_map<_tchar,
		list<_tchar>::iterator> m_LRURefs;
	uint64_t					m_iFrameCounter = 0;


private:
	HRESULT						FontAtlas_Setting();
	HRESULT						Add_Glyphs(_tchar ch);
	void						Update_LRU(_tchar ch);
	HRESULT						EvictOldGlyphs();
public:
	static CFont_Face*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, FT_Library	FTLib, const _char* pFontFilePath, _uint iWidth, _uint iHeight);
	virtual void				Free() override;
};

NS_END	