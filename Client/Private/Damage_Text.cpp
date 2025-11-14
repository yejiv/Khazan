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
	m_vUV.resize(1);
	m_iTexPass = 3;
	m_iShaderPass = 4;
	m_isPool = true;

	m_fAccTime = 1.f;
	m_fAlpha = 1.f;
	return S_OK;
}

void CDamage_Text::Priority_Update(_float fTimeDelta)
{
}

void CDamage_Text::Update(_float fTimeDelta)
{
	m_fAccTime -= fTimeDelta;
	
	if (m_fAccTime <= 0.f)
		m_isDead = true;
}

void CDamage_Text::Late_Update(_float fTimeDelta)
{
	Update_WolrdPos(XMLoadFloat4(&m_vWorldTranslation));
	if (!m_isVisible)
		return;
	for (_int i = 0; i < m_iLength; ++i)
	{
		m_vUV[0] = m_vDamage_UV[i];
		Offset_Pos(i, m_iLength);
		CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::ATLAS, this);
	}
}

HRESULT CDamage_Text::Render()
{

	return S_OK;
}

_bool CDamage_Text::Render_Damage(DAMAGE_TYPE eDamageType, _vector vPos, _uint iDamage, _float2 vOffset)
{
	if (eDamageType == DAMAGE_TYPE::END && iDamage < 0)
		return false;
	m_vLocalPos.x = vOffset.x;
	m_vLocalPos.y = -vOffset.y;
	m_iDamage = iDamage;
	m_eDamageType = eDamageType;

	if (eDamageType == DAMAGE_TYPE::DEFAULT)
	{
		m_vLocalSize = { 22.f, 22.f };
		Update_Scaling(1.f);
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else if (eDamageType == DAMAGE_TYPE::BACK)
	{
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else if (eDamageType == DAMAGE_TYPE::SPECIAL)
	{
		m_fAccTime = 2.f;
		m_vColor = { 1.f, 1.f, 1.f, 1.f };
	}
	else if (eDamageType == DAMAGE_TYPE::PLAYER)
	{
		m_vLocalSize = { 22.f, 22.f };
		Update_Scaling(1.f);
		m_vColor = { 1.f, 0.f, 0.f, 1.f };
	}

	string strDamage = to_string(iDamage);
	m_iLength = strDamage.length();
    m_vDamage_UV.resize(m_iLength);
	for (_int i = 0; i < m_iLength; ++i)
	{
        _float4 vUV = Mapping_Number(strDamage[i] - '0');
        m_vDamage_UV[i] = vUV;
	}

	XMStoreFloat4(&m_vWorldTranslation, vPos);

	return true;
}

void CDamage_Text::Reset()
{
	m_fAccTime = 1.f;
	m_fAlpha = 1.f;
	m_vDamage_UV.clear();
	m_vLocalPos = {};
    m_iLength = 0;
}

void CDamage_Text::Update_WolrdPos(_vector vPos)
{
	_float4 vTemp = CClientInstance::GetInstance()->Get_ActiveCameraLook();
	_vector vCamLook = XMLoadFloat4(&vTemp);
	vCamLook = XMVector3Normalize(vCamLook);

	_float3 vDest = CClientInstance::GetInstance()->Get_ActiveCameraPos();
	_vector vCamPos = XMLoadFloat3(&vDest);

	_vector vDir = XMVector3Normalize(vPos - vCamPos);

	_float fDot = XMVectorGetX(XMVector3Dot(vCamLook, vDir));

	if (fDot <= 0)
	{
		m_isVisible = false;
		return;
	}
	else
		m_isVisible = true;

	_matrix OldVeiw = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix OldProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

	_vector vWinPos{};
	_matrix vVPMatrix = OldVeiw * OldProj;

	vWinPos = XMVector3TransformCoord(vPos, vVPMatrix);

	_float fWinPosX = (XMVectorGetX(vWinPos) + 1.f) * 0.5f * g_iWinSizeX;
	_float fWinPosY = (1.f - XMVectorGetY(vWinPos)) * 0.5f * g_iWinSizeY;

	m_vCenterPos.x = fWinPosX + m_vLocalPos.x;
	m_vCenterPos.y = fWinPosY + m_vLocalPos.y;

}

void CDamage_Text::Offset_Pos(_int iIndex, _int iMaxIndex)
{
	if (m_eDamageType == DAMAGE_TYPE::DEFAULT || m_eDamageType == DAMAGE_TYPE::PLAYER)
	{
		m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f) / 2;
		m_vWorldPos.y = m_vCenterPos.y + m_fAccTime * 30.f;
		m_fAlpha = m_fAccTime;
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - g_iWinSizeX * 0.5f, -m_vWorldPos.y + g_iWinSizeY * 0.5f, 0.0f, 1.0f));
	}
	else if (m_eDamageType == DAMAGE_TYPE::BACK)
	{
		if (m_fAccTime >= 0.95f)
		{
			m_vLocalSize.x = 32.f * (0.5f + m_fAccTime / 0.5f);
			m_vLocalSize.y = 32.f * (0.5f + m_fAccTime / 0.5f);

			Update_Scaling(1.f);
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f + 1.f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f + 1.f) / 2;
			m_vWorldPos.y = m_vCenterPos.y + m_fAccTime * 3.f;// +(m_fAccTime - 1.3f) * 30.f;
		}
		else if (m_fAccTime >= 0.5f)
		{
			m_vLocalSize.x = 32.f;
			m_vLocalSize.y = 32.f;

			Update_Scaling(1.f);
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f) / 2;
			m_vWorldPos.y = m_vCenterPos.y - m_fAccTime * 1.5f;
		}
		else
		{
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f) / 2;
			m_vWorldPos.y = m_vCenterPos.y - m_fAccTime * 1.5f;
			m_fAlpha = m_fAccTime / 0.5f;
		}

		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - g_iWinSizeX * 0.5f, -m_vWorldPos.y + g_iWinSizeY * 0.5f, 0.0f, 1.0f));
	}
	else if (m_eDamageType == DAMAGE_TYPE::SPECIAL)
	{
		if (m_fAccTime >= 1.7f)
		{
			_float t = Clamp((m_fAccTime - 1.7f) / 0.3f);
			_float fScale = (exp(-5.f * t)) / (1.f - exp(-5.f));
			if (fScale < 0.1f)
				fScale = 0.1f;
			if (fScale >= 1.f)
				fScale = 1.f;
			m_vLocalSize.x = 64.f * fScale;
			m_vLocalSize.y = 64.f * fScale;

			Update_Scaling(1.f);
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f + 1.f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f + 1.f) / 2;
			m_vWorldPos.y = m_vCenterPos.y;// +(m_fAccTime - 1.3f) * 30.f;
		}
		else if (m_fAccTime > 1.3f)
		{
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f + 1.f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f + 1.f) / 2;
			m_vWorldPos.y = m_vCenterPos.y;
			m_fAlpha = 1.f;
		}
		else if (m_fAccTime >= 0.7f)
		{
			//_float t = Clamp((1.f - m_fAccTime) / 0.5f);
			//_float fScale = (exp(-4.f * t)) / (1.f - exp(-4.f));
			
			_float fScale = (m_fAccTime - 1.f) / 0.5f;
			if (fScale < 0.5f)
				fScale = 0.5f;
			if (fScale >= 1.f)
				fScale = 1.f;
			m_vLocalSize.x = 64.f * fScale;
			m_vLocalSize.y = 64.f * fScale;
			Update_Scaling(1.f);
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f + 1.f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f + 1.f) / 2;
			m_vWorldPos.y = m_vCenterPos.y;
			m_fAlpha = fScale;

		}
		else
		{
			m_vWorldPos.x = m_vCenterPos.x + iIndex * (m_vLocalSize.x * 0.6f + 1.f) - (iMaxIndex - 1) * (m_vLocalSize.x * 0.6f + 1.f) / 2;
			m_vWorldPos.y = m_vCenterPos.y;
			m_fAlpha = m_fAccTime * 0.5f;
		}
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - g_iWinSizeX * 0.5f, -m_vWorldPos.y + g_iWinSizeY * 0.5f, 0.0f, 1.0f));
	}
}

_float4 CDamage_Text::Mapping_Number(_int iNumber)
{
	string strNum = {};

	if (iNumber == 0) strNum = "T_icon_Key_0.png";
	else if (iNumber == 1) strNum = "T_Icon_Key_1.png";
	else if (iNumber == 2) strNum = "T_Icon_Key_2.png";
	else if (iNumber == 3) strNum = "T_Icon_Key_3.png";
	else if (iNumber == 4) strNum = "T_Icon_Key_4.png";
	else if (iNumber == 5) strNum = "T_icon_Key_5.png";
	else if (iNumber == 6) strNum = "T_icon_Key_6.png";
	else if (iNumber == 7) strNum = "T_icon_Key_7.png";
	else if (iNumber == 8) strNum = "T_icon_Key_8.png";
	else if (iNumber == 9) strNum = "T_icon_Key_9.png";
	else return { 0.f,0.f,1.f,1.f };

    
	return CClientInstance::GetInstance()->Get_AtlasUV(strNum, 3);
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
