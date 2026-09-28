# BuzzerAlarm

无源蜂鸣器报警模块。
Passive buzzer alarm module.

构造时模块注册 LibXR 致命错误回调，并播放一段开机提示音（B4 200 ms、G3 200 ms、
B4 400 ms，构造期间阻塞约 800 ms）。发生致命错误时，LibXR 的致命错误循环反复调用该回调：
每次以 `alarm_freq` 鸣叫 `alarm_duration` 毫秒，然后等待 `alarm_delay` 毫秒。
LibXR 只保存一个致命错误回调，后构造的模块（例如 BlinkLED）注册的回调会覆盖先注册的。

On construction the Module registers a LibXR fatal-error callback and plays a
start-up tune (B4 200 ms, G3 200 ms, B4 400 ms; the constructor blocks for about
800 ms). On a fatal error the LibXR fatal-error loop calls the callback repeatedly:
each call beeps at `alarm_freq` for `alarm_duration` ms and then waits `alarm_delay`
ms. LibXR keeps only one fatal-error callback, so a Module constructed later that
registers its own (for example BlinkLED) replaces it.

## 接口 / Interface

- `void Play(uint32_t freq, uint32_t duration)`：以 `freq` Hz、0.5% 占空比鸣叫
  `duration` 毫秒（阻塞），然后关闭 PWM。/ Beep at `freq` Hz with 0.5 % duty for
  `duration` ms (blocking), then disable the PWM.
- `void PlayNote(NoteName note, uint32_t octave, uint32_t duration)`：按音名和八度
  （十二平均律，A4 = 440 Hz）换算频率后调用 `Play`。`NoteName` 为 `C`、`Cs`、`D`、`Ds`、
  `E`、`F`、`Fs`、`G`、`Gs`、`A`、`As`、`B`。/ Convert note name and octave
  (equal temperament, A4 = 440 Hz) to a frequency and call `Play`.

## 依赖 / Dependencies

无其他模块依赖，仅使用 LibXR。
No other Modules; LibXR only.

## 构造接口 / Constructor

```cpp
BuzzerAlarm(LibXR::PWM& pwm,
            uint32_t alarm_freq = 1500,
            uint32_t alarm_duration = 300,
            uint32_t alarm_delay = 300);
```

依赖 / Dependencies:

- `pwm`：驱动无源蜂鸣器的 PWM 通道。/ The PWM channel that drives the passive buzzer.

配置 / Configuration:

- `alarm_freq`：致命错误报警频率，单位 Hz，默认 1500。/ Fatal-error alarm frequency
  in Hz, default 1500.
- `alarm_duration`：每次报警鸣叫时长，单位 ms，默认 300。/ Beep length per alarm in
  ms, default 300.
- `alarm_delay`：每次报警后的等待时间，单位 ms，默认 300。/ Pause after each alarm
  beep in ms, default 300.

## 使用 / Use

```sh
xrobot module add xrobot-org/BuzzerAlarm
xrobot setup
xrobot instance add xrobot-org/BuzzerAlarm
```

`xrobot instance add` 在 `User/xrobot.yaml` 中写入一个实例，依赖项留空，默认值按源码写出；
把 `pwm` 填为 BSP 中用 `XR_REGISTER` 注册的 PWM 对象名：
`xrobot instance add` writes an instance to `User/xrobot.yaml` with empty
dependencies and the source defaults; set `pwm` to the name of a PWM object the BSP
registers with `XR_REGISTER`:

```yaml
modules:
  - module: xrobot-org/BuzzerAlarm
    id: buzzeralarm_0
    args:
      - pwm: pwm_buzzer
      - alarm_freq: '1500'
      - alarm_duration: '300'
      - alarm_delay: '300'
```

BSP 侧 / BSP side:

```cpp
XR_REGISTER(pwm_buzzer, LibXR::PWM);
```

填好后再次运行 `xrobot setup`，生成 `User/xrobot_main.hpp`。
Run `xrobot setup` again to generate `User/xrobot_main.hpp`.

`xrobot module show .`（在本仓库中）或 `xrobot module show Modules/xrobot-org/BuzzerAlarm`
（在 BSP 中）打印 manifest 和当前的构造函数。
`xrobot module show .` in this repository, or
`xrobot module show Modules/xrobot-org/BuzzerAlarm` in a BSP, prints the manifest and
the current constructor.
