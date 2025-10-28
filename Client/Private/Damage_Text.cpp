#include "Damage_Text.h"
#include "GameInstance.h"
#include "ClientInstance.h"

CDamage_Text::CDamage_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Text(pDevice, pContext)
{
}

CDamage_Text::CDamage_Text(const CDamage_Text& Prototype)
	: CUI_Text(Prototype)
{
}

HRESULT CDamage_Text::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDamage_Text::Initialize_Clone(void* pArg)
{
	__super::Initialize_Clone(pArg);
	return S_OK;
}

void CDamage_Text::Priority_Update(_float fTimeDelta)
{
}

void CDamage_Text::Update(_float fTimeDelta)
{
}

void CDamage_Text::Late_Update(_float fTimeDelta)
{
	CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
}

HRESULT CDamage_Text::Render()
{

	return S_OK;
}

_bool CDamage_Text::Render_Damage(DAMAGE_TYPE eDamageType, _vector vPos, _uint iDamage)
{
	if (eDamageType == DAMAGE_TYPE::END && iDamage < 0)
		return false;

	m_iDamage = iDamage;

	if (eDamageType == DAMAGE_TYPE::DEFAULT)
	{
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else if (eDamageType == DAMAGE_TYPE::BACK)
	{
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else if (eDamageType == DAMAGE_TYPE::SPECIAL)
	{
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else if (eDamageType == DAMAGE_TYPE::PLAYER)
	{
		m_vColor = { 1.f, 0.f, 0.f, 1.f };
	}



	_matrix OldVeiw = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix OldProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

	_vector vWinPos{};
	_matrix vVPMatrix = OldVeiw * OldProj;

	vWinPos = XMVector3TransformCoord(vPos, vVPMatrix);

	_float fWinPosX = (XMVectorGetX(vWinPos) + 1.f) * 0.5f * g_iWinSizeX;
	_float fWinPosY = (1.f - XMVectorGetY(vWinPos)) * 0.5f * g_iWinSizeY;

	m_vWorldPos.x = fWinPosX + m_vLocalPos.x;
	m_vWorldPos.y = fWinPosY + m_vLocalPos.y;
	return _bool();
}

void CDamage_Text::Reset()
{
}

void CDamage_Text::Update_WolrdPos(_vector vPos)
{
	_matrix OldVeiw = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix OldProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

	_vector vWinPos{};
	_matrix vVPMatrix = OldVeiw * OldProj;

	vWinPos = XMVector3TransformCoord(vPos, vVPMatrix);

	_float fWinPosX = (XMVectorGetX(vWinPos) + 1.f) * 0.5f * g_iWinSizeX;
	_float fWinPosY = (1.f - XMVectorGetY(vWinPos)) * 0.5f * g_iWinSizeY;

	m_vWorldPos.x = fWinPosX + m_vLocalPos.x;
	m_vWorldPos.y = fWinPosY + m_vLocalPos.y;
}

CDamage_Text* CDamage_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDamage_Text* pInstance = new CDamage_Text(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CDamage_Text"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDamage_Text::Clone(void* pArg)
{
	CDamage_Text* pInstance = new CDamage_Text(*this);
	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CDamage_Text"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDamage_Text::Free()
{
	__super::Free();
}
