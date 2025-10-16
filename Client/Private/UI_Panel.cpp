#include "UI_Panel.h"

CUI_Panel::CUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject{ pDevice, pContext }
{
}

CUI_Panel::CUI_Panel(const CUI_Panel& Prototype)
	:CUIObject{ Prototype }
{
}

HRESULT CUI_Panel::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUI_Panel::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CUI_Panel::Priority_Update(_float fTimeDelta)
{
}

void CUI_Panel::Update(_float fTimeDelta)
{
}

void CUI_Panel::Late_Update(_float fTimeDelta)
{
   
}

HRESULT CUI_Panel::Render()
{
	return S_OK;
}

void CUI_Panel::Update_LayOut()
{
    //if (!m_isAlign)
    //    return;

    //// 트렌스폼을 갱신하기전에 Panel 기준으로 정렬하는 기능

    //_float2 vOffset = {};

    //for (auto& pChild : m_Children)
    //{
    //    if (nullptr == pChild)
    //        continue;

    //    _float2 vChildSize = { pChild->Get_LocalSize().x, pChild->Get_LocalSize().y };

    //    // Panel 기준 위치 + 오프셋
    //    _float2 vBasePos = { m_vLocalPos.x + vOffset.x, m_vLocalPos.y + vOffset.y };

    //    // 자식의 Alignment 적용 (Panel 기준 정렬)
    //    _float2 vAlignedPos = pChild->Compute_AlignedPos(vBasePos, vChildSize);

    //    // 최종 로컬 좌표로 설정
    //    pChild->Set_LocalPos(_float3(vAlignedPos.x, vAlignedPos.y, 0.f));

    //    // 다음 자식의 기준 오프셋 이동
    //    switch (m_eType)
    //    {
    //    case PANEL_TYPE::VERTICAL:
    //        vOffset.y += vChildSize.y + m_fSpace;
    //        break;
    //    case PANEL_TYPE::HORIZONTAL:
    //        vOffset.x += vChildSize.x + m_fSpace;
    //        break;
    //    case PANEL_TYPE::END:
    //        break;
    //    }
    //}

    if (!m_isAlign)
        return;

    // Panel 기준 오프셋 초기화
    _float2 vOffset = {};

    for (auto& pChild : m_Children)
    {
        if (nullptr == pChild)
            continue;

        _float2 vChildSize = { pChild->Get_LocalSize().x, pChild->Get_LocalSize().y };

        // Panel 기준 위치 제외, Offset만 적용
        _float2 vBasePos = vOffset;

        // 자식의 Alignment 적용 (Panel 기준)
        _float2 vAlignedPos = pChild->Compute_AlignedPos(vBasePos, vChildSize);

        // 최종 로컬 좌표로 설정
        pChild->Set_LocalPos(_float3(vAlignedPos.x, vAlignedPos.y, 0.f));

        // 다음 자식 기준 오프셋 이동
        switch (m_eType)
        {
        case PANEL_TYPE::VERTICAL:
            vOffset.y += vChildSize.y + m_fSpace;
            break;
        case PANEL_TYPE::HORIZONTAL:
            vOffset.x += vChildSize.x + m_fSpace;
            break;
        case PANEL_TYPE::END:
            break;
        }
    }

}

void CUI_Panel::Free()
{
	__super::Free();
}
