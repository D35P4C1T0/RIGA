# Contributing

## Project structure

- `Source/Engine.*`: processing chain, routing and channel state.
- `Source/Tone.*`: filters, EQ and drive.
- `Source/Dynamics.*`: compression and gate/expander processing.
- `Source/Parameters.*`: parameter IDs, ranges, defaults and formatting.
- `Source/PluginProcessor.*`: JUCE integration, buses, automation and settings.
- `Source/PluginEditor.*`: vector editor and parameter attachments.
- `tests/`: DSP regression checks and numerical fixtures.
- `tools/`: offline rendering, render comparison and editor checks.

Keep audio-thread processing free of allocation and blocking operations. Preserve stable parameter IDs and saved-state compatibility when making changes. Keep UI drawing separate from audio processing.

Build and run CTest before submitting DSP changes. For editor changes, enable `STRIP_EDITOR_CHECK`, render a snapshot and check layout and value readability. Format C++ changes using the included `.clang-format` configuration.

Keep build outputs, local recordings and temporary files out of source control. Document any new dependency and its build requirements.
