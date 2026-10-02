# Changelog

All notable changes to Capicola. Versions follow [semver](https://semver.org);
the version string lives in `src/main.cpp` and is stamped into the HostLink
descriptor, so what the programmer reports is always what is running.

## [1.0.0] — 2026-10-01

First public release. Earlier version strings (0.3.0 and below) were never
tagged or published.

### Added

- **Bidirectional stretch.** P2 is bipolar: a true freeze at noon, realtime
  forward at the CW stop, realtime reverse at CCW. The keyframe grid can now
  walk backward through the buffer.
- **Bidirectional pitch.** P1 is grain playback speed, bipolar and linear:
  forward above noon, reversed below, up to 2× either way, and a true stop at
  noon. Unity sits at ¼ and ¾ with a detent holding each. The programmer shows
  the speed directly (e.g. `1.25x`).
- **Ping-pong gesture.** Hold B2 and P1 or P2 becomes a direction control: the
  rate pins where it is and a nudge of the knob flips polarity *at that same
  rate*, instead of dragging it down through the centre and back. A pitch flip
  hands off between the two grains over one crossfade, so it never clicks.
  Release and
  the knob waits behind a catch pip, exactly like a knob after a page switch. A
  CV riding the knob nudges it too, so an LFO into P1 ping-pongs on its own.
- **Buffer-exhaustion handling.** Freezing or reversing long enough that the
  grid runs out of room behind the writer now re-anchors to the delayed tap over
  a full fixed fade. Works on a partially filled ring as well as a lapped one.

### Changed

- **The save gesture moved to B1+B2** (held 0.33 s), mirroring B1+B3 = reset. B2
  alone is the ping-pong gesture, so the two cannot collide — and while B1 is
  down, a knob nudge cannot flip a polarity.
- **Quality (P5) range narrowed** to ε 0.03–0.001, from 0.1–0.001. The old CCW
  end measured around 2.5 dB reconstruction SNR on speech — past crunchy into
  broken — and because the map is linear in ε it consumed half the sweep. The
  whole travel is now usable.

### Fixed

- **Windows only stepped forward.** A cursor leaving its keyframe bracket
  *backward* was never noticed, so the window went stale for the rest of the
  pass and the read stuck at a clamped endpoint — silently, with no click and no
  NaN. Unreachable before bidirectional playback existed; a prerequisite for it.
- **Crossfades never finished at negative pitch.** The splice phase advanced by
  `increment × pitch`, which runs backward when the head does. It now advances
  by `|pitch|`; at exactly zero a fade simply holds until the head moves again.

### Notes

- Saved presets from pre-release firmware will not read correctly: P1, P2 and P5
  all changed meaning. Hold B1+B3 to reset a page to factory defaults, then
  B1+B2 to save.
