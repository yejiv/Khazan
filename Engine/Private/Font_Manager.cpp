#include "Font_Manager.h"

#include "Font_Face.h"
#include "Font_Renderer.h"

CFont_Manager::CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice }
    , m_pContext { pContext } 
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Initialize()
{
    if (FT_Init_FreeType(&m_FT))
        return E_FAIL;

    m_pRenderer = CFont_Renderer::Create(m_pDevice, m_pContext);
    if (nullptr == m_pRenderer)
        return E_FAIL;

    return S_OK;
}

HRESULT CFont_Manager::Font_Load(const _wstring& strFontTag, const _char* pFontFilePath, _uint iWidth, _uint iHeight)
{
    if (m_Fonts.find(strFontTag) != m_Fonts.end())
        return S_OK; // 이미 로드됨

    CFont_Face* pFont = CFont_Face::Create(m_pDevice, m_pContext, m_FT, pFontFilePath, iWidth, iHeight);
    if (pFont == nullptr)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);
    return S_OK;
}

HRESULT CFont_Manager::Font_Load_Data(const _char* pFontFilePath)
{
    ifstream In(pFontFilePath);
    if (!In.is_open())
    {
        MSG_BOX(TEXT("Font json 파일 불러오기 실패"));
        In.close();
    }
    else
    {
        nlohmann::json jsonData;
        In >> jsonData;

        for (auto& t : jsonData["Font"])
        {
            string strFontTag = t.value("tag", "");
            string strFontFilePath = ".." + t.value("path", "");
            _int iSize = t.value("size", 0);

            Font_Load(AnsiToWString(strFontTag), strFontFilePath.c_str(), 0, iSize);
        }

    }
    return S_OK;
}

HRESULT CFont_Manager::Draw_Text(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign)
{
    CFont_Face* pFont = Find_Font(strFontTag);
    if (!pFont)    
        return E_FAIL;

    return m_pRenderer->DrawText(pFont, strText, fX, fY, vColor, eAlign);
}

HRESULT CFont_Manager::Draw_TextBox(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, _float fMaxWidth, _float fOffsetHeight, const _float4& vColor, TEXT_ALIGN eAlign)
{
    CFont_Face* pFont = Find_Font(strFontTag);
    if (!pFont)
        return E_FAIL;

    return m_pRenderer->DrawTextBox(pFont, strText, fX, fY, fMaxWidth, fOffsetHeight, vColor, eAlign);;
}

HRESULT CFont_Manager::DrawTextWorld(const _wstring& strFontTag, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign, _matrix WorldMat)
{
    CFont_Face* pFont = Find_Font(strFontTag);
    if (!pFont)
        return E_FAIL;

    return m_pRenderer->DrawTextWorld(pFont, strText, fX, fY, vColor, eAlign, WorldMat);
}

CFont_Manager* CFont_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CFont_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CFont_Face* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto it = m_Fonts.find(strFontTag);

    if (it == m_Fonts.end())
        return nullptr;

    return it->second;
}

void CFont_Manager::Free()
{
    __super::Free();
   
    for (auto& pair : m_Fonts)
        Safe_Release(pair.second);
    m_Fonts.clear();
  
    Safe_Release(m_pRenderer);
   
    if (m_FT)
    {
        FT_Done_FreeType(m_FT);
        m_FT = nullptr;
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
