# STM32 RC Car Autonomous Driving

STM32 기반 초음파 센서를 이용한 장애물 회피 RC Car 자율주행 시스템

---

## Project Overview

본 프로젝트는 STM32 마이크로컨트롤러와 초음파 센서를 이용하여  
장애물을 감지하고 회피하며 주행하는 RC Car 자율주행 시스템을 구현한 프로젝트입니다.

전방 거리 측정과 좌우 거리 비교를 기반으로 회피 방향을 결정하며  
PWM 제어를 통해 DC 모터 속도를 제어합니다.

---

## Hardware

- MCU : STM32
- Ultrasonic Sensor : HC-SR04
- Motor Driver : L298N
- DC Motor

---

## Control Logic

1. 초음파 센서를 이용해 전방 거리 측정
2. 전방 장애물 감지 시 좌우 거리 비교
3. 좌회전 또는 우회전 결정
4. PWM 기반 모터 속도 제어

---

## Technologies

- STM32
- PWM Motor Control
- Timer Distance Measurement
- Ultrasonic Sensor (HC-SR04)
- L298N Motor Driver
