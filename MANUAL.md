# Capicola — Panel Reference

*Auto-slicer / keyframe time-stretcher, Alchemy Lab V2. Firmware 1.0.0, 2026-10-01.*

> **Interactive version:**
> [heavylight-industries.github.io/capicola/manual.html](https://heavylight-industries.github.io/capicola/manual.html)
> — the same reference drawn on the real faceplate, with live LED state per page.

Capicola listens constantly. Two independent channels (L/R) each run a transient
detector over the input; every detected transient — or a press of **B2**, or a
rising edge on **TRIG IN** — splices the granular playback head onto fresh
material. Pitch and time-stretch are fully decoupled, and the stretch grid
re-anchors on every trigger, so stretched tails ring out underneath while the
output stays locked to the incoming rhythm.

**Both axes run backwards.** P1 reverses the grains, P2 reverses the stretch
grid, and they are independent — so forward grains over a reversing grid, or
reversed grains over a forward one, are settings rather than accidents. The two
combine in ways that are easier to hear than to describe: with only the grid
reversed, each grain still plays forward but starts earlier than the last, so
the material scrubs backward while the timbre stays put; with only the grains
reversed, time runs forward while every slice plays itself backwards. Reverse
both and they partly cancel — the grid and the head travel together, so they
separate slowly and the splices come further apart.

The wet path's latency is the OLA crossfade: **20 ms** by default, adjustable
10–250 ms on the secondary page. The fade is also the retrigger floor —
splices can't come faster.

---

## Getting around

Two page groups, each rotated by a single press of its own button. Each group
remembers where it was left: pressing the *other* group's button jumps straight
to that group's remembered page, and the next press of the same button advances
within the group.

| Button | Pages (in order) | Mode colors |
|---|---|---|
| **B1** | Primary → Depth | **orange** → **pink** |
| **B3** | Routing → Secondary | **blue** → **teal** |

The active page's mode color is worn simultaneously by all three button LEDs.
Button LEDs are never off, so activity feedback is a **dark flash**: B2 blinks
off when a trigger fires, and all three blink off three times to confirm a
settings save or a page reset.

**Pot catch:** every page keeps stored values per knob. After boot or a page
switch, a knob is *decoupled* until the physical pot sweeps through the stored
value (the ring shows a muted pip at the stored position) — then it catches and
tracks. Nothing ever jumps. On the routing page the catch is zone-granular:
entering the stored zone is enough.

**Settings:** hold **B1+B2 for 0.33 s** to save everything (all four pages) as
the boot state — QSPI slot 0, confirmed by the triple dark blink. The module
restores that save at power-up, or falls back to factory defaults if there
isn't one. Hold **B1+B3 for 0.5 s** to reset the *currently visible page* to
factory defaults instead; the saved slot is untouched until the next B1+B2.

B2 on its own is the ping-pong gesture below, so the two never collide — and
while B1 is down, nudging a knob can't flip a polarity.

### Ping-pong — hold B2 and flip direction

> **Hold B2 and P1 or P2 stops being a value and becomes a *direction*
> control.** The rate pins where it was, and a small nudge of the knob either
> way flips polarity *at that same rate* — forward grains to reverse without the
> pitch moving, or the grid's direction without its speed moving.

Crossing noon by hand also reverses a knob, but the rate has to travel down
through the centre and back out to get there. The gesture is the shortcut: the
polarity changes and nothing else does.

A pitch flip never reverses the playing head in place — that would put a corner
in the waveform. The idle grain takes over from the exact same spot in the new
direction and the two crossfade over one fade time, the same ramp a trigger
uses. If a trigger's ramp is already running, the flip lands when it finishes.

The press still fires its slice on the way down, so every flip lands on a fresh
splice — which is most of why it works for fills.

Let go and the knob **stays where the gesture left it**, inert behind a catch
pip, until you sweep the pot through that pip. It is the same decoupling as a
page switch and the pip means the same thing. Press B2 again and the gesture
resumes from what is *playing*, not from where the pot happens to be sitting.

A CV riding the knob nudges it too, so an LFO into P1 with B2 held ping-pongs on
its own, flipping at the LFO's turning points.

### Running out of buffer

The recorder writes continuously into a fixed ring. Anything that stops the
stretch grid keeping up with the writer eventually runs out of room behind it,
and the module handles that the same way every time: **a full fixed-fade
re-anchor to the delayed tap** — i.e. it jumps to "now" and carries on, with a
crossfade rather than a click.

Three ways to get there:

- **Freeze and wait.** At the centre of P2 the grid stops while the writer keeps
  going. Left alone it will eventually hit the back of the ring and re-anchor.
- **Reverse and wait.** The grid walking backward separates from the writer
  faster than a freeze does — at realtime reverse, about twice as fast.
- **Reverse early in a session.** Before the ring has filled there is simply
  nothing older to read, so the grid reaches the oldest frame it has and
  re-anchors there instead.

How long any of this takes is counted in *keyframes, not seconds*, so it depends
entirely on the material — dense, bright input fills the ring far faster than a
sparse one. On busy material expect minutes.

There is a separate, quieter case, only in the first moments after power-up: a
reversed **read head** with a long grain can walk back to the very start of the
recording before its grain ends. It holds the oldest sample there — silence,
not a click — until the next splice or trigger moves it.

---

## Primary (B1 · page 1 · orange)

| Knob | Function | Range | Notes |
|---|---|---|---|
| P1 | Pitch | ±2×, bipolar | Playback speed of the grains, linear. CW of noon they play **forward**, CCW **in reverse**, up to 2× either way. **Noon stops the head.** Unity sits at ¼ and ¾, with a detent holding each. Grain speed and direction only — the stretch clock keeps its own. |
| P2 | Stretch | ±1.0, bipolar | **Freeze at noon** (the grid stops). CW → realtime forward, CCW → realtime **reverse**; ¾ up ≈ 6× slow. Taper is \|u\|^2.5 each side. |
| P3 | Threshold | ratio 0–8 | See below — it's relative, not absolute. |
| P4 | Grain size | 32–4096 keyframes | The adaptive splice leash. |
| P5 | Quality | ε 0.03 → 0.001 | Analyzer keyframe threshold. CCW = crunchy sparse reconstruction, CW = max fidelity. |
| P6 | Feedback | 0–1.5 loop gain | See below. |

**P3 — threshold is self-calibrating.** The detector tracks a running average
of its own envelope and keeps events at `knob × average` — a *ratio*, not an
absolute level, so it needs no gain-staging and survives level changes. The
first 90% of the sweep spans ratio 0 (keep everything) to 4 (transients only);
the last stretch climbs to 8 (very picky). **The very top of the knob
hard-mutes auto-triggering** — the module keeps playing its current material
and only B2 or TRIG IN splices in new audio.

**P6 — feedback.** The previous block's *wet* output → sinc saturator (table
lookup, unity small-signal gain) → state-variable **bandpass** (centre set by
the secondary page's P6, ~480 Hz by default) → back into the recorder input,
where it gets re-sliced and re-pitched. The knob is loop gain: **above 1.0 is
deliberately unstable** (chaos zone), with a hard ±1 clamp on the injection as
the runaway guard. The feedback tap sits *before* the mix blend, so turning mix
down doesn't starve the loop.

**6-o'clock pips** (always on, every page): a live mirror of the four output
jacks — TRIG 1 (P3), TRIG 2 (P4), in-envelope (P5, turns red when the input
clips), out-envelope (P6). The two trig pips run off the same events as the
jacks, held 60 ms so a 10 ms pulse is visible at a glance; P3 therefore lights
on a B2 press or a TRIG IN edge exactly as the jack does.

---

## Depth (B1 · page 2 · pink)

Each pot is the **bipolar modulation depth** for the same-numbered performance
knob: noon = off, CW (pink) = positive, CCW (orange) = negative. A faint
page-hued glow marks a centered (inactive) depth. B1+B3 zeroes the whole page.

Modulation is applied as an offset to the primary knob's *position* —
`source × depth` — then runs through that knob's normal range mapping. Follower
sources are unipolar 0..1; the CV IN jack is bipolar ±1.

---

## Routing (B3 · page 1 · blue)

Each pot is a **3-zone selector** choosing the mod source for the same-numbered
performance knob, CCW → CW:

| Zone | Source | Color | Character |
|---|---|---|---|
| 1 | Input follower | orange | unipolar 0..1 |
| 2 | Output follower | blue | unipolar 0..1 |
| 3 | CV IN jack | violet | bipolar ±1 |

The ring always shows all three zones dim with the selected zone at full
brightness — it doubles as the position map. Default source is the **input**
follower (`RoutingStore::zone` is all zeroes, and B1+B3 resets to zone 0), so
out of the box the module modulates from what you feed it. Selecting the
*output* follower instead makes a knob self-modulating, since that envelope
reflects what the module is already doing — depth alone then closes a loop
through the modulation.

The two followers are the detectors' own smoothed TKEO envelopes (input side =
max of L/R; output side runs on the final post-mix mono sum). They share the
P3 threshold and the smoothing, but the output follower never triggers
splices — it's a signal source only.

