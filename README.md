# 퍼스트 버서커: 카잔

<div align="center">
  <video src="https://i.imgur.com/Cb4OPB1.mp4" width="800" autoplay loop muted playsinline></video>
  <br><br>
  👇 <b>유튜브 영상 바로가기</b>
  <br>
  <a href="https://youtu.be/OJpX6eox33w">https://youtu.be/OJpX6eox33w</a>
</div>

<br/><br/>

## 📌 프로젝트 개요
- **DirectX11 기반 '퍼스트 버서커: 카잔' 3D 모작**
- **DLL 프레임워크 구현 프로젝트**로, 클라이언트와 분리된 구조를 바탕으로 시스템 이펙트 및 렌더링 최적화를 중점적으로 구현했습니다.
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

> [!warning]
> [Build 주의사항]
> 
> 1. DirectX SDK 경로를 환경 변수로 설정 (DXSDK_DIR)
> 2. Engine 프로젝트를 먼저 빌드하여 DLL이 생성된 후, Client에서 참조 필요

<br/><br/>

## 🛠️ 주요 구현 내용 (탁예지)

### 1. Effect System & Tool

**Data-Driven 이펙트 툴 및 타임트랙 시스템**
* 하나의 이펙트 프리팹이 다양한 형태의 이펙트 객체들을 타임트랙으로 관리하도록 구조화했다.
* 이펙트의 종류, 실행 시점, 동작 정보를 시간 기준으로 제어하는 이벤트 시스템을 구축하여 여러 이펙트가 흐름에 따라 순차 재생되도록 구현했다.
* 에디터 상에서 파티클의 개수, 범위, 위치, 사이즈, 수명, 색상, 텍스처 등을 직관적으로 조절할 수 있도록 구성했다.

**Advanced Shader Effects**
* **UV Scrolling:** 디퓨즈 텍스처를 이동시켜 흐르는 효과를 연출했으며, 특정 매쉬의 UV 데이터 구조에 맞춰 축(Axis) 방향을 의도적으로 스왑하여 스크롤링 로직이 정상 작동하도록 처리했다. 마스크 텍스처 조작을 통해 이펙트 노출 영역과 타이밍을 제어한다.
* **Stretched & Gravity Particle:** 파티클 이동 벡터 기준 Up 벡터를 구성하고 카메라 방향과의 외적을 통해 궤적을 강조하는 스트레치 파티클을 구현했다. 중력 가속도를 누적하여 무게감을 표현했으며, `fDecreaseAlpha` 변수를 적용해 파티클 수명(Life-time)에 따른 자연스러운 알파 감쇠 로직을 구현했다.
* **Fresnel Shader:** 픽셀의 법선 벡터와 카메라 방향 벡터를 내적하고 절대값을 적용하여, 양방향에서 매쉬 가장자리가 자연스럽게 강조되도록 처리했다.
* **Turbulence:** `Compute Shader` 단계에서 노이즈 텍스처를 샘플링해 난기류를 구현했다. 월드 좌표의 3차원 축 조합(yz, xz, xy)을 사용해 파티클들이 개별적으로 흔들리면서도 전체적인 흐름을 유지하도록 구성했다.
* **Dissolve:** 시간에 따른 Dissolve 값과 노이즈 텍스처를 픽셀 셰이더에서 비교하여 단계적 픽셀 `discard`를 수행했다. Edge 영역에 별도의 색상과 두께를 주어 소멸 경계의 불타는 연출을 추가했다.

### 2. Rendering Optimization

**Point / Mesh Instancing**
* 파티클 렌더링 시 동일한 지오메트리를 공유하도록 메쉬/포인트 인스턴싱을 적용했다. 인스턴스 데이터를 GPU 버퍼로 일괄 전달하여 파티클 수효와 무관하게 드로우 콜(Draw Call)을 최소화했다.

**Order-Independent Transparency (Weight Blend)**
* **문제:** 파티클 증가에 따른 기존 Z-Sorting의 CPU 정렬 비용 증가 및 시각적 아티팩트 발생.
* **해결:** 픽셀이 화면에 미치는 영향을 계산하는 `Weight Blend` 기법을 도입했다. 각 파티클의 색상과 알파 값을 깊이에 따른 가중치로 변환해 픽셀 단위 누적 블렌딩을 수행함으로써, 정렬 비용 없이 투명 레이어링 효과를 안정적으로 표현했다.

**Compute Shader & Double Staging Buffer**
* **문제:** 수백 개의 파티클 상태를 CPU에서 접근해 업데이트할 경우 발생하는 병목 현상.
* **해결:** 파티클 로직을 렌더링과 독립된 `Compute Shader` 패스로 옮겨 GPU 병렬 처리를 수행했다. 또한, 동기화 대기 시간을 없애기 위해 2개의 Staging Buffer를 설계하여 '이번 프레임에 요청한 데이터를 다음 프레임에 읽도록' 파이프라인을 구성, 프레임 드랍을 효과적으로 방지했다.

### 3. Trail System

**Mesh Trail & Line Trail**
* 무기 궤적 표현을 위한 트레일 클래스와 전용 버퍼 컴포넌트를 설계했다. 보간을 적용해 부드러운 경로를 샘플링하고 GPU 정점 버퍼 생성을 컴포넌트 단계로 위임했다.
* `Mesh Trail`이 두 개의 월드 좌표로 메쉬를 구성한다면, `Line Trail`은 단일 중심 경로에 offset을 적용해 정점을 구성하도록 로직을 재사용 및 분리했다.

**Screen Trail 적용 및 셰이더 분리**
* 기존 `Line Trail`의 로직을 기반으로 버퍼 컴포넌트의 입력을 함수 오버로딩으로 확장했다. 동일한 데이터를 사용하되 셰이더 패스만 분리하여 좌표계 변환을 제어했으며, U 좌표 기준 알파 페이딩으로 궤적이 점진적으로 소멸하도록 구현했다.
