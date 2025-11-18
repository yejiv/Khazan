#include "Font_Renderer.h"
#include "Font_Face.h"
#include "GameInstance.h"

CFont_Renderer::CFont_Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext }, m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CFont_Renderer::Initialize()
{
    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.ByteWidth = sizeof(XMFLOAT4X4) + sizeof(XMFLOAT4) + sizeof(XMFLOAT4);
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pCB);

    const _uint maxVerts = 8192; // 1300글자?
    const _uint maxIndices = maxVerts;

    D3D11_SAMPLER_DESC samp{};
    samp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samp.AddressU = samp.AddressV = samp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    m_pDevice->CreateSamplerState(&samp, &m_pSampler);

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = sizeof(FONTVERTEX) * maxVerts;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&vbDesc, nullptr, &m_pVB);

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.ByteWidth = sizeof(_uint) * maxIndices;
    ibDesc.Usage = D3D11_USAGE_DYNAMIC;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(&ibDesc, nullptr, &m_pIB);

	return S_OK;
}

HRESULT CFont_Renderer::DrawText(CFont_Face* pFont, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign)
{
    if (!pFont || strText.empty())
        return E_FAIL;

    _float2 vSize = pFont->ComputeTextSize(strText);

    _float2 fOffset = Offset_Align(eAlign, vSize);
    
    _float cursorX = fX + fOffset.x;
    _float cursorY = fY + fOffset.y;

    vector<FONTVERTEX> verts;
    vector<_uint> indices;
    verts.reserve(strText.size() * 4);
    indices.reserve(strText.size() * 6);

    _uint baseIndex = 0;

    _float minX = +FLT_MAX;
    _float minY = +FLT_MAX;
    _float maxX = -FLT_MAX;
    _float maxY = -FLT_MAX;

    for (_tchar ch : strText)
    {
        const GLYPH_INFO* g = pFont->GetGlyph(ch);
        if (!g) continue;

        _float xpos = cursorX + g->iBearingX;
        _float ypos = cursorY - g->iBearingY;
        _float w = (_float)g->iWidth;
        _float h = (_float)g->iHeight;

        verts.push_back({ {xpos,     ypos + h, 0}, {g->u0, g->v1} });
        verts.push_back({ {xpos,     ypos,     0}, {g->u0, g->v0} });
        verts.push_back({ {xpos + w, ypos,     0}, {g->u1, g->v0} });
        verts.push_back({ {xpos + w, ypos + h, 0}, {g->u1, g->v1} });

        indices.insert(indices.end(), { baseIndex, baseIndex + 1, baseIndex + 2, baseIndex, baseIndex + 2, baseIndex + 3 });
        baseIndex += 4;
        
        minX = min(minX, xpos);
        minY = min(minY, ypos);
        maxX = max(maxX, xpos + w);
        maxY = max(maxY, ypos + h);

        cursorX += g->iAdvance;
    }

    _float rectX = minX;
    _float rectY = minY;
    _float rectW = maxX - minX;
    _float rectH = maxY - minY;
    // VB Map/Unmap
    D3D11_MAPPED_SUBRESOURCE mappedVB{};
    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
    memcpy(mappedVB.pData, verts.data(), verts.size() * sizeof(FONTVERTEX));
    m_pContext->Unmap(m_pVB, 0);

    // IB Map/Unmap
    D3D11_MAPPED_SUBRESOURCE mappedIB{};
    m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIB);
    memcpy(mappedIB.pData, indices.data(), indices.size() * sizeof(_uint));
    m_pContext->Unmap(m_pIB, 0);

    // IA 세팅
    _uint stride = sizeof(FONTVERTEX);
    _uint offset = 0;
    m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
    m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 상수버퍼 (색상 + 투영)
    struct CBData { _float4x4 mtx; _float4 color; _float4 rect;} cb;
    XMStoreFloat4x4(&cb.mtx, XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0, 1920, 1080, 0, 0, 1)));

    cb.color = vColor;
    cb.rect = { rectX, rectY, rectW, rectH };
    m_pContext->UpdateSubresource(m_pCB, 0, nullptr, &cb, 0, 0);
    m_pContext->VSSetConstantBuffers(0, 1, &m_pCB);
    m_pContext->PSSetConstantBuffers(0, 1, &m_pCB);

    // 텍스처, 샘플러
    ID3D11ShaderResourceView* srv = pFont->GetSRV();
    m_pContext->PSSetShaderResources(0, 1, &srv);
    m_pContext->PSSetSamplers(0, 1, &m_pSampler);

    // Draw
    m_pContext->DrawIndexed((_uint)indices.size(), 0, 0);

    return S_OK;
}