---

## Secondary (B3 · page 2 · teal)

Deeper voicing controls. Saved with everything else in the preset store.

| Knob | Function | Range | Default |
|---|---|---|---|
| P1 | Envelope smoothing | follower cutoff, exp — ~1 Hz at CCW | ~34 Hz |
| P2 | Fade | OLA crossfade 10–250 ms, exp — latency, retrigger floor and punch length in one | 20 ms |
| P3 | Drive | keyframe shaper gain 0.5–4.0 | 1.0 |
| P4 | Drive character | quake ← clean → sinc | full sinc |
| P5 | Mix | dry/wet | full wet |
| P6 | Feedback tone | feedback bandpass centre, exp | ~480 Hz |

**P3/P4 — the keyframe waveshaper.** Drive is applied to the *keyframes
themselves*, before interpolation — the nonlinearity never sees the sample
rate, so it can't fold aliases into the band no matter how hard you push it.
Character morphs the shape: full CCW is the quake curve (saturates hard but
drops out near silence — gated, gnarly), noon is clean bypass, full CW is the
sinc shaper (the default coloration).

**P2 — fade.** One knob, three tied consequences: the wet path's latency, the
splice crossfade length, and the retrigger floor. Short = snappy, clicky
splices at high rates; long = smeared, washy takeovers that cap the splice
rate at a few Hz. Changing it never moves a playing grain — the new length
applies from the next splice on.

