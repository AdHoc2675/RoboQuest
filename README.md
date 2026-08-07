# RoboQuest

> Unreal Engine 5.4 기반 1인칭 로그라이트 슈터

---

## 프로젝트 개요

**RoboQuest**는 절차적 전투와 무기 커스터마이징을 핵심으로 하는 1인칭 로그라이트 FPS입니다.  
플레이어는 다양한 로봇 적과 전투하며, 전투 보상으로 무기를 강화하고 어빌리티를 활용해 스테이지를 클리어합니다.

### 핵심 특징

- **데이터 드리븐 무기 시스템** — DataTable 기반 스탯 + Affix(퍼크) + Rarity 등급
- **경량 커스텀 어빌리티 시스템** — GAS 없이 UObject 기반으로 구현
- **전투존 기반 진행** — 진입 → 잠금 → 스폰 → 전멸 → 개방 사이클
- **셀 셰이딩 비주얼** + Niagara 이펙트

---

## 기술 스택

| 항목 | 내용 |
|---|---|
| 엔진 | Unreal Engine 5.4 |
| 언어 | C++ / Blueprint 하이브리드 |
| 입력 | Enhanced Input System |
| AI | AIModule + Behavior Tree |
| UI | UMG (Unreal Motion Graphics) |
| VFX | Niagara |
| 레벨 전환 | MoviePlayer (페이크 로딩 스크린) |

### 모듈 의존성

```
Core, CoreUObject, Engine, InputCore, EnhancedInput,
Slate, SlateCore, Niagara, UMG, MoviePlayer, AIModule
```

---

## 아키텍처

### 시스템 구조

```
ARoboQuestGameMode
├── ARoboQuestCharacter (Player)
│   ├── UStatusComponent         — 체력 / EXP / 레벨 / 방어 / 이속 / 파워(화폐)
│   ├── UPlayerAbilityComponent  — Q / F / Passive 어빌리티 슬롯
│   ├── UTP_WeaponComponent      — 무기 (스탯 + Affix + 발사 + 재장전)
│   └── UBaseUserHUDWidget       — 메인 HUD
│
├── AEnemyBase (Enemy Hierarchy)
│   ├── AEnemyBotBase            — 지상 보행 로봇
│   ├── AEnemyFlyBase            — 비행 유닛
│   ├── AEnemyPawnBase           — AI Controller 기반 포탑
│   └── AEnemyPodBase            — 투사체 발사 포드
│
├── ACombatZone                  — 전투 영역 관리 (스폰 + 문 잠금/개방)
│
└── URoboQuestGameInstance       — 레벨 전환 + 로딩 화면
```

### 소스 디렉토리

```
Source/RoboQuest/
├── RoboQuestCharacter.h/cpp        # 플레이어 캐릭터
├── TP_WeaponComponent.h/cpp        # 무기 컴포넌트
├── RoboQuestProjectile.h/cpp       # 투사체
├── RoboQuestGameMode.h/cpp         # 게임 모드
├── RoboQuestPlayerController.h/cpp # 플레이어 컨트롤러
├── TP_PickUpComponent.h/cpp        # 픽업 컴포넌트
│
├── Public/
│   ├── Abilities/                  # 어빌리티 (Base + Frenzy, HomingMissile, ShotgunBlast)
│   ├── Components/                 # StatusComponent, PlayerAbilityComponent
│   ├── Data/                       # WeaponStatRow, EnemyStatRow, WeaponAffix, Affixes/
│   ├── Enemy/                      # EnemyBase, Bot/, Fly/, Pawn/, Pod/, CombatZone, SpawnPoint
│   ├── Interactable/               # DoorBase, SlidingDoor, LevelPortal, UpgradeBench
│   ├── Pickups/                    # HealingCell, PowerCell
│   ├── System/                     # RoboQuestGameInstance
│   ├── UI/                         # HUD, DamageText, Crosshair, AbilityDisplay 등
│   └── World/                      # DoorBase
│
└── Private/                        # 위 Public 구조의 .cpp 구현
```

---

## 핵심 시스템

### 1. 무기 시스템

