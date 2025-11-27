#include "Mon_HP.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Mon_Gague.h"

CMon_HP::CMon_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
   : CUI_Panel{ pDevice, pContext }
{
}

CMon_HP::CMon_HP(const CMon_HP& Prototype)
   : CUI_Panel(Prototype)
{
}

void CMon_HP::Setting_HP(const _float4* pTagetMat, _float2 vOffset, const _float* pHpValue, const _float* pHpMaxValue, const _float* pStaminaCulValue, const _float* pStaminaMaxValue)
{
    m_pTargetPos = pTagetMat;
   m_vLocalPos = { vOffset.x, -vOffset.y };

   m_pHPGauge->Setting_Progress(pHpValue, pHpMaxValue);
   m_pStaminaGauge->Setting_Progress(pStaminaCulValue, pStaminaMaxValue);
}

HRESULT CMon_HP::Initialize_Prototype(_uint iLevel)
{
   m_iLevel = iLevel;
   CHECK_FAILED(Ready_Prototype(),E_FAIL);
   return S_OK;
}

HRESULT CMon_HP::Initialize_Clone(void* pArg)
{
   m_iTexPass = 0;
   m_iShaderPass = 1;
   m_vColor = { 1.f,1.f,1.f, 0.8f };
   m_fAlpha = 1.f;

   CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
   CHECK_FAILED(Ready_Component(), E_FAIL);
   CHECK_FAILED(Ready_Children(), E_FAIL);

   return S_OK;
}
void CMon_HP::Priority_Update(_float fTimeDelta)
{
   __super::Priority_Update(fTimeDelta);
}

void CMon_HP::Update(_float fTimeDelta)
{
   Update_WorldPos();
   __super::Update(fTimeDelta);
}

void CMon_HP::Late_Update(_float fTimeDelta)
{
   if (!m_isVisible || !m_isActive)
      return;

   CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
   __super::Late_Update(fTimeDelta);
}

HRESULT CMon_HP::Render()
{
   if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
      return E_FAIL;

   if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
      return E_FAIL;

   if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
      return E_FAIL;

   if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
      return E_FAIL;

   CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
   CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
   
   m_pShaderCom->Begin(m_iShaderPass);
   m_pVIBufferCom->Bind_Resources();
   m_pVIBufferCom->Render();

   return S_OK;
}

void CMon_HP::Reset()
{
    m_pTargetPos = nullptr;
   m_pHPGauge->Reset_Progress();
   m_pStaminaGauge->Reset_Progress();
}

HRESULT CMon_HP::Ready_Prototype()
{
   if (FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Mon_Gague"),
      CMon_Gauge::Create(m_pDevice, m_pContext))))
      return E_FAIL;

   return S_OK;
}

HRESULT CMon_HP::Ready_Component()
{
   if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
      TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
      return E_FAIL;

   if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
      TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
      return E_FAIL;

   if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hud_HPGauge_BG"),
      TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
      return E_FAIL;

   return S_OK;
}

HRESULT CMon_HP::Ready_Children()
{
   CMon_Gauge::MONGAUGE_DESC Desc;

   Desc.fDepth = m_fDepth - 0.1f;
   Desc.iUIType = ENUM_CLASS(UITYPE::PROGRESSBAR);
   Desc.szName = "HP";
   Desc.vLocalPos = _float2{ 0.f, -3.f };
   Desc.vLocalSize = { 100.f, 6.f };
   Desc.iTexPass = 1;
   Desc.iShaderPass = 8;
   m_pHPGauge = static_cast<CMon_Gauge*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_Mon_Gague"), &Desc));

   if (m_pHPGauge == nullptr)
      return E_FAIL;
   m_Children.push_back(m_pHPGauge);
   Safe_AddRef(m_pHPGauge);


   Desc.szName = "Stamina";
   Desc.vLocalPos = _float2{ 0.f, 5.f };
   Desc.vLocalSize = { 100.f, 4.f };
   Desc.iTexPass = 2;
   Desc.iShaderPass = 7;
   m_pStaminaGauge = static_cast<CMon_Gauge*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iLevel, TEXT("Prototype_GameObject_UI_Mon_Gague"), &Desc));

   if (m_pStaminaGauge == nullptr)
      return E_FAIL;
   m_Children.push_back(m_pStaminaGauge);
   Safe_AddRef(m_pStaminaGauge);

   return S_OK;
}

void CMon_HP::Update_WorldPos()
{
	_float4 vTemp = *m_pTargetPos;
 	_vector vTargetPos = XMLoadFloat4(&vTemp);

   vTemp = CClientInstance::GetInstance()->Get_ActiveCameraLook();
   _vector vCamLook = XMLoadFloat4(&vTemp);
   vCamLook = XMVector3Normalize(vCamLook);

   _float3 vDest = CClientInstance::GetInstance()->Get_ActiveCameraPos();
   _vector vCamPos = XMLoadFloat3(&vDest);

   _vector vDir = XMVector3Normalize(vTargetPos - vCamPos);

   _float fDot = XMVectorGetX(XMVector3Dot(vCamLook, vDir));

   if (fDot <= 0)
      m_isActive = false;
   else
      m_isActive = true;

   _matrix OldVeiw = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
   _matrix OldProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

   _vector vWinPos{};
   _matrix vVPMatrix = OldVeiw * OldProj;

   vWinPos = XMVector3TransformCoord(vTargetPos, vVPMatrix);

   _float fWinPosX = (XMVectorGetX(vWinPos) + 1.f) * 0.5f * g_iWinSizeX;
   _float fWinPosY = (1.f - XMVectorGetY(vWinPos)) * 0.5f * g_iWinSizeY;

   m_vWorldPos.x = fWinPosX + m_vLocalPos.x;
   m_vWorldPos.y = fWinPosY + m_vLocalPos.y;

   if (m_vWorldPos.x < 0)
      m_vWorldPos.x = 0;
   else if (m_vWorldPos.x > g_iWinSizeX)
      m_vWorldPos.x = g_iWinSizeX;

   if (m_vWorldPos.y < 0)
      m_vWorldPos.y = 0;
   else if (m_vWorldPos.y > g_iWinSizeY)
      m_vWorldPos.y = g_iWinSizeY;


   Update_Transform(nullptr, m_vWorldPos);
}

CMon_HP* CMon_HP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
   CMon_HP* pInstance = new CMon_HP(pDevice, pContext);
   if (FAILED(pInstance->Initialize_Prototype(iLevel)))
   {
      MSG_BOX(TEXT("Failed Created : CMon_HP"));
      Safe_Release(pInstance);
   }
   return pInstance;
}

CGameObject* CMon_HP::Clone(void* pArg)
{
   CMon_HP* pInstance = new CMon_HP(*this);
   if (FAILED(pInstance->Initialize_Clone(pArg)))
   {
      MSG_BOX(TEXT("Failed Cloned : CMon_HP"));
      Safe_Release(pInstance);
   }
   return pInstance;
}

void CMon_HP::Free()
{
   __super::Free();
   Safe_Release(m_pStaminaGauge);
   Safe_Release(m_pHPGauge);
   Safe_Release(m_pShaderCom);
   Safe_Release(m_pTextureCom);
   Safe_Release(m_pVIBufferCom);

}
