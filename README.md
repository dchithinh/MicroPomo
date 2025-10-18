## MicroPomo - Embedded Pomodoro Timer
A lightweight, cross-platform Pomodoro timer built with LVGL v9.4.0

### Setup & Run:
The build script is design to build and run on window.
```
    git clone https://github.com/dchithinh/MicroPomo.git
    cd MicroPomo && ./build.cmd
```

## Design
### High Level Design
![alt text](https://github.com/dchithinh/MicroPomo/blob/main/src/pomodoro/doc/HighLevelDesign.png)

### Pomodoro State Machine
![alt text](https://github.com/dchithinh/MicroPomo/blob/main/src/pomodoro/doc/PomoAppStateMachine.png)

### Timer State Machine
![alt text](https://github.com/dchithinh/MicroPomo/blob/main/src/pomodoro/doc/TimerStateMachine.png)

The draw.io file design is provided in `doc` and also the `readme.md`

## Screenshot
![alt text](https://github.com/dchithinh/MicroPomo/blob/main/src/pomodoro/doc/Start.png)

![alt text](https://github.com/dchithinh/MicroPomo/blob/main/src/pomodoro/doc/Run.png)

![alt text](https://github.com/dchithinh/MicroPomo/blob/main/src/pomodoro/doc/Settings.png)

## Porting
- Use can use this build design to run/debug on your PC. Then can intergate the `pomodoro` folder standablone into your HW **OR**
- Take the whole code and add your HW driver into `hal`

As an example, I have taken `pomodoro` folder into my STM32F407 board. You can have a look at [stm32f407xx_pomodoro](https://github.com/dchithinh/stm32f407xx_pomodoro/tree/master)

## Acknowledgment
- lv_port_pc_vscode: https://github.com/lvgl/lv_port_pc_vscode
