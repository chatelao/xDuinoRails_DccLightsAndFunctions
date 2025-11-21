#ifndef EFFECT_H
#define EFFECT_H

#include "../PhysicalOutput.h"
#include <vector>
#include <cstdint>
#include <FastLED.h>

namespace xDuinoRails {

class Effect {
public:
    virtual ~Effect() {}
    virtual void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) = 0;
    virtual void setActive(bool active) { _is_active = active; }
    virtual bool isActive() const { return _is_active; }
    virtual void setDimmed(bool dimmed) {}
    virtual bool isDimmed() const { return false; }

protected:
    bool _is_active = false;
};

class EffectSteady : public Effect {
public:
    EffectSteady(uint8_t brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    uint8_t _brightness;
};

class EffectDimming : public Effect {
public:
    EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setDimmed(bool dimmed) override;
    bool isDimmed() const override { return _is_dimmed; }
private:
    uint8_t _brightness_full;
    uint8_t _brightness_dimmed;
    bool _is_dimmed = false;
};

class EffectFlicker : public Effect {
public:
    EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    uint8_t _base_brightness;
    uint8_t _flicker_depth;
    uint8_t _flicker_speed;
    uint16_t _noise_position; // Changed to uint16_t for FastLED inoise8
    uint16_t _noise_increment; // Changed to uint16_t
};

class EffectStrobe : public Effect {
public:
    EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;
private:
    uint32_t _strobe_period_ms;
    uint32_t _on_time_ms;
    uint8_t _brightness;
    uint32_t _timer;
};

class EffectMarsLight : public Effect {
public:
    EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    uint8_t _bpm; // Beats per minute, derived from frequency
    uint8_t _peak_brightness;
    uint8_t _phase_shift; // 0-255
};

class EffectSoftStartStop : public Effect {
public:
    EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;
private:
    uint16_t _fade_in_time_ms;
    uint16_t _fade_out_time_ms;
    uint8_t _target_brightness;
    uint32_t _current_brightness; // Changed to 32-bit integer (8.8 fixed point requires 16+ bits, keeping 32 for safety and compatibility with accumulators)
    uint32_t _timer; // Added for timing
};

class EffectServo : public Effect {
public:
    EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;
private:
    uint8_t _endpoint_a;
    uint8_t _endpoint_b;
    float _speed;
    float _current_angle;
    float _target_angle;
    bool _is_at_a = true;
};

class EffectSmokeGenerator : public Effect {
public:
    EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    bool _heater_enabled;
    uint8_t _fan_speed;
};

// New Effect: Fire (Virtual Strip Demo)
class EffectFire : public Effect {
public:
    EffectFire(uint8_t cooling, uint8_t sparking, uint8_t length);
    ~EffectFire();

    // Delete copy constructor and assignment operator to prevent double-free
    EffectFire(const EffectFire&) = delete;
    EffectFire& operator=(const EffectFire&) = delete;

    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    uint8_t _cooling;
    uint8_t _sparking;
    uint8_t _length;
    uint8_t* _heat; // Virtual heat array
};

}

#endif // EFFECT_H