HRESULT CFont_Renderer::DrawTextBox(CFont_Face* pFont, const _wstring& strText, _float fX, _float fY, _float fMaxWidth, _float fOffsetHeight, const _float4& vColor, TEXT_ALIGN eAlign)
{

    if (!pFont || strText.empty())
        return E_FAIL;

    _float2 vSize = pFont->ComputeTextSize(strText);

    _float2 fOffset = Offset_Align(eAlign, vSize);
    _float fOffsetWidth = Offset_Align(eAlign, { fMaxWidth, 0.f }).x;

    _float cursorX = fX + fOffset.x;
    if (eAlign == TEXT_ALIGN::LEFT_BOTTOM || eAlign == TEXT_ALIGN::LEFT_CENTER || eAlign == TEXT_ALIGN::LEFT_TOP)
        cursorX = fX;
    else if(eAlign == TEXT_ALIGN::CENTER_TOP || eAlign == TEXT_ALIGN::CENTER || eAlign == TEXT_ALIGN::CENTER_BOTTOM)
        cursorX = fX + fOffsetWidth;
    
    _float cursorY = fY + fOffset.y;

    vector<FONTVERTEX> verts;
    vector<_uint> indices;
    verts.reserve(strText.size() * 4);
    indices.reserve(strText.size() * 6);

    _uint baseIndex = 0;

    _float lineHeight = vSize.y + fOffsetHeight;
    _float lineWidth = 0.f;

    _float minX = +FLT_MAX;
    _float minY = +FLT_MAX;
    _float maxX = -FLT_MAX;
    _float maxY = -FLT_MAX;

    for (_tchar ch : strText)
    {
        if (ch == L'\\')
        {
            if (eAlign == TEXT_ALIGN::LEFT_BOTTOM || eAlign == TEXT_ALIGN::LEFT_CENTER || eAlign == TEXT_ALIGN::LEFT_TOP)
                cursorX = fX;
            else
            {
                Offset_Align(eAlign, vSize);
                cursorX = fX + fOffsetWidth;
            }
            lineWidth = 0.f;
            cursorY += lineHeight;
            continue;
        }

        const GLYPH_INFO* g = pFont->GetGlyph(ch);
        if (!g) continue;

        if (lineWidth + g->iAdvance > fMaxWidth)
        {
            if (eAlign == TEXT_ALIGN::LEFT_BOTTOM || eAlign == TEXT_ALIGN::LEFT_CENTER || eAlign == TEXT_ALIGN::LEFT_TOP)
                cursorX = fX;
            else
            {
                Offset_Align(eAlign, vSize);
                cursorX = fX + fOffsetWidth;
            }
            cursorY += lineHeight;
            lineWidth = 0.f;
        }

        _float xpos = cursorX + g->iBearingX;
        _float ypos = cursorY - (g->iHeight - g->iBearingY);
        _float w = (_float)g->iWidth;
        _float h = (_float)g->iHeight;

        verts.push_back({ {xpos,     ypos + h, 0}, {g->u0, g->v1} });
        verts.push_back({ {xpos,     ypos,     0}, {g->u0, g->v0} });
        verts.push_back({ {xpos + w, ypos,     0}, {g->u1, g->v0} });
        verts.push_back({ {xpos + w, ypos + h, 0}, {g->u1, g->v1} });

        indices.insert(indices.end(), { baseIndex, baseIndex + 1, baseIndex + 2, baseIndex, baseIndex + 2, baseIndex + 3 });
        baseIndex += 4;

        minX = min(minX, xpos);
        minY = min(minY, ypos);
        maxX = max(maxX, xpos + w);
        maxY = max(maxY, ypos + h);
        
        cursorX += g->iAdvance;
        lineWidth += g->iAdvance;
        vSize.x -= g->iAdvance;


    }
    
    _float rectX = minX;
    _float rectY = minY;
    _float rectW = maxX - minX;
    _float rectH = maxY - minY;

    // VB Map/Unmap
    D3D11_MAPPED_SUBRESOURCE mappedVB{};
    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
    memcpy(mappedVB.pData, verts.data(), verts.size() * sizeof(FONTVERTEX));
    m_pContext->Unmap(m_pVB, 0);

    // IB Map/Unmap
    D3D11_MAPPED_SUBRESOURCE mappedIB{};
    m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIB);
    memcpy(mappedIB.pData, indices.data(), indices.size() * sizeof(_uint));
    m_pContext->Unmap(m_pIB, 0);

    // IA 세팅
    _uint stride = sizeof(FONTVERTEX);
    _uint offset = 0;
    m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
    m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 상수버퍼 (색상 + 투영)
    struct CBData { _float4x4 mtx; _float4 color; _float4 rect; } cb;
    XMStoreFloat4x4(&cb.mtx, XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0, 1920, 1080, 0, 0, 1)));

    cb.color = vColor;
    cb.rect = { rectX, rectY, rectW, rectH };
    m_pContext->UpdateSubresource(m_pCB, 0, nullptr, &cb, 0, 0);
    m_pContext->VSSetConstantBuffers(0, 1, &m_pCB);
    m_pContext->PSSetConstantBuffers(0, 1, &m_pCB);

    // 텍스처, 샘플러
    ID3D11ShaderResourceView* srv = pFont->GetSRV();
    m_pContext->PSSetShaderResources(0, 1, &srv);
    m_pContext->PSSetSamplers(0, 1, &m_pSampler);

    // Draw
    m_pContext->DrawIndexed((_uint)indices.size(), 0, 0);

    return S_OK;
}

