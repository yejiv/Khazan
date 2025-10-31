#ifndef Engine_Function_h__
#define Engine_Function_h__

//#include <bit>

namespace Engine
{
	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();

			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long		dwRefCnt = 0;

		if (nullptr != pInstance)
			dwRefCnt = pInstance->AddRef();
		return dwRefCnt;
	}

	inline string WStringToAnsi(const std::wstring& w, UINT codepage = CP_ACP)
	{
		if (w.empty()) return {};
		const int size = WideCharToMultiByte(codepage, 0,
			w.c_str(), (int)w.size(),
			nullptr, 0, nullptr, nullptr);
		string out(size, '\0');
		WideCharToMultiByte(codepage, 0,
			w.c_str(), (int)w.size(),
			out.data(), size, nullptr, nullptr);
		return out;
	}

	inline std::wstring AnsiToWString(const std::string& s, UINT codepage = CP_ACP)
	{
		if (s.empty()) return {};
		const int size = MultiByteToWideChar(codepage, 0,
			s.c_str(), (int)s.size(),
			nullptr, 0);
		std::wstring out(size, L'\0');
		MultiByteToWideChar(codepage, 0,
			s.c_str(), (int)s.size(),
			out.data(), size);
		return out;
	}

	static inline float Length3(float x, float y, float z) {
		return sqrtf(x * x + y * y + z * z);
	}
	static inline void Normalize3(float& x, float& y, float& z) {
		float len = Length3(x, y, z);
		if (len > 1e-8f) { x /= len; y /= len; z /= len; }
	}
	static inline void Orthonormalize3x3(float& m00, float& m01, float& m02,
		float& m10, float& m11, float& m12,
		float& m20, float& m21, float& m22) {
		// Gram-Schmidt: X축 정규화 → Y에서 X 성분 제거 후 정규화 → Z = X×Y
		Normalize3(m00, m01, m02);
		// Y = Y - dot(Y,X)*X
		float dotYX = m10 * m00 + m11 * m01 + m12 * m02;
		m10 -= dotYX * m00; m11 -= dotYX * m01; m12 -= dotYX * m02;
		Normalize3(m10, m11, m12);
		// Z = X × Y
		m20 = m01 * m12 - m02 * m11;
		m21 = m02 * m10 - m00 * m12;
		m22 = m00 * m11 - m01 * m10;
	}
	static inline _float4 QuaternionFrom3x3(float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22) {
		// 쿼터니언 (x,y,z,w) 생성 - 안정적 trace 방식
		float trace = m00 + m11 + m22;
		float x, y, z, w;
		if (trace > 0.0f) {
			float s = sqrtf(trace + 1.0f) * 2.0f; // s = 4*w
			w = 0.25f * s;
			x = (m21 - m12) / s;
			y = (m02 - m20) / s;
			z = (m10 - m01) / s;
		}
		else if (m00 > m11 && m00 > m22) {
			float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f; // s = 4*x
			w = (m21 - m12) / s;
			x = 0.25f * s;
			y = (m01 + m10) / s;
			z = (m02 + m20) / s;
		}
		else if (m11 > m22) {
			float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f; // s = 4*y
			w = (m02 - m20) / s;
			x = (m01 + m10) / s;
			y = 0.25f * s;
			z = (m12 + m21) / s;
		}
		else {
			float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f; // s = 4*z
			w = (m10 - m01) / s;
			x = (m02 + m20) / s;
			y = (m12 + m21) / s;
			z = 0.25f * s;
		}
		// 정규화
		float norm = sqrtf(x * x + y * y + z * z + w * w);
		if (norm > 1e-8f) { x /= norm; y /= norm; z /= norm; w /= norm; }
		return { x, y, z, w }; // _float4(x,y,z,w)
	}

	static inline bool DecomposeMatrixTR(const _float4x4& M, _float3* outPos, _float4* outQuat)
	{
		if (!outPos || !outQuat) return false;

		// 위치
		outPos->x = M._41; outPos->y = M._42; outPos->z = M._43;

		// 3x3 상단 행렬에서 스케일 제거
		float m00 = M._11, m01 = M._12, m02 = M._13;
		float m10 = M._21, m11 = M._22, m12 = M._23;
		float m20 = M._31, m21 = M._32, m22 = M._33;

		// 컬럼 길이로 스케일 계산
		float sx = Length3(m00, m10, m20);
		float sy = Length3(m01, m11, m21);
		float sz = Length3(m02, m12, m22);
		if (sx < 1e-8f || sy < 1e-8f || sz < 1e-8f) return false;

		// 스케일 제거(컬럼 나눗셈)
		m00 /= sx; m10 /= sx; m20 /= sx;
		m01 /= sy; m11 /= sy; m21 /= sy;
		m02 /= sz; m12 /= sz; m22 /= sz;

		// 수치오차 보정(직교화)
		Orthonormalize3x3(m00, m01, m02, m10, m11, m12, m20, m21, m22);

		// 오른손 좌표계 보정(필요 시): det < 0면 한 축 뒤집기
		float det = m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) + m02 * (m10 * m21 - m11 * m20);
		if (det < 0.0f) { m00 = -m00; m10 = -m10; m20 = -m20; }

		*outQuat = QuaternionFrom3x3(m00, m01, m02, m10, m11, m12, m20, m21, m22);
		return true;
	}

	// 클램프
	inline float Clamp(float fA)
	{
		if (fA < 0.0f)
			return 0.0f;
		else if (fA > 1.0f) 
			return 1.0f;
	}

	// 기본 Lerp (Unclamped): t가 0~1 밖이어도 그대로 계산
	inline float Lerp(float fA, float fB, float fT) noexcept
	{
		return fA + (fB - fA) * fT;
	}

	inline XMFLOAT4 Lerp(XMFLOAT4 fA, XMFLOAT4 fB, float fT) noexcept
	{
		fT = Clamp(fT);
		XMFLOAT4 vLerp = {};
		XMStoreFloat4(&vLerp, XMLoadFloat4(&fA) + (XMLoadFloat4(&fB) - XMLoadFloat4(&fA)) * fT);
		return vLerp;
	}

	// 안전 Lerp + 클램프 (t를 0~1로 제한)
	inline float LerpClamped(float fA, float fB, float fT) noexcept
	{
		fT = Clamp(fT);

		return fA + (fB - fA) * fT;
	}

	// JPH
	static inline Vec3 LoadVec3(const _float3& vVector) { return Vec3(vVector.x, vVector.y, vVector.z); }
	static inline Vec3 LoadVec3(const _fvector& vVector) { return Vec3(vVector.m128_f32[0], vVector.m128_f32[1], vVector.m128_f32[2]); }

	static inline Quat LoadQuat(const _float4& vQuat) { return Quat(vQuat.x, vQuat.y, vQuat.z, vQuat.w); }
	static inline Quat LoadQuat(const _fvector& vQuat) { return Quat(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]); }

	/* 오현 : iIndex 값에서 최하위 비트( "right" )부터 시작하여 연속된 0 비트의 개수를 반환 */
	inline constexpr unsigned int GetBitPosition(unsigned int iIndex)
	{
	/*	if (iIndex == 0) return -1;
		return std::countr_zero(iIndex);*/

		if (iIndex == 0) return static_cast<unsigned int>(-1);

		unsigned int pos = 0;
		while ((iIndex & 1u) == 0u)
		{
			iIndex >>= 1u;
			++pos;
		}
		return pos;
	}

	/* 오현 :  실수 value 값에서 정수부분 자리수를 반환*/
	inline int GetIntDigits(float value)
	{
		value = fabs(value);
		if (value < 1.f) return 1;
		return static_cast<int>(floor(log10(value))) + 1;
	}

	/* 오현 : const _char* -> _wstring */
	inline std::wstring CharToWString(const char* pStr) {
		if (!pStr || pStr[0] == '\0') return L"";

		// 필요한 버퍼 크기 계산
		int len = MultiByteToWideChar(CP_ACP, 0, pStr, -1, nullptr, 0);
		if (len <= 0) return L"";

		std::wstring result(len - 1, L'\0');
		MultiByteToWideChar(CP_ACP, 0, pStr, -1, &result[0], len);

		return result;
	}




	/* 정환 Jolt 유틸함수*/

