#pragma once

// clang-format off
/* === MODULE MANIFEST ===
module_name: BuzzerAlarm
module_description: 无源蜂鸣器报警模块 / Buzzer alarm module
constructor_args:
  - alarm_freq: 1500
  - alarm_duration: 300
  - alarm_delay: 300
required_hardware: pwm_buzzer
repository: https://github.com/xrobot-org/BuzzerAlarm
=== END MANIFEST === */
// clang-format on

#include "app_framework.hpp"
#include "pwm.hpp"

template <typename HardwareContainer>
class BuzzerAlarm : public LibXR::Application {
 public:
  // NOLINTNEXTLINE
  enum class NoteName { C = 0, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B };

  BuzzerAlarm(HardwareContainer &hw, LibXR::ApplicationManager &app,
              uint32_t alarm_freq, uint32_t alarm_duration,
              uint32_t alarm_delay)
      : alarm_freq_(alarm_freq),
        alarm_duration_(alarm_duration),
        alarm_delay_(alarm_delay),
        pwm_(hw.template FindOrExit<LibXR::PWM>({"pwm_buzzer"})) {
    app.Register(*this);

    auto error_callback = LibXR::Callback<const char *, uint32_t>::Create(
        [](bool in_isr, BuzzerAlarm<HardwareContainer> *alarm, const char *file,
           uint32_t line) {
          UNUSED(file);
          UNUSED(line);

          alarm->Play(alarm->alarm_freq_, alarm->alarm_duration_);
          if (!in_isr) {
            LibXR::Thread::Sleep(alarm->alarm_delay_);
          }
        },
        this);

    LibXR::Assert::RegisterFatalErrorCB(error_callback);
    PlayNote(NoteName::B, 4, 200);
    PlayNote(NoteName::G, 3, 200);
    PlayNote(NoteName::B, 4, 400);
  }

  void Play(uint32_t freq, uint32_t duration) {
    pwm_->SetConfig({freq});
    pwm_->Enable();
    pwm_->SetDutyCycle(0.5);
    LibXR::Thread::Sleep(duration);
    pwm_->Disable();
  }

  void PlayNote(NoteName note, uint32_t octave, uint32_t duration) {
    float midi_num = static_cast<int>(note) + (octave + 1) * 12;
    float freq = 440.0f * std::pow(2.0f, (midi_num - 69) / 12.0f);
    Play(freq, duration);
  }

  void OnMonitor() override {}

 private:
  uint32_t alarm_freq_;
  uint32_t alarm_duration_;
  uint32_t alarm_delay_;

  LibXR::PWM *pwm_;
};