HRESULT CFont_Renderer::DrawTextWorld(CFont_Face* pFont, const _wstring& strText, _float fX, _float fY, const _float4& vColor, TEXT_ALIGN eAlign, _matrix WorldMat)
{
    if (!pFont || strText.empty())
        return E_FAIL;

    _float2 vSize = pFont->ComputeTextSize(strText);
    _float2 fOffset = Offset_Align(eAlign, vSize);

    _int iMaxBearingY = {};
    _int iMaxBottom = {};

    pFont->ComputeMaxBearingY(strText, iMaxBearingY, iMaxBottom);

    _float cursorX = fX + fOffset.x;
    _float cursorY = fY + fOffset.y + iMaxBearingY;
    _float worldScale = 0.005f;

    vector<FONTVERTEX> verts;
    vector<_uint> indices;
    verts.reserve(strText.size() * 4);
    indices.reserve(strText.size() * 6);

    _uint baseIndex = 0;

    for (_tchar ch : strText)
    {
        const GLYPH_INFO* g = pFont->GetGlyph(ch);
        if (!g) continue;

        _float x = (cursorX + g->iBearingX) * worldScale;
        _float w = g->iWidth * worldScale;
        _float h = g->iHeight * worldScale;

        _float top = (cursorY - g->iBearingY) * worldScale;
        _float bottom = top + g->iHeight * worldScale;

        verts.push_back({ {x,      top,    0}, {g->u0, g->v1} });
        verts.push_back({ {x,      bottom, 0}, {g->u0, g->v0} });
        verts.push_back({ {x + w,  bottom, 0}, {g->u1, g->v0} });
        verts.push_back({ {x + w,  top,    0}, {g->u1, g->v1} });

        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);

        baseIndex += 4;

        cursorX += g->iAdvance; 
    }


    // VB Map/Unmap
    D3D11_MAPPED_SUBRESOURCE mappedVB{};
    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB);
    memcpy(mappedVB.pData, verts.data(), verts.size() * sizeof(FONTVERTEX));
    m_pContext->Unmap(m_pVB, 0);

    // IB Map/Unmap
    D3D11_MAPPED_SUBRESOURCE mappedIB{};
    m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIB);
    memcpy(mappedIB.pData, indices.data(), indices.size() * sizeof(_uint));
    m_pContext->Unmap(m_pIB, 0);

    // IA 세팅
    _uint stride = sizeof(FONTVERTEX);
    _uint offset = 0;
    m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);
    m_pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R32_UINT, 0);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 상수버퍼 (색상 + 투영)
    struct CBData { _float4x4 mtx; _float4 color; _float4 rect; } cb;
    _matrix matWorld = WorldMat;
    _matrix matView = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
    _matrix matProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
    _matrix matWVP = matWorld * matView * matProj;
    XMStoreFloat4x4(&cb.mtx, XMMatrixTranspose(matWVP));
    

    cb.color = vColor;
    cb.rect = { 0.f,0.f,0.f,0.f };
    m_pContext->UpdateSubresource(m_pCB, 0, nullptr, &cb, 0, 0);
    m_pContext->VSSetConstantBuffers(0, 1, &m_pCB);
    m_pContext->PSSetConstantBuffers(0, 1, &m_pCB);

    // 텍스처, 샘플러
    ID3D11ShaderResourceView* srv = pFont->GetSRV();
    m_pContext->PSSetShaderResources(0, 1, &srv);
    m_pContext->PSSetSamplers(0, 1, &m_pSampler);

    // Draw
    m_pContext->DrawIndexed((_uint)indices.size(), 0, 0);

    return S_OK;
}