// 선형 보간 [0,1]
	inline double LerpD(double a, double b, float t) {
		return (1.0 - t) * a + t * b;
	}

	// RVec3(=double) 보간
	inline JPH::RVec3 LerpRVec3(const JPH::RVec3& a, const JPH::RVec3& b, float t) {
		return JPH::RVec3(
			LerpD(a.GetX(), b.GetX(), t),
			LerpD(a.GetY(), b.GetY(), t),
			LerpD(a.GetZ(), b.GetZ(), t)
		);
	}

	// 단위 쿼터니언 정규화 (std::sqrt 사용)
	inline JPH::Quat NormalizeQuat(const JPH::Quat& q) {
		const float len2 = q.LengthSq();
		if (len2 <= 0.f) return JPH::Quat::sIdentity();
		const float invLen = 1.0f / std::sqrt(len2);
		return q * invLen;              // JPH::Quat * float 연산자 존재
	}

	// 로버스트 Slerp (anti-podal / 작은 각도 처리)
	inline JPH::Quat SlerpQuat(JPH::Quat a, JPH::Quat b, float t) {
		// (안전) 단위화
		a = NormalizeQuat(a);
		b = NormalizeQuat(b);

		float dot = a.Dot(b);

		// 반대쪽 반구 보정
		if (dot < 0.0f) {
			b = -b;
			dot = -dot;
		}

		// 각도 매우 작으면 nlerp 근사
		constexpr float kEps = 1e-5f;
		if (dot > 1.0f - kEps) {
			JPH::Quat q = NormalizeQuat(a * (1.0f - t) + b * t);
			return q;
		}

		// 표준 slerp
		dot = std::clamp(dot, -1.0f, 1.0f);
		const float theta = std::acos(dot);
		const float s = std::sin(theta);
		const float w1 = std::sin((1.0f - t) * theta) / s;
		const float w2 = std::sin(t * theta) / s;
		return a * w1 + b * w2;
	}


	static inline int Wrap(int i, int n) { return (i % n + n) % n; }
	static inline float DampAlpha(float smooth, float dt) {
		// exp(-smooth*dt) 기반 지수 스무딩
		return 1.f - expf(-smooth * dt);
	}

	static float ChordLenAlpha(XMVECTOR a, XMVECTOR b, float alpha = 0.5f) {
		float d = XMVectorGetX(XMVector3Length(a - b));
		d = (d < 1e-6f ? 1e-6f : d);
		return powf(d, alpha);
	}
	static XMVECTOR CatmullCR(XMVECTOR P0, XMVECTOR P1, XMVECTOR P2, XMVECTOR P3,
		float s01, float s12, float s23, float s) {
		float t0 = 0.f, t1 = t0 + s01, t2 = t1 + s12, t3 = t2 + s23;
		float u = t1 + s * (t2 - t1);

		auto L = [](XMVECTOR a, XMVECTOR b, float t) { return XMVectorLerp(a, b, t); };
		float d10 = (t1 - t0), d21 = (t2 - t1), d32 = (t3 - t2), d20 = (t2 - t0), d31 = (t3 - t1);
		d10 = max(d10, 1e-6f); d21 = max(d21, 1e-6f); d32 = max(d32, 1e-6f);
		d20 = max(d20, 1e-6f); d31 = max(d31, 1e-6f);

		XMVECTOR A1 = L(P0, P1, (u - t0) / d10), A2 = L(P1, P2, (u - t1) / d21), A3 = L(P2, P3, (u - t2) / d32);
		XMVECTOR B1 = L(A1, A2, (u - t0) / d20), B2 = L(A2, A3, (u - t1) / d31);
		return L(B1, B2, (u - t1) / max(t2 - t1, 1e-6f));
	}
	static XMVECTOR QuatFromLook(const XMFLOAT4& la) {
		XMVECTOR look = XMVector3Normalize(XMLoadFloat4(&la));
		if (XMVectorGetX(XMVector3Length(look)) < 1e-6f) look = XMVectorSet(0, 0, 1, 0);

		XMVECTOR up = XMVectorSet(0, 1, 0, 0);
		if (fabsf(XMVectorGetX(XMVector3Dot(up, look))) > 0.999f) up = XMVectorSet(1, 0, 0, 0);

		XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));
		up = XMVector3Normalize(XMVector3Cross(look, right));

		XMMATRIX R(
			XMVectorSet(XMVectorGetX(right), XMVectorGetY(right), XMVectorGetZ(right), 0),
			XMVectorSet(XMVectorGetX(up), XMVectorGetY(up), XMVectorGetZ(up), 0),
			XMVectorSet(XMVectorGetX(look), XMVectorGetY(look), XMVectorGetZ(look), 0),
			XMVectorSet(0, 0, 0, 1));
		return XMQuaternionNormalize(XMQuaternionRotationMatrix(R));
	}

}

#endif // Engine_Function_h__