**P5 — mix.** The wet path lags the dry by the fade, so mid-mix settings
comb/phase against sustained material — expected, and a sound of its own at
unity pitch. Full wet is the classic Capicola behavior.

---

## Buttons

| | Idle | Action |
|---|---|---|
| **B1** | mode color | Click: primary ↔ depth (or return to the B1 group). Held with B2 0.33 s: save boot state. Held with B3 0.5 s: reset visible page. |
| **B2** | mode color | Press: force-trigger a splice on both channels (works even at threshold mute). Held: ping-pong gesture on P1/P2 — nudge the knob to flip playback direction at a constant rate. Dark blink on trigger. |
| **B3** | mode color | Click: routing ↔ secondary (or return to the B3 group). |

---

## Jacks (bottom 2×5 grid)

| | Left | | | | Right |
|---|---|---|---|---|---|
| top | **IN L** | **TRIG IN** | **TRIG 1** out — slice | **TRIG 2** out — out-transient | **OUT L** |
| bottom | **IN R** | **CV IN** | **ENV 1** out — in-env | **ENV 2** out — out-env | **OUT R** |

- **TRIG IN:** external slice trigger — rising edge (fires ~+2 V, re-arms
  ~+1 V), always accepted, even at threshold mute. Shares B2's dark flash.
- **CV IN:** the external mod source, eurorack bipolar, routable to any knob
  via the routing page.
- **Trig outs:** 0/5 V pulses, 10 ms, one per event. **TRIG 1** fires on every
  splice the engine actually takes — auto-detected transients, B2 presses and
  TRIG IN alike. **TRIG 2** fires on the output follower's kept transients.
  Neither can pulse faster than the detector's 150 ms hold-off, and a long fade
  thins TRIG 1 further (the fade is the retrigger floor), so the jack always
  reports what the engine did rather than what the detector saw.
- **Env outs:** the follower envelopes, 0–5 V — the same signals available
  internally as mod sources, mirrored on the P5/P6 pips.

Output is the mix; there is no dry normalling in hardware.

---

## Fixed internals

| Constant | Value | What it is |
|---|---|---|
| `kFbSatGain` | 0.5 | feedback saturator scaling (unity small-signal loop gain) |
| `kMaxDriftSamples` | 48000 (1 s) | stereo re-alignment guardrail — if one channel auto-fires and the two grids have drifted more than 1 s apart, the quiet channel is forced to re-anchor. Direction-agnostic: it measures the gap, not which way either grid is going |
| boundary keyframes | gap-gated, fade/2 | coverage guard frames, written only when no extremum has been logged recently |
| `kPitchMax` | 2× | grain speed at either end of P1 |
| `kPitchDetent` | ±0.03× | how wide the unity hold is at each of P1's ¼ and ¾ points |
| `kGestureNudge` | 1.5% of sweep | how far a knob must move under a held B2 to count as a polarity flip |