_float2 CFont_Renderer::Offset_Align(TEXT_ALIGN eAlign, _float2 vSize)
{
    _float2 vAlign = {};
    switch (eAlign)
    {
    case TEXT_ALIGN::LEFT_TOP:
        vAlign = { 0.f, 0.f };
        break;
    case TEXT_ALIGN::CENTER_TOP:
        vAlign = { -vSize.x * 0.5f, 0.f };
        break;
    case TEXT_ALIGN::RIGHT_TOP:
        vAlign = { -vSize.x, 0.f };
        break;
    case TEXT_ALIGN::LEFT_CENTER:
        vAlign = { 0.f, -vSize.y * 0.5f };
        break;
    case TEXT_ALIGN::CENTER:
        vAlign = { -vSize.x * 0.5f, -vSize.y * 0.5f };
        break;
    case TEXT_ALIGN::RIGHT_CENTER:
        vAlign = { -vSize.x, -vSize.y * 0.5f };
        break;
    case TEXT_ALIGN::LEFT_BOTTOM:
        vAlign = { 0.f, -vSize.y };
        break;
    case TEXT_ALIGN::CENTER_BOTTOM:
        vAlign = { -vSize.x * 0.5f, -vSize.y };
        break;
    case TEXT_ALIGN::RIGHT_BOTTOM:
        vAlign = { -vSize.x, -vSize.y };
        break;

    }
    return vAlign;
}

CFont_Renderer* CFont_Renderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFont_Renderer* pInstance = new CFont_Renderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CFont_Renderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Renderer::Free()
{
	__super::Free();

    Safe_Release(m_pCB);
    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
    Safe_Release(m_pSampler);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
