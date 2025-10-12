#ifndef Engine_Function_h__
#define Engine_Function_h__

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






	// JPH 유틸함수
	static inline Vec3 LoadVec3(const _float3& vVector) { return Vec3(vVector.x, vVector.y, vVector.z); }
	static inline Vec3 LoadVec3(const _fvector& vVector) { return Vec3(vVector.m128_f32[0], vVector.m128_f32[1], vVector.m128_f32[2]); }

	static inline Quat LoadQuat(const _float4& vQuat) { return Quat(vQuat.x, vQuat.y, vQuat.z, vQuat.w); }
	static inline Quat LoadQuat(const _fvector& vQuat) { return Quat(vQuat.m128_f32[0], vQuat.m128_f32[1], vQuat.m128_f32[2], vQuat.m128_f32[3]); }
}

#endif // Engine_Function_h__
