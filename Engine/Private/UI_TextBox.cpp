#include "UI_TextBox.h"
#include "GameInstance.h"

CUI_TextBox::CUI_TextBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{pDevice,pContext}
{

}

CUI_TextBox::CUI_TextBox(const CUI_TextBox& Prototype)
	:CUIObject{Prototype}
{

}

void CUI_TextBox::Set_Text(const _wstring& strText)
{
	m_strText = strText;
	m_isChange = true;
}

void CUI_TextBox::Set_FontColor(const _float3& vColor)
{
	m_vFontColor = vColor;
	m_isChange = true;

}

void CUI_TextBox::Set_FontTag(const _wstring& strFontTag)
{
	m_strFontTag = strFontTag;
	m_isChange = true;
}

void CUI_TextBox::Set_FontScale(const _float2& vScale)
{
	m_vFontScale = vScale;
	m_isChange = true;

}

void CUI_TextBox::Set_FontAlpha(_float fAlpha)
{
	m_fAlpha = fAlpha;
	m_isChange = true;
}

void CUI_TextBox::Set_Centered(UI_ALIGNMENT eAligment)
{
	__super::Set_Alignment(eAligment);
	m_isChange = true;

}

HRESULT CUI_TextBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_TextBox::Initialize_Clone(void* pArg)
{
	TEXTBOX_DESC* pDesc = static_cast<TEXTBOX_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_strText = TEXT("...");
	m_strFontTag = pDesc->strFontTag;
	m_vFontColor = pDesc->vColor;
	m_vFontScale = pDesc->vFontScale;
	m_fAlpha = pDesc->fAlpha;
	m_isChange = false;

	return S_OK;
}

void CUI_TextBox::Priority_Update(_float fTimeDelta)
{

}

void CUI_TextBox::Update(_float fTimeDelta)
{
	if (m_isChange)
	{
		// TextBox 정보를 바꿨을때 초기화 하는 함수.
		Recalculate_TextInfo();
		m_isChange = false;
	}
}

void CUI_TextBox::Late_Update(_float fTimeDelta)
{

}

HRESULT CUI_TextBox::Render()
{

	m_pGameInstance->DrawTextW(m_strFontTag.c_str(), m_strText.c_str(),
		m_vFontPos,
		XMVectorSet(m_vFontColor.x, m_vFontColor.y, m_vFontColor.z, m_fAlpha),
		0.f,
		_float2(m_vLocalPos.x + (m_vLocalSize.x * 0.5f + m_vFontOffset.x) , -m_vLocalPos.y + (m_vLocalSize.y * 0.5f + m_vFontOffset.y)),
		m_vFontScale);
	
	return S_OK;
}

void CUI_TextBox::Recalculate_TextInfo()
{
	//_float2 vTextSize = m_pGameInstance->Compute_TextSize(m_strFontTag,m_strText,m_vFontScale);
	m_vFontOffset = m_pGameInstance->Compute_TextSize(m_strFontTag,m_strText,m_vFontScale);
	//m_vFontPos = Compute_AlignedPos(_float2(m_vWorldPos.x,m_vWorldPos.y),vTextSize);
	m_vFontPos = Compute_AlignedPos(_float2(m_vWorldPos.x,m_vWorldPos.y),_float2(m_vLocalSize.x * 0.5f,m_vLocalSize.y * 0.5f));
	
}

void CUI_TextBox::Free()
{
	__super::Free();
}
