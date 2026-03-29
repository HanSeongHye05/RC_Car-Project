# STM32 RC Car Project

## 📌 Overview
STM32 기반 RC Car 제어 시스템 구현 프로젝트입니다.  
PWM 모터 제어, Bluetooth(UART) 통신, 초음파 센서를 활용한 장애물 감지 기능을 포함합니다.

---

## ⚙️ 주요 기능

- PWM 기반 DC 모터 속도 및 방향 제어
- Bluetooth(UART) 기반 원격 제어
- 초음파 센서 기반 거리 측정 및 자동 정지 기능

---

## 🧠 시스템 구조

Input (Bluetooth / Sensor)
→ Control Logic (Car_Control)
→ Output (Motor)

---

## 🛠️ 기술 스택

- STM32 HAL Driver
- UART / PWM / Timer Input Capture
- Embedded C

---

## 📁 코드 구조
