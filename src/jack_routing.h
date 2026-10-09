/**
 * @file jack_routing.h
 * @brief Which signal drives which CV jack — the pure decision, no hardware.
 *
 * Four jacks can carry the followers: Trig 1, Env 1, Trig 2, Env 2. As
 * printed, Trig 1 / Env 1 carry the input side (every slice, input envelope)
 * and Trig 2 / Env 2 the output side. The Options can flip Trig 1 / Env 1
 * into CV inputs (CV 2 / CV 3); in that mode, and only then, Trig 2 and
 * Env 2 each carry whichever of the four signals the user picks. The 6pm pips
 * never follow any of this — they always show the followers.
 *
 * Out of the box Trig 1 / Env 1 are inputs and Trig 2 / Env 2 carry the input
 * side, so the extra CV costs nothing. A corrupt preset byte falls back to the
 * printed roles instead — outputs, which never fight a patched cable.
 *
 * main.cpp applies the Plan to the hardware; tests/host checks every
 * combination of it, so the logic that ships is the logic that is tested.
 */

#pragma once

#include <cstdint>

namespace jacks {

/* What a jack can carry. Byte values are stored in presets — append only. */
enum Signal : uint8_t
{
    kInTrig  = 0,   // a pulse on every slice the engine takes
    kOutTrig = 1,   // a pulse on each kept output-follower transient
    kInEnv   = 2,   // input follower envelope
    kOutEnv  = 3,   // output follower envelope
    kNumSignals
};

/* Jack order, also the order of Plan's arrays. */
enum Jack : uint8_t { kTrig1 = 0, kEnv1 = 1, kTrig2 = 2, kEnv2 = 3, kNumJacks };

constexpr float kTrigVolts = 5.0f;   // a high trigger
constexpr float kEnvVolts  = 5.0f;   // envelope full scale (0..1 → 0..5 V)

struct Options
{
    bool    cvIn  = true;       // Trig 1 / Env 1 as CV inputs
    uint8_t trig2 = kInTrig;    // what Trig 2 carries in CV-input mode
    uint8_t env2  = kInEnv;     // what Env 2 carries in CV-input mode
};

/* The live follower signals, sampled at the control rate. */
struct Signals
{
    bool  inTrig  = false;   // slice pulse currently high
    bool  outTrig = false;   // output-follower pulse currently high
    float inEnv   = 0.0f;    // 0..1
    float outEnv  = 0.0f;    // 0..1
};

/* Per jack: driven as an output, and if so at what voltage. */
struct Plan
{
    bool  output[kNumJacks];
    float volts [kNumJacks];
};

inline float Volts(uint8_t sig, const Signals& s)
{
    auto env = [](float e) { return (e < 0.0f ? 0.0f : e > 1.0f ? 1.0f : e) * kEnvVolts; };
    switch (sig)
    {
        case kInTrig:  return s.inTrig  ? kTrigVolts : 0.0f;
        case kOutTrig: return s.outTrig ? kTrigVolts : 0.0f;
        case kInEnv:   return env(s.inEnv);
        case kOutEnv:  return env(s.outEnv);
        default:       return 0.0f;
    }
}

inline Plan Route(const Options& o, const Signals& s)
{
    Plan p{};
    p.output[kTrig1] = !o.cvIn;
    p.output[kEnv1]  = !o.cvIn;
    p.output[kTrig2] = true;
    p.output[kEnv2]  = true;
    p.volts[kTrig1]  = o.cvIn ? 0.0f : Volts(kInTrig, s);
    p.volts[kEnv1]   = o.cvIn ? 0.0f : Volts(kInEnv,  s);
    p.volts[kTrig2]  = Volts(o.cvIn ? o.trig2 : (uint8_t)kOutTrig, s);
    p.volts[kEnv2]   = Volts(o.cvIn ? o.env2  : (uint8_t)kOutEnv,  s);
    return p;
}

/* Preset bytes: [0] cvIn, [1] trig2, [2] env2. Anything out of range — a
 * corrupt or foreign record — falls back to the printed roles. */
constexpr uint8_t kBytes = 3;

inline void Encode(const Options& o, uint8_t* out)
{
    out[0] = o.cvIn ? 1 : 0;
    out[1] = o.trig2;
    out[2] = o.env2;
}

inline Options Decode(const uint8_t* in)
{
    Options o;
    o.cvIn  = in[0] == 1;
    o.trig2 = (in[1] < kNumSignals) ? in[1] : (uint8_t)kOutTrig;
    o.env2  = (in[2] < kNumSignals) ? in[2] : (uint8_t)kOutEnv;
    return o;
}

} // namespace jacks
