#include "Jolt_DebugRenderer.h"

#include "DebugDraw.h"
#include "GameInstance.h"

CJolt_DebugRenderer::CJolt_DebugRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }, m_pContext{ pContext },
	m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode;
	size_t iShaderCodeLength;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderCodeLength);

	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderCodeLength, &m_pInputLayout);
}

CJolt_DebugRenderer::~CJolt_DebugRenderer()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

void CJolt_DebugRenderer::BeginFrame()
{
	if (m_bBatchOpen) return; // 중복 방지

	// View/Proj는 보통 프레임마다 1회만 바뀌므로 여기서 세팅
	m_matView = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	m_matProj = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

	m_pEffect->SetWorld(m_matWorldIdent);
	m_pEffect->SetView(m_matView);
	m_pEffect->SetProjection(m_matProj);

	// 파이프라인 고정 세팅 (이펙트/레이아웃)
	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	// PrimitiveBatch 오픈 (이 구간 동안 DrawLine은 매우 싸짐)
	m_pBatch->Begin();
	m_bBatchOpen = true;
}

void CJolt_DebugRenderer::EndFrame()
{
	if (!m_bBatchOpen) return;
	m_pBatch->End();
	m_bBatchOpen = false;
}

void CJolt_DebugRenderer::DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor)
{
	/*Float3 FromPos{}, ToPos{};
	inFrom.StoreFloat3(&FromPos);
	inTo.StoreFloat3(&ToPos);
	_float3 vFromPos{}, vToPos{};
	memcpy(&vFromPos, &FromPos, sizeof(_float3));
	memcpy(&vToPos, &ToPos, sizeof(_float3));

	_float4 vColor = _float4(inColor.r, inColor.g, inColor.b, inColor.a);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();
	m_pBatch->DrawLine(
		VertexPositionColor(vFromPos, vColor),
		VertexPositionColor(vToPos, vColor)
	);
	m_pBatch->End();*/

	// 여기서는 파이프라인/이펙트 세팅을 절대 하지 않음!
	// (BeginFrame에서 이미 완료됨)

	// 좌표 변환 (불필요한 memcpy 최소화)
	Float3 fromF3{}, toF3{};
	inFrom.StoreFloat3(&fromF3);
	inTo.StoreFloat3(&toF3);

	_float3 vFromPos = _float3(fromF3.x, fromF3.y, fromF3.z);
	_float3 vToPos = _float3(toF3.x,   toF3.y,   toF3.z);

	_float4 vColor = _float4(inColor.r, inColor.g, inColor.b, inColor.a);

	// 배치가 열려있다는 가정 (안 열려있으면 방어적으로 열어도 됨)
	// assert(m_bBatchOpen && "Call BeginFrame() before DrawLine()");

	m_pBatch->DrawLine(
		VertexPositionColor(vFromPos, vColor),
		VertexPositionColor(vToPos, vColor)
	);
}

void CJolt_DebugRenderer::DrawText3D(RVec3Arg inPosition, const string_view& inString, ColorArg inColor, float inHeight)
{
}
