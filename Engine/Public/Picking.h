#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update(); /* 후처리 에서 그려준 픽킹을 위한 뎁스정보를 현재 텍스쳐에 복사해준다., */

	_bool isPicked(_float3* pOut);
	// 맵 오브젝트 ( Props ) MapObjectID 쉐이더에 바인딩한거 꺼내오는 용도의 함수 ( VtxMesh )
	_bool isPicked(_float3* pOut, _uint* iObjectID);

	_float4 isPickRenderTargetPixel(_wstring strRenderTargetTag);

private:
	HWND					m_hWnd = {};
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	_uint					m_iWinSizeX = {}, m_iWinSizeY = {};
	ID3D11Texture2D*		m_pTexture2D = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	_float4* m_pPixels = { nullptr };
	POINT					m_ptMouse = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;

};

NS_END