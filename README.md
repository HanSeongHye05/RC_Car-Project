# 🚗 STM32 RC Car Control System

---

## 📌 프로젝트 개요

* STM32 기반 RC Car 제어 시스템
* Bluetooth(UART) + 초음파 센서 기반 제어
* 센서 입력에 따라 동작이 결정되는 구조

👉 **센서 기반 자율 제어 시스템 구현**

---

## ⚙️ 시스템 구조

```plaintext
[ Bluetooth 입력 ]        [ 초음파 센서 ]
        ↓                       ↓
        └──────→ [ 제어 로직 ] ←──────┘
                        ↓
                [ Motor 제어 (PWM + GPIO) ]
```

👉 **입력 → 판단 → 제어 흐름**

---

## 🔧 핵심 기능

### 1. Bluetooth 제어 (UART)

* UART 인터럽트 기반 데이터 수신
* 명령어 (w / a / s / d / x) → 주행 제어

---

### 2. 초음파 센서 (Input Capture)

* Echo 시간 기반 거리 계산
* `distance = echoTime / 58`
* 일정 거리 이하 → 자동 정지

---

### 3. Motor 제어 (PWM)

* PWM → 속도 제어
* GPIO → 방향 제어
* Forward / Back / Left / Right / Stop

---

## 🔄 제어 흐름

```plaintext
Bluetooth 입력 수신
        ↓
초음파 거리 측정
        ↓
거리 조건 판단
        ↓
모터 제어 수행
```

---

## 🚨 문제 해결 경험

* 센서 값 변동 → 제어 불안정
  → 거리 임계값 기반 제어 적용

* 장애물 감지 시 즉시 정지 → 흐름 단절
  → 조건 기반 주행 로직 개선

* UART 수신 불안정
  → 인터럽트 방식으로 개선

---

## 📈 개선 방향

* 센서 노이즈 필터링 적용
* 거리 조건 기반 제어 로직 고도화
* 다중 센서 기반 확장

---

## 🧰 기술 스택

* STM32 HAL Driver
* UART / PWM / Timer Input Capture
* Embedded C

---

## 📁 코드 구조

```plaintext
main.c          → 전체 제어 흐름
motor.c         → 모터 제어 (PWM + GPIO)
ultrasonic.c    → 거리 측정 (Input Capture)
bluetooth.c     → UART 제어 (인터럽트)
```

---

## 🎯 핵심 포인트

👉 **센서 데이터를 기반으로 동작을 결정하는 제어 시스템 구현**

---
