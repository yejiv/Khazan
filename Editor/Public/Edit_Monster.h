#pragma once
#include "Editor_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CAI_Controller;
NS_END


NS_BEGIN(Editor)

class CEdit_Monster final : public CContainerObject
{

private:
	CEdit_Monster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Monster(const CEdit_Monster& Prototype);
	virtual ~CEdit_Monster() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }

private:
	HRESULT						Ready_Components();

private:
	CAI_Controller*				m_pController = { nullptr };


public:
	static CEdit_Monster*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

NS_END