#include "Effect.h"
#include <Arduino.h>
#include <algorithm>
#include <cstring>

// FastLED math is used here. Since we might be in a mock environment,
// we rely on the FastLED headers being present or mocked appropriately.

namespace xDuinoRails {

EffectSteady::EffectSteady(uint8_t brightness) : _brightness(brightness) {}

void EffectSteady::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    uint8_t value = _is_active ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectDimming::EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed)
    : _brightness_full(brightness_full), _brightness_dimmed(brightness_dimmed) {}

void EffectDimming::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    uint8_t value = 0;
    if (_is_active) {
        value = _is_dimmed ? _brightness_dimmed : _brightness_full;
    }
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

void EffectDimming::setDimmed(bool dimmed) {
    _is_dimmed = dimmed;
}

// Refactored EffectFlicker to use FastLED inoise8
EffectFlicker::EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed)
    : _base_brightness(base_brightness), _flicker_depth(flicker_depth), _flicker_speed(flicker_speed),
      _noise_position(random16()), _noise_increment(0) {
    // Map speed 0-255 to a reasonable noise increment step
    // FastLED noise usually works well with steps of 10-100 per frame
    _noise_increment = map(flicker_speed, 0, 255, 5, 100);
}

void EffectFlicker::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }

    // Advance noise position proportional to delta_ms to keep speed consistent
    // Assuming 60fps reference (approx 16ms)
    uint16_t steps = (delta_ms * _noise_increment) / 16;
    if (steps == 0 && delta_ms > 0) steps = 1; // Ensure minimal movement
    _noise_position += steps;

    // Get noise value (0-255)
    uint8_t noise_val = inoise8(_noise_position);

    // Scale noise to flicker depth
    // If noise is 0, we subtract half depth. If 255, add half depth.
    // Result = base + (noise mapped to [-depth/2, depth/2])

    int16_t delta = scale8(noise_val, _flicker_depth) - (_flicker_depth / 2);
    int16_t val = _base_brightness + delta;

    uint8_t value = constrain(val, 0, 255);

    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// EffectStrobe remains mostly similar but uses standard types
EffectStrobe::EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness)
    : _brightness(brightness), _timer(0) {
    if (strobe_frequency_hz == 0) strobe_frequency_hz = 1;
    _strobe_period_ms = 1000 / strobe_frequency_hz;
    _on_time_ms = (_strobe_period_ms * constrain(duty_cycle_percent, 0, 100)) / 100;
}

void EffectStrobe::setActive(bool active) {
    Effect::setActive(active);
    if (!active) _timer = 0;
}

void EffectStrobe::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }

    // Optimized to avoid modulo
    _timer += delta_ms;
    if (_timer >= _strobe_period_ms) {
        _timer %= _strobe_period_ms; // Fallback only if exceeded
    }

    uint8_t value = (_timer < _on_time_ms) ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// Refactored EffectMarsLight to use FastLED beatsin8 / sin8
EffectMarsLight::EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent)
    : _peak_brightness(peak_brightness) {
    // oscillation_frequency_mhz is in milli-Hertz (e.g. 1000 = 1Hz)
    // FastLED beatsin8 uses BPM. 1Hz = 60 BPM.
    // mHz to BPM: (mHz / 1000) * 60 = mHz * 0.06
    float bpm_f = oscillation_frequency_mhz * 0.06f;
    _bpm = (uint8_t)bpm_f;
    if (_bpm == 0) _bpm = 1;

    // Phase shift: 0-100% -> 0-255
    _phase_shift = map(phase_shift_percent, 0, 100, 0, 255);
}

void EffectMarsLight::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }

    // beatsin8 returns a value oscillating between low and high
    // We want 0 to peak_brightness
    // beatsin8(bpm, low, high, timebase, phase_offset)
    // We use millis() as timebase.

    // Note: beatsin8 auto-calculates based on millis().
    // If we need exact phase sync across multiple effects, they rely on the same millis().
    // The phase_offset allows the shift.

    uint8_t value = beatsin8(_bpm, 0, _peak_brightness, 0, _phase_shift);

    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// Refactored EffectSoftStartStop to use fixed-point math (8.8)
EffectSoftStartStop::EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness)
    : _target_brightness(target_brightness), _current_brightness(0), _timer(0),
      _fade_in_time_ms(fade_in_time_ms), _fade_out_time_ms(fade_out_time_ms) {}

void EffectSoftStartStop::setActive(bool active) {
    Effect::setActive(active);
}

