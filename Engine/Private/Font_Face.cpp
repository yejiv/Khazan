#include "Font_Face.h"

CFont_Face::CFont_Face(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{pDevice}, m_pContext{pContext}
{
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CFont_Face::Initialize(FT_Library FTLib, const _char* pFontFilePath, _uint iWidth, _uint iHeight)
{
    if (FT_New_Face(FTLib, pFontFilePath, 0, &m_Face))
    {
        MSG_BOX(TEXT("폰트 로드 실패"));
        return E_FAIL;
    }
    FT_Set_Pixel_Sizes(m_Face, iWidth, iHeight);
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    return FontAtlas_Setting();
}

_float2 CFont_Face::ComputeTextSize(const _wstring& strText)
{
    _float fTotalWidth = 0.f;
    _float fMaxTop = 0.f;
    _float fMaxBottom = 0.f;

    for (_tchar ch : strText)
    {
        const GLYPH_INFO* g = GetGlyph(ch);
        if (!g) continue;

        fTotalWidth += g->iAdvance;
        fMaxTop = max(fMaxTop, (_float)g->iBearingY);

        _float bottom = (_float)(g->iHeight - g->iBearingY);
        fMaxBottom = max(fMaxBottom, bottom);
    }
    _float fTotalHeight = fMaxTop + fMaxBottom;
    return { fTotalWidth, fTotalHeight };
}

void CFont_Face::ComputeMaxBearingY(const _wstring& strText, _int& OutMaxBearingY, _int& OutMaxBottom)
{
    _int maxBearingY = 0;
    _int maxBottom = 0;

    for (_tchar ch : strText)
    {
        const GLYPH_INFO* g = GetGlyph(ch);
        if (!g) continue;

        maxBearingY = max(maxBearingY, g->iBearingY);

        _int bottom = g->iHeight - g->iBearingY;
        maxBottom = max(maxBottom, bottom);
    }

    OutMaxBearingY = maxBearingY;
    OutMaxBottom = maxBottom;
}

const GLYPH_INFO* CFont_Face::GetGlyph(_tchar ch)
{
    m_iFrameCounter++;

    auto it = m_Glyphs.find(ch);
    if (it == m_Glyphs.end())
    {
        CHECK_FAILED(Add_Glyphs(ch), nullptr);
           
        auto it = m_Glyphs.find(ch);
        return &it->second;
    }
    
    Update_LRU(ch);
    it->second.lastUsedFrame = m_iFrameCounter;
    return &it->second;
}

HRESULT CFont_Face::FontAtlas_Setting()
{
    m_Buffer.assign(m_iTexWidth * m_iTexHeight, 0);
    
    _wstring preload = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789안녕하세요가나다라마바사아자차카타파하";
    for (wchar_t ch : preload)
    {
        if (FT_Load_Char(m_Face, ch, FT_LOAD_RENDER)) continue;
        FT_Bitmap& bmp = m_Face->glyph->bitmap;

        if (m_iPenX + bmp.width >= m_iTexWidth)
        {
            m_iPenX = 0;
            m_iPenY += m_iRowH + 1;
            m_iRowH = 0;
        }

        for (int y = 0; y < bmp.rows; ++y)
        {
            memcpy(&m_Buffer[(m_iPenY + y) * m_iTexWidth + m_iPenX], &bmp.buffer[y * bmp.pitch], bmp.width);
        }

        GLYPH_INFO g;
        g.iWidth = bmp.width;
        g.iHeight = bmp.rows;
        g.iBearingX = m_Face->glyph->bitmap_left;
        g.iBearingY = m_Face->glyph->bitmap_top;
        g.iAdvance = m_Face->glyph->advance.x >> 6;
        g.u0 = (float)m_iPenX / m_iTexWidth;
        g.v0 = (float)m_iPenY / m_iTexHeight;
        g.u1 = (float)(m_iPenX + bmp.width) / m_iTexWidth;
        g.v1 = (float)(m_iPenY + bmp.rows) / m_iTexHeight;

        m_Glyphs[ch] = g;
        m_iPenX += bmp.width + 1;
        m_iRowH = max(m_iRowH, (UINT)bmp.rows);
    }

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = m_iTexWidth;
    desc.Height = m_iTexHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = m_Buffer.data();
    init.SysMemPitch = m_iTexWidth;

    CHECK_FAILED(m_pDevice->CreateTexture2D(&desc, &init, &m_pTexture), E_FAIL);
    CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pSRV), E_FAIL);

    return S_OK;
}

