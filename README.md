# Project: BossRaid

### 5분마다 보스를 처치하며 무한히 도전하는 3인칭 FPS 로그라이크 게임

 <img width="1519" height="649" alt="스크린샷 2025-08-24 234322" src="https://github.com/user-attachments/assets/6ba3a7b0-b9cf-4d78-8c7d-2f4a37b6dc00" />

## 1. 프로젝트 개요
이 프로젝트는 개인 포트폴리오 목적으로 제작된 3인칭 FPS 로그라이크 게임입니다. 단순히 게임을 완성하는 것을 넘어, 아래와 같은 구체적인 기술 목표를 달성하는 데 중점을 두었습니다.

- **개발 기간:** 2025.06 ~ 2025.09
- **개발 인원:** 1인 개발
- **프로젝트 목표:**
    - Gameplay Ability System(GAS) 프레임워크의 깊이 있는 학습 및 실전 적용
    - 대규모 전투 상황에서의 런타임 퍼포먼스 최적화 기법 연구 및 적용
    - 기획/디자인 직군과의 협업 효율을 높이는 커스텀 툴 개발

## 2. 주요 기능
- GAS 기반의 데이터 기반 스킬/어빌리티 시스템
- 다양한 몬스터 패턴 및 보스전
- Slate 기반의 스킬트리 커스텀 에디터
- 멀티스레딩 및 오브젝트 풀링을 통한 런타임 최적화
- Behavior Tree 기반의 몬스터 AI

## 3. 기술 스택
- **Engine:** Unreal Engine 5.3
- **Language:** C++
- **Core Systems:** Gameplay Ability System(GAS), Slate UI, Behavior Tree, Multi-threading

---
## 4. 주요 구현 내용

### 1. AI 성능 최적화를 위한 멀티스레딩 적용
<img width="1252" height="669" alt="스크린샷 2025-08-27 023204" src="https://github.com/user-attachments/assets/925628d3-b04e-494a-9b2d-dc2435289abf" />

- **문제:** `BTService`에서 실행되는 타겟 탐색 로직이 게임 스레드에 큰 부하를 주어, AI 개체 수가 많아질수록 프레임 드랍이 발생했습니다.
- **해결:** `AsyncTask`를 활용하여 복잡한 타겟 탐색 연산을 워커 스레드로 분산시켰습니다. 게임 스레드는 작업 요청만 하고 결과는 다음 틱에 받아오도록 비동기 처리하여 병목 현상을 해결했습니다.
- **결과:** 해당 기능의 프로파일러상 **Inclusive Time이 87% 감소**했으며, 게임 스레드에서 **104ms의 연산 부하를 성공적으로 제거**했습니다.
- **관련 코드:** [MTTargetToFind.cpp](https://github.com/No1playblock/BossRaidGame/blob/main/Source/BossRaidGame/AI/Service/BTService_MTTargetToFind.cpp)

### 2. 런타임 스폰 부하 감소를 위한 오브젝트 풀링
<img width="1290" height="553" alt="스크린샷 2025-08-27 023422" src="https://github.com/user-attachments/assets/3c742baf-7964-4851-a15b-c7c5471f92d8" />

- **문제:** 전투 중 다수의 총알, 이펙트, 몬스터가 `SpawnActor`로 반복 생성/파괴되며 CPU 스파이크 및 프레임 드랍을 유발했습니다.
- **해결:** 미리 생성된 액터를 재활용하는 오브젝트 풀 매니저를 구현했습니다. `SpawnActor`/`DestroyActor` 대신 `SpawnFromPool`/`ReturnToPool` 함수를 사용하여 메모리 할당/해제 비용을 원천적으로 제거했습니다.
- **결과:** 몬스터 대량 소환 구간에서 순간적으로 **18 FPS까지 급락하던 최저 프레임을 33 FPS 이상으로 안정**시켜, 80%가 넘는 성능 향상을 이뤄냈습니다.
- **관련 코드:** [MobSpawnManager.cpp](https://github.com/No1playblock/BossRaidGame/blob/main/Source/BossRaidGame/Manager/MobSpawnManager.cpp)
  
### 3. 개발 효율성 증대를 위한 Slate 커스텀 에디터
<img width="2558" height="1008" alt="스크린샷 2025-07-21 223642" src="https://github.com/user-attachments/assets/5a728533-c99a-4605-8487-3196cb627026" />

- **문제:** 기획자가 스킬트리나 레벨별 스탯 밸런스를 수정할 때마다 개발자의 수동 작업 및 리빌드가 필요하여 반복 작업 시간이 길어졌습니다.
- **해결:** `Slate`를 사용하여 스킬 데이터를 노드 기반으로 시각화하고, 데이터 테이블과 직접 연동되는 스킬트리 에디터를 제작했습니다. 레벨별 스탯 테이블도 별도의 에디터에서 직접 수정할 수 있도록 개발했습니다.
- **결과:** 비개발 직군이 직접 밸런싱 작업 후 즉시 테스트하는 것이 가능해져, 개발 생산성 및 협업 효율을 크게 증대시켰습니다.
- **관련 코드:** [BRCustomEditor](https://github.com/No1playblock/BossRaidGame/tree/main/Source/BRCustomEditor)
  