`UTP_WeaponComponent`는 `USkeletalMeshComponent`를 상속하여 **무기 자체가 메시 컴포넌트**로 동작합니다.

**스탯 계산 모델**: 모든 수치 스탯은 `Base × Multiplier = Final` 패턴을 따릅니다.

| 스탯 | 설명 |
|---|---|
| Damage | 탄당 기본 데미지 |
| RateOfFire | 초당 발사 수 (RPS) |
| MaxAmmo | 탄창 용량 |
| RangeMeter | 유효 사거리 (미터) |
| ReloadTime | 재장전 시간 (초) |
| CritDamage | 크리티컬 데미지 배율 |
| Spread | 조준 산탄 (Min/Max, 발사 시 증가 → 시간 경과 시 회복) |

**데이터 흐름**:
```
DataTable (FWeaponStatRow) → InitializeWeapon() → Base 스탯 설정
                                                  ↓
                          RecalculateStats() ← Affix 적용 + 레벨 보정
                                                  ↓
                                            Final 스탯 산출
```

**무기 분류**:
- **타입**: Assault, Precision, Technology, Demolition, CloseCombat
- **탄약**: Magazine, Energy
- **등급**: Common → Uncommon → Rare → Epic → Fantastic

#### Affix 시스템

`UWeaponAffix`를 상속하는 퍼크/룬 시스템입니다. 무기 등급(Rarity)에 따라 장착 가능한 Affix 수가 결정됩니다.

- **스탯 Affix**: Swift(이속+10%), Big Mag(탄창+50%), Cadence(발사속도+15%/정확도-20%), Longshot(정확도+35%/사거리+35%), Quickload(재장전+20%)
- **속성 Affix**: Burn(화염), Cryo(빙결), Shock(전기) — `DamageType`을 변환

각 무기는 DataTable에서 정의된 기본 Affix(`DefaultAffixes`)를 가지며, `RerollRandomAffixes()`로 랜덤 Affix를 재추첨할 수 있습니다.

### 2. 어빌리티 시스템

GAS를 사용하지 않고 **`URoboQuestAbility` (UObject)** 기반으로 경량 구현했습니다.

```
URoboQuestAbility (Abstract)
├── UAbility_Frenzy         — Passive, 스택 기반 버프
├── UAbility_HomingMissile  — Q 슬롯, 유도 미사일
└── UAbility_ShotgunBlast   — F 슬롯, 산탄 폭발
```

- **쿨다운**: `CooldownDuration` + `FTimerHandle` 기반 관리
- **패시브 훅**: `OnPassiveActivate()` — 컴포넌트 초기화 시 즉시 호출
- **스택 인터페이스**: `HasStacks()`, `GetCurrentStackCount()` — UI 바인딩용

#### Frenzy (패시브 어빌리티)

킬/보스 적중 이벤트에 반응하여 스택을 축적하고, 스택 수에 비례하여 무기/이동 스탯을 버프합니다.

| 파라미터 | 값 |
|---|---|
| 킬 당 스택 | +2 |
| 보스 어빌리티 적중 당 스택 | +4 |
| 최대 스택 | 16 |
| 감쇠 속도 | 1.25 스택/초 |
| 스택 당 효과 | 발사속도 +1%, 재장전 +1%, 이속 +0.5% |

### 3. 적 시스템

`AEnemyBase`(`ACharacter` 상속)를 공통 부모로, 4가지 행동 패턴의 적 계열을 파생합니다.

| 계열 | 클래스 | 특징 | AI |
|---|---|---|---|
| Bot | `AEnemyBotBase` | 지상 보행 | NavMesh |
| Fly | `AEnemyFlyBase` | 공중 비행 | 커스텀 이동 |
| Pawn | `AEnemyPawnBase` | 고정/포탑형 | `AEnemyPawnAIController` + Behavior Tree |
| Pod | `AEnemyPodBase` | 투사체 발사 포드 | 커스텀 로직 |

