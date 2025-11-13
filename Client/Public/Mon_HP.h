#pragma once
#include "UI_Panel.h"
#include "Client_Defines.h"

NS_BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)
class CMon_HP final : public CUI_Panel
{
private:
   CMon_HP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
   CMon_HP(const CMon_HP& Prototype);
   virtual ~CMon_HP() = default;

public:
   void               Setting_HP(const _float4* pTagetMat, _float2 vOffset, const _float* pHpValue, const _float* pHpMaxValue, const _float* pStaminaCulValue, const _float* pStaminaMaxValue);
         
public:
   virtual HRESULT         Initialize_Prototype(_uint iLevel);
   virtual HRESULT         Initialize_Clone(void* pArg) override;
   virtual void         Priority_Update(_float fTimeDelta) override;
   virtual void         Update(_float fTimeDelta) override;
   virtual void         Late_Update(_float fTimeDelta) override;
   virtual HRESULT         Render() override;

   virtual void         Reset() override;
private:
   CShader*            m_pShaderCom = { nullptr };
   CTexture*            m_pTextureCom = { nullptr };
   CVIBuffer_Rect*         m_pVIBufferCom = { nullptr };

   class CMon_Gauge*      m_pHPGauge = { nullptr };
   class CMon_Gauge*      m_pStaminaGauge = { nullptr };

    const _float4*          m_pTargetPos = { nullptr };

private:
   HRESULT               Ready_Prototype();
   HRESULT               Ready_Component();
   HRESULT               Ready_Children();


   void               Update_WorldPos();
public:
   static CMon_HP*         Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
   virtual CGameObject*   Clone(void* pArg) override;
   virtual void         Free() override;
};

NS_END