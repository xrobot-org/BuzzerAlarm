#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: 无源蜂鸣器报警模块 / Buzzer alarm module
constructor_args:
  - alarm_freq: 1500
  - alarm_duration: 300
  - alarm_delay: 300
template_args: []
required_hardware: pwm_buzzer
depends: []
=== END MANIFEST === */
// clang-format on

#include <cstdint>

#include "app_framework.hpp"
#include "pwm.hpp"
#include "thread.hpp"

/**
 * @brief 无源蜂鸣器报警模块
 * @details 提供固定频率报警和音阶播放能力，并支持 FatalError 回调报警。
 */
class BuzzerAlarm : public LibXR::Application {
 public:
  // NOLINTNEXTLINE
  enum class Octave { LOW = 0, MIDDLE = 1, HIGH = 2 };

  enum class NoteName : uint8_t { C = 0, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };

  /**
   * @brief 构造 BuzzerAlarm 模块
   * @param hw 硬件容器
   * @param app 应用管理器
   * @param alarm_freq 报警频率
   * @param alarm_duration 单次报警持续时间（ms）
   * @param alarm_delay 报警间隔（ms）
   */
  BuzzerAlarm(LibXR::HardwareContainer& hw, LibXR::ApplicationManager& app,
              uint32_t alarm_freq, uint32_t alarm_duration,
              uint32_t alarm_delay)
      : alarm_freq_(alarm_freq),
        alarm_duration_(alarm_duration),
        alarm_delay_(alarm_delay),
        pwm_(hw.template FindOrExit<LibXR::PWM>({"pwm_buzzer"})) {
    app.Register(*this);

    auto error_callback = LibXR::Assert::Callback::Create(
        [](bool in_isr, BuzzerAlarm* alarm, const char* file, uint32_t line) {
          UNUSED(file);
          UNUSED(line);

          alarm->Play(alarm->alarm_freq_, alarm->alarm_duration_);
          if (!in_isr) {
            LibXR::Thread::Sleep(alarm->alarm_delay_);
          }
        },
        this);

    LibXR::Assert::RegisterFatalErrorCallback(error_callback);
    PlayNote(3, Octave::HIGH, 600);
    PlayNote(2, Octave::HIGH, 300);
    PlayNote(1, Octave::HIGH, 600);
  }

  /**
   * @brief 播放指定频率蜂鸣
   * @param freq 频率（Hz）
   * @param duration 持续时间（ms）
   */
  void Play(uint32_t freq, uint32_t duration) {
    pwm_->SetConfig({freq});
    pwm_->Enable();
    pwm_->SetDutyCycle(0.01);
    LibXR::Thread::Sleep(duration);
    pwm_->Disable();
  }

  /**
   * @brief 按音阶播放音符
   * @param note_in_scale 音阶序号，范围 1~7
   * @param octave 八度
   * @param duration 持续时间（ms）
   */
  void PlayNote(uint32_t note_in_scale, Octave octave, uint32_t duration) {
    if (note_in_scale < 1 || note_in_scale > 7) {
      return;
    }
    int index = (static_cast<int>(octave) * 7) + (note_in_scale - 1);
    Play(static_cast<uint32_t>(FRE[index]), duration);
  }

  void OnMonitor() override {}

 private:
  static constexpr float FRE[21] = {
      261.63f,  293.67f,  329.63f,  349.23f,  391.99f,  440.00f,  493.88f,
      523.25f,  587.33f,  659.25f,  698.46f,  783.99f,  880.00f,  987.76f,
      1046.50f, 1174.66f, 1318.51f, 1396.92f, 1567.98f, 1760.00f, 1975.52f};
  uint32_t alarm_freq_;
  uint32_t alarm_duration_;
  uint32_t alarm_delay_;

  LibXR::PWM* pwm_;
};