**공통 기능**:
- `UStatusComponent`를 통한 체력/레벨 관리 (DataTable 기반 `InitializeEnemyStats()`)
- 사망 시 드롭: HealingCell(고정), PowerCell(확률 10%)
- 플로팅 데미지 텍스트 (`UDamageTextWidget`)
- `OnEnemyDied` 델리게이트 → `ACombatZone`에서 구독하여 남은 적 추적

### 4. 전투존 시스템

`ACombatZone`은 레벨 내 전투 구역을 정의하고 관리합니다.

```
플레이어 진입 (BoxComponent Overlap)
    → LinkedDoors 잠금
    → SpawnPoints에서 적 즉시 스폰
    → AliveEnemyCount 추적
    → 전원 처치 시 CompleteZone()
        → LinkedDoors 해제
        → DoorsToOpenOnComplete 개방
```

### 5. 상호작용 & 업그레이드

플레이어 캐릭터는 매 Tick마다 `PerformInteractionCheck()`로 전방 300 유닛 범위의 상호작용 가능 오브젝트를 감지합니다.

| 오브젝트 | 기능 |
|---|---|
| `AWeaponUpgradeBench` | 무기 레벨 업그레이드 (파워셀 소비) |
| `AWeaponRarityUpgradeBench` | 무기 등급 업그레이드 |
| `AWeaponUpgradeStation` | 무기 업그레이드 통합 스테이션 |
| `ALevelPortal` | 다음 레벨로 전환 |
| `AStageFinishDoor` | 스테이지 클리어 문 |

### 6. UI

UMG 기반 위젯으로 모든 인게임 UI를 구성합니다.

| 위젯 | 역할 |
|---|---|
| `UBaseUserHUDWidget` | 메인 HUD — 체력바, 탄약, 경험치, 크로스헤어, 어빌리티 디스플레이 |
| `UDamageTextWidget` | 적 피격 시 플로팅 데미지 넘버 |
| `UCrosshairWidget` | 동적 크로스헤어 (Spread 값 반영) |
| `UAbilityDisplayWidget` | 어빌리티 쿨다운/스택 표시 |
| `UWeaponDetailWidget` | 무기 상세 정보 패널 |
| `UStageResultWidget` | 스테이지 결과 화면 |
| `URoboQuestLoadingWidget` | 레벨 전환 로딩 화면 (팁 + 배경 이미지) |

---

## 게임 루프

```
레벨 진입
  └→ 탐색 & 상호작용
       └→ 전투존 진입
            └→ 문 잠금 + 적 스폰
                 └→ 전투 (적 처치 → EXP/HealingCell/PowerCell 획득)
                      └→ 전원 처치 → 문 개방
                           └→ 업그레이드 벤치 (무기 레벨/등급/Affix)
                                └→ 다음 전투존 또는 포탈
                                     └→ 스테이지 클리어 → 결과 화면 → 다음 레벨
```

---

## 콘텐츠 에셋 구조

```
Content/
├── CelShader/            # 셀 셰이딩 머티리얼
├── FPWeapon/             # 1인칭 무기 메시
├── FirstPerson/          # 1인칭 기본 에셋
├── FirstPersonArms/      # 1인칭 팔 메시/애니메이션
├── LevelPrototyping/     # 레벨 프로토타이핑
├── MuzzleFlashesVol1/    # 머즐 플래시 VFX
├── RoboQuestAsset/       # 로봇 캐릭터 에셋 (FlashFly, GatlingBot, GunPawn, LightFly)
├── StarterContent/       # UE 기본 에셋
└── _BP/
    ├── Abilities/        # 어빌리티 Blueprint (3종)
    ├── DataTable/        # 무기/적 DataTable
    ├── Enemy/            # 적 BP + AnimBP + BehaviorTree (16개)
    ├── Map/              # 맵 + 문/포탈 BP (8개)
    ├── Pickups/          # 픽업 아이템 BP
    ├── UI/               # UMG 위젯 BP (8개)
    ├── Weapon/           # 무기 BP
    └── etc/              # 기타
```

---

## 빌드 환경

- **엔진**: Unreal Engine 5.4
- **IDE**: Visual Studio (`.vsconfig` 포함)
- **플러그인**: ModelingToolsEditorMode (Editor 전용)
