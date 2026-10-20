# Validation

## Automated checks

The DSP regression suite checks:

- Stereo and mono impulse checkpoints at 48 kHz.
- Monitor gain and polarity.
- Deterministic output across processing block sizes with a fixed noise seed.
- Transparent host bypass.
- Finite output for 200 randomized configurations across 22.05, 44.1, 48, 96 and 192 kHz.

An additional 2,500-configuration tone impulse sweep remained finite. Its largest peak was 0.0807 for a 0.01 input impulse.

The optional editor check exercises A–D setting recall, state restoration, offscreen rendering and buffers larger than the host's declared maximum. Editor snapshots are also visually inspected.

## Platform checks

The macOS Apple Silicon VST3 and standalone bundles build and pass signature verification. The Windows x64 VST3 builds, loads and processes 16,384 stereo frames in a native host with zero reported latency. The Windows DSP regression suite also passes.

## Scope

Selected automation sequences and steady settings have been exercised. Simultaneous changes to every routing, topology and bypass control have not been exhaustively tested. Meter ballistics use a simple peak/reduction display. Noise is stochastic unless a fixed seed is selected in the offline renderer.

## Editor palette

| Controls | Colour |
|---|---|
| High EQ and noise | `#D23431` |
| High-mid EQ | `#4D7E47` |
| Low-mid EQ | `#4381B2` |
| Low EQ, Curve A / B | `#3B3B3B` / `#765142` |
| Filters, main compressor knobs and input | `#E6E6E6` |
| Gate, standard / extended threshold range | `#4D7E47` / `#2FA185` |
| Small trims and drive | `#9B7C43` |

All controls are drawn as vectors. Colour changes affect the editor only.