HRESULT CFont_Face::Add_Glyphs(_tchar ch)
{
    if (m_Glyphs.find(ch) != m_Glyphs.end())
        return S_OK;

    if (FT_Load_Char(m_Face, ch, FT_LOAD_RENDER))
        return E_FAIL;

    FT_Bitmap& bmp = m_Face->glyph->bitmap;

    if (m_iPenX + bmp.width >= m_iTexWidth)
    {
        m_iPenX = 0;
        m_iPenY += m_iRowH + 1;
        m_iRowH = 0;
    }

    if (m_iPenY + bmp.rows >= m_iTexHeight)
    {
        if (FAILED(EvictOldGlyphs()))
        {
            MSG_BOX(TEXT("폰트 페이스 제거 실패"));
            return E_FAIL;
        }
    }

    for (int y = 0; y < bmp.rows; ++y)
    {
        memcpy(&m_Buffer[(m_iPenY + y) * m_iTexWidth + m_iPenX],
            &bmp.buffer[y * bmp.pitch],
            bmp.width);
    }

    D3D11_BOX box{};
    box.left = m_iPenX;
    box.top = m_iPenY;
    box.front = 0;
    box.right = m_iPenX + bmp.width;
    box.bottom = m_iPenY + bmp.rows;
    box.back = 1;

    m_pContext->UpdateSubresource(
        m_pTexture,
        0,
        &box,
        &m_Buffer[m_iPenY * m_iTexWidth + m_iPenX],
        m_iTexWidth,
        0
    );

    GLYPH_INFO g{};
    g.iWidth = bmp.width;
    g.iHeight = bmp.rows;
    g.iBearingX = m_Face->glyph->bitmap_left;
    g.iBearingY = m_Face->glyph->bitmap_top;
    g.iAdvance = m_Face->glyph->advance.x >> 6;
    g.u0 = (float)m_iPenX / m_iTexWidth;
    g.v0 = (float)m_iPenY / m_iTexHeight;
    g.u1 = (float)(m_iPenX + bmp.width) / m_iTexWidth;
    g.v1 = (float)(m_iPenY + bmp.rows) / m_iTexHeight;

    m_Glyphs[ch] = g;

    m_iPenX += bmp.width + 1;
    m_iRowH = max(m_iRowH, (UINT)bmp.rows);

    return S_OK;
}

void CFont_Face::Update_LRU(_tchar ch)
{
    auto ref = m_LRURefs.find(ch);

    if (ref != m_LRURefs.end())
        m_LRUList.erase(ref->second);

    m_LRUList.push_back(ch);
    m_LRURefs[ch] = prev(m_LRUList.end());
}

HRESULT CFont_Face::EvictOldGlyphs()
{
    _int iRemoveValue = 32;
    
    _int iRemoved = 0;

    while (!m_LRUList.empty() && iRemoved < iRemoveValue)
    {
        _tchar szOldch = m_LRUList.front();
        m_LRUList.pop_front();
        m_LRURefs.erase(szOldch);

        auto it = m_Glyphs.find(szOldch);
        if (it != m_Glyphs.end())
        {
            const GLYPH_INFO& g = it->second;

            vector<uint8_t> zeroData(g.iWidth * g.iHeight, 0);
            D3D11_BOX box{};
            box.left = (UINT)(g.u0 * m_iTexWidth);
            box.top = (UINT)(g.v0 * m_iTexHeight);
            box.right = (UINT)(g.u1 * m_iTexWidth);
            box.bottom = (UINT)(g.v1 * m_iTexHeight);
            box.front = 0;
            box.back = 1;

            m_pContext->UpdateSubresource(
                m_pTexture, 0, &box,
                zeroData.data(),
                g.iWidth,
                0
            );

            m_Glyphs.erase(it);
            iRemoved++;
        }
    }
    m_iPenX = 0;
    m_iPenY = 0;
    m_iRowH = 0;
    return (iRemoved > 0) ? S_OK : E_FAIL;
}

CFont_Face* CFont_Face::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, FT_Library FTLib, const _char* pFontFilePath, _uint iWidth, _uint iHeight)
{
	CFont_Face* pInstance = new CFont_Face(pDevice, pContext);

	if (FAILED(pInstance->Initialize(FTLib, pFontFilePath, iWidth, iHeight)))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Face"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Face::Free()
{
	__super::Free();

    if (m_Face)
    {
        FT_Done_Face(m_Face);
        m_Face = nullptr;
    }

    m_Glyphs.clear();
    m_Buffer.clear();

    m_LRURefs.clear();

    Safe_Release(m_pTexture);
    Safe_Release(m_pSRV);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
