#pragma once

#include "Editor_Defines.h"
#include "Prop.h"

// 정적 맵 오브젝트 ( 인스턴싱 O )

NS_BEGIN(Editor)

class CProp_Export final : public CProp
{
public:
	typedef struct tagPropExportDesc : public CProp::PROP_DESC
	{

	}PROP_EXPORT_DESC;

private:
	CProp_Export(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp_Export(const CProp_Export& Prototype);
	virtual ~CProp_Export() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CEditor_Model* m_pModelCom = { nullptr };

	string m_strExportPath = { "../../Client/Bin/Data/" };

private:
	HRESULT Ready_Components(void* pArg);

public:
	static CProp_Export* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END