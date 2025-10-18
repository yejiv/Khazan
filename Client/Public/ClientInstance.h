#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CClientInstance final : public CBase
{
	DECLARE_SINGLETON(CClientInstance)
private:
	CClientInstance();
	virtual ~CClientInstance() = default;

#pragma region CLIENT
public:
	HRESULT Initialize(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext);
	void Update(_float fTimeDelta);
#pragma endregion

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

NS_END