void EffectSoftStartStop::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    // We use _current_brightness as a 16-bit value where the high byte is the integer brightness (0-255)
    // and the low byte is the fractional part.
    // Target brightness must be shifted up by 8 bits for comparison.

    uint32_t target_fixed = ((uint32_t)_target_brightness) << 8;

    // Calculate step size per ms in fixed point.
    // Step = (Target * 256) / Duration
    // We calculate this dynamically to support changing delta_ms, but for constant duration we could cache it.
    // Note: this is still linear.

    uint32_t step_in = (_fade_in_time_ms > 0) ? (target_fixed * delta_ms) / _fade_in_time_ms : target_fixed;
    uint32_t step_out = (_fade_out_time_ms > 0) ? (target_fixed * delta_ms) / _fade_out_time_ms : target_fixed;

    // Ensure we move at least 1 unit in fixed point if duration is very long but not infinite
    if (step_in == 0 && delta_ms > 0 && _fade_in_time_ms > 0) step_in = 1;
    if (step_out == 0 && delta_ms > 0 && _fade_out_time_ms > 0) step_out = 1;

    if (_is_active) {
        if (_current_brightness < target_fixed) {
            if (target_fixed - _current_brightness < step_in) {
                _current_brightness = target_fixed;
            } else {
                _current_brightness += step_in;
            }
        }
    } else {
        if (_current_brightness > 0) {
            if (_current_brightness < step_out) {
                _current_brightness = 0;
            } else {
                _current_brightness -= step_out;
            }
        }
    }

    // Output value is the high byte
    uint8_t value = (uint8_t)(_current_brightness >> 8);

    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectServo::EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed)
    : _endpoint_a(endpoint_a), _endpoint_b(endpoint_b), _current_angle((uint16_t)endpoint_a << 8), _target_angle((uint16_t)endpoint_a << 8) {

    // Calculate speed in fixed point 8.8 (units per ms)
    // Original formula: speed_f = 0.01f + (travel_speed / 255.0f) * 0.49f; (degrees per ms)
    // In fixed point 8.8, 1 degree = 256 units.
    // Min speed (0.01 deg/ms) = 0.01 * 256 = 2.56 ~ 3 units/ms
    // Max speed (0.5 deg/ms) = 0.5 * 256 = 128 units/ms

    if (travel_speed == 0) {
        // Instant move (very high speed)
        _speed = 0xFFFF; // Max value
    } else {
        // map 1-255 to 3-128
        // We can use map() or manual calculation
        // 3 + (travel_speed * (128 - 3)) / 255
        uint32_t speed_calc = 3 + ((uint32_t)travel_speed * 125) / 255;
        _speed = (uint16_t)speed_calc;
    }
}

void EffectServo::setActive(bool active) {
    if (active && !_is_active) {
        // Set new target
        uint8_t target = _is_at_a ? _endpoint_b : _endpoint_a;
        _target_angle = (uint16_t)target << 8;
        _is_at_a = !_is_at_a;
    }
    Effect::setActive(active);
}

void EffectServo::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (_current_angle != _target_angle) {
        // Calculate delta angle in fixed point
        // prevent overflow if delta_ms is huge
        uint32_t delta_angle = (uint32_t)_speed * delta_ms;

        if (_current_angle < _target_angle) {
            // Moving up
            if ((uint32_t)_target_angle - _current_angle < delta_angle) {
                _current_angle = _target_angle;
            } else {
                _current_angle += (uint16_t)delta_angle;
            }
        } else {
            // Moving down
            if ((uint32_t)_current_angle - _target_angle < delta_angle) {
                _current_angle = _target_angle;
            } else {
                _current_angle -= (uint16_t)delta_angle;
            }
        }
    }
    for (auto* output : outputs) {
        // Output angle is integer part (high byte)
        output->setServoAngle(_current_angle >> 8);
    }
}

EffectSmokeGenerator::EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed)
    : _heater_enabled(heater_enabled), _fan_speed(fan_speed) {}

void EffectSmokeGenerator::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (outputs.empty()) return;
    uint8_t heater_value = (_is_active && _heater_enabled) ? 255 : 0;
    uint8_t fan_value = _is_active ? _fan_speed : 0;
    if (outputs.size() > 0) outputs[0]->setValue(heater_value);
    if (outputs.size() > 1) outputs[1]->setValue(fan_value);
}

// Implementation of EffectFire (Virtual Strip Demo)
EffectFire::EffectFire(uint8_t cooling, uint8_t sparking, uint8_t length)
    : _cooling(cooling), _sparking(sparking), _length(length) {
    if (_length == 0) _length = 1;
    // Allocate virtual heat array
    _heat = new uint8_t[_length];
    memset(_heat, 0, _length);
}

EffectFire::~EffectFire() {
    delete[] _heat;
}

void EffectFire::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }

    // Standard Fire2012 simulation logic, adapted for arbitrary length
    // Step 1.  Cool down every cell a little
    for( int i = 0; i < _length; i++) {
        // Calculate cooling amount.
        // Since update() is called with delta_ms, and Fire2012 assumes roughly 30-60fps,
        // we should scale the cooling.
        // Original: random(0, ((cooling * 10) / NUM_LEDS) + 2)
        // We simplify for this demo:
        uint8_t cooldown = random8(0, ((_cooling * 10) / _length) + 2);
        if(cooldown > _heat[i]) {
            _heat[i] = 0;
        } else {
            _heat[i] = _heat[i] - cooldown;
        }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= _length - 1; k >= 2; k--) {
        _heat[k] = (_heat[k - 1] + _heat[k - 2] + _heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if( random8() < _sparking ) {
        int y = random8(std::min((int)_length, 7));
        _heat[y] = qadd8( _heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors (or just brightness for now)
    // Since PhysicalOutput interface is setValue(brightness), we will just output the Heat value directly.
    // If the user attaches this to a Red LED, it will look like fire brightness.
    // If the user attaches to a NeoPixel, it will be brightness (of the pre-set color).

    // We map the virtual array to the physical outputs.
    // If we have fewer outputs than virtual pixels, we just show the first N.
    // If we have more, we repeat or zero? Let's just map 1:1 up to min count.

    size_t count = std::min((size_t)_length, outputs.size());
    for (size_t i = 0; i < count; i++) {
        // Heat is 0-255.
        outputs[i]->setValue(_heat[i]);
    }
}

}
