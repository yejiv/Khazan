# 퍼스트 버서커: 카잔

<div align="center">
   <img src="https://github.com/user-attachments/assets/21cd38ed-1671-4a37-8efd-da2371e03a44" width="800" alt="퍼스트 버서커: 카잔 실행 영상">
  <br><br>
  👇 <b>유튜브 영상 바로가기</b>
  <br>
  <a href="https://youtu.be/OJpX6eox33w">https://youtu.be/OJpX6eox33w</a>
</div>

<br/><br/>

## 📌 프로젝트 개요
- **DirectX11을 사용한 “퍼스트 버서커: 카잔” 3D 모작**
- **클라이언트 프레임워크 구현 및 별도의 프로젝트로 구성된 엔진 프로젝트를 DLL로 빌드하여 클라이언트에서 사용**
- **개발 기간 : 2025. 10. 01 ~ 2025. 12. 15**

<br/><br/>

## 👥 팀원 소개

<div align="center">

| [<img src="https://i.imgur.com/hcfixwC.png" width="180" alt="조정환">](https://github.com/lasval) | [<img src="https://imgur.com/3pJ6Tnb.png" width="180" alt="정오현">](https://github.com/dhgus110) | [<img src="https://i.imgur.com/kq4xJD3.png" width="180" alt="권성은">](https://github.com/kwon7330) | [<img src="https://i.imgur.com/RZlfARy.png" width="180" alt="박준영">](https://github.com/zuuun-0) |
| :---: | :---: | :---: | :---: |
| **\[팀장\]** 조정환 | 정오현 | 권성은 | 박준영 |
| 메인프레임, 카메라 | 애니메이션 | AI | 맵 |
| [<img src="https://imgur.com/yoldESr.png" width="60" alt="조정환 깃허브">](https://github.com/lasval) | [<img src="https://imgur.com/yoldESr.png" width="60" alt="정오현 깃허브">](https://github.com/dhgus110) | [<img src="https://imgur.com/yoldESr.png" width="60" alt="권성은 깃허브">](https://github.com/kwon7330) | [<img src="https://imgur.com/yoldESr.png" width="60" alt="박준영 깃허브">](https://github.com/zuuun-0) |

<br/>

| [<img src="https://i.imgur.com/DCdj845.png" width="180" alt="김범수">](https://github.com/kimbumsu0312) | [<img src="https://imgur.com/MmLkGDO.png" width="180" alt="탁예지">](https://github.com/yejiv) | [<img src="https://imgur.com/Dmve5hf.png" width="180" alt="이단비">](https://github.com/eksql010) |
| :---: | :---: | :---: |
| 김범수 | 탁예지 | 이단비 |
| UI, AI | 이펙트 | 셰이더 |
| [<img src="https://imgur.com/yoldESr.png" width="60" alt="김범수 깃허브">](https://github.com/kimbumsu0312) | [<img src="https://imgur.com/yoldESr.png" width="60" alt="탁예지 깃허브">](https://github.com/yejiv) [<img src="https://imgur.com/3yif98T.png" width="22" alt="탁예지 블로그">](https://velog.io/@yejiv/posts) | [<img src="https://imgur.com/yoldESr.png" width="60" alt="이단비 깃허브">](https://github.com/eksql010) |

</div>

<br/><br/>

## 💻 개발 환경

| IDE | Language | OS | Graphic API | Communication |
| :---: | :---: | :---: | :---: | :---: |
| ![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white) ![Visual Studio Code](https://img.shields.io/badge/VS%20Code-0078d7.svg?style=for-the-badge&logo=visual-studio-code&logoColor=white) | ![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![HLSL](https://img.shields.io/badge/HLSL-302E31?style=for-the-badge) | ![Windows](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white) | ![DirectX](https://img.shields.io/badge/DirectX-000000?style=for-the-badge&logo=windows&logoColor=white) | ![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white) ![Notion](https://img.shields.io/badge/Notion-%23000000.svg?style=for-the-badge&logo=notion&logoColor=white) ![Discord](https://img.shields.io/badge/Discord-%235865F2.svg?style=for-the-badge&logo=discord&logoColor=white) |

<br/><br/>

## 🛠️ 주요 구현 내용 (탁예지)

### Order-Independent Transparency (Weight Blend)
투명 오브젝트의 Z값 정렬 시 파티클 수가 증가할수록 CPU 소팅 비용이 커지고 시각적 아티팩트가 발생하는 한계를 해결하기 위해 설계했다. 깊이 정렬 없이 픽셀 단위 기여도를 계산하는 Weight Blended OIT 기법을 적용했다.

```hlsl
// Weight Blend Pixel Shader
float z = In.vProjPos.z / In.vProjPos.w;
vFinalColor.a *= exp(-z * 0.75f); // 지수 감쇠 형태의 가중치 계산

// RGB 채널: 가중 평균의 분자 / A 채널: 가중 평균의 분모 역할
Out.vAccumColor = float4(vFinalColor.rgb * vFinalColor.a, vFinalColor.a) * weight;
Out.vAccumAlpha = vFinalColor.a;  // 배경 노출도(Revealage) 
```
* **누적 렌더 타겟 (MRT)** — 가중치가 적용된 색상 누적용 버퍼(`RT_AccumColor`)와 배경 노출도 누적용 버퍼(`RT_AccumAlpha`)를 생성하여 병렬 출력.
* **가중치 (Weight) 계산** — 깊이(`z`)와 알파(`a`) 값을 기반으로 가중치를 부여해 전면에 위치한 객체가 상대적으로 자연스럽게 강조되도록 누적.
* 정렬 연산(Sorting)을 제거하여 수백 개 이상의 반투명 이펙트가 중첩되는 상황에서도 프레임 저하 없이 투명 레이어링 효과 구현.

### Double Staging Buffer (CPU-GPU 동기화 병목 완화)
Compute Shader에서 연산된 결과(Dead Flag 등)를 CPU 로직에서 참조할 때, `CopyResource` 직후 `Map`을 호출하면서 발생하는 GPU 작업 대기 및 프레임 드랍 현상을 해결했다.

```cpp
// 1. 이번 프레임 연산 데이터 복사 요청 (비동기)
m_pContext->CopySubresourceRegion(m_pStagingBuffer[m_iWriteIdx], 0, 0, 0, 0, m_pSpeedBuffer, 0, &m_SourceBox);

// 2. 이전 프레임에 요청했던 데이터 읽기 (병목 제거)
D3D11_MAPPED_SUBRESOURCE mappedResource;
if (SUCCEEDED(m_pContext->Map(m_pStagingBuffer[m_iReadIdx], 0, D3D11_MAP_READ, 0, &mappedResource))) {
    POINT_INSTANCE_SPEED_PARAMS* aliveCount = reinterpret_cast<POINT_INSTANCE_SPEED_PARAMS*>(mappedResource.pData);
    m_pContext->Unmap(m_pStagingBuffer[m_iReadIdx], 0);
    if (aliveCount->bDead) flag = true;
}

// 3. 프레임 종료 시 버퍼 인덱스 교체
swap(m_iReadIdx, m_iWriteIdx);
```
* `CopySubresourceRegion()` — 전체 버퍼 복사 대신 필요한 범위만 부분 복사 요청하여 메모리 전송 비용 최적화.
* `Map(D3D11_MAP_READ)` — 이전 프레임에 복사 요청한 버퍼(`m_iReadIdx`)를 타겟으로 지정하여, 대기 시간 없이 즉시 데이터 참조.
* CPU-GPU 병렬성 확보를 통해 동기화 병목현상 제거 및 안정적인 프레임 유지 달성 (FPS 39 ➔ 60 개선).

### Data-Driven Effect Tool & Instancing
* **이펙트 에디터 및 타임트랙** — 이펙트 프리팹이 지닌 하위 객체 속성(개수, 위치, 수명, 텍스처 등)을 에디터에서 직관적으로 파싱 및 수정 가능하도록 구성. 타임트랙 기반 이벤트 시스템을 적용해 이펙트들이 시간 흐름에 따라 순차 재생되도록 제어했다.
* **Point / Mesh Instancing** — 메쉬 및 포인트 파티클에 인스턴싱 기법을 적용, 단일 지오메트리를 공유하고 인스턴스 데이터를 GPU 버퍼로 일괄 전달하여 Draw Call을 최소화했다.

### Advanced Shader Effects
* **UV Scrolling & Turbulence** — 디퓨즈/마스크 텍스처를 스크롤링하여 시각적 흐름과 타이밍을 제어. Compute Shader 단계에서 3차원 축 조합(yz, xz, xy)으로 노이즈를 샘플링해 파티클의 난기류 움직임을 구현.
* **Stretched & Gravity Particle** — 이동 벡터 기준 Up 벡터와 카메라 외적을 통해 궤적을 강조하는 스트레치 형태 구성. 파티클별 속도 벡터에 중력 가속도를 누적 연산하여 물리적 무게감 표현.
* **Fresnel & Dissolve** — 픽셀 법선과 카메라 방향 내적을 통해 매쉬 가장자리를 강조(Fresnel). 노이즈 텍스처를 샘플링한 뒤 Dissolve 값과 픽셀 셰이더에서 비교하여 소멸 경계를 불타는 듯이 연출(`discard`).

### Trail System Architecture
* **Mesh & Line Trail** — 무기 궤적 생성을 위해 보간을 적용한 트레일 전용 버퍼 컴포넌트 설계. 2개의 월드 좌표를 사용하는 `Mesh` 방식과, 단일 중심 경로에 offset을 적용하는 `Line` 방식으로 정점 구성 로직을 분리 및 재사용.
* **Screen Trail** — 기존 Line Trail 로직을 바탕으로 함수 오버로딩과 셰이더 패스만 분리하여 공간 변환 처리. 픽셀 셰이더에서 U 좌표를 기준으로 알파 페이딩을 제어해 점진적으로 사라지는 궤적 구현.

<br/><br/>

> [!warning]
> [Build 주의사항]
> 
> 1. DirectX SDK 경로를 환경 변수로 설정 (DXSDK_DIR)
>
> 2. Engine 프로젝트를 먼저 빌드하여 DLL이 생성된 후, Client에서 참조 필요
