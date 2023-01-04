# Beatmaster2k

A rudimentary step sequencer based on a Teensy 4.1 and a Novation Launchpad. 
It is mainly intended as a "jamming partner", not a full-fledged production tool.

Some basic features/specs:

- 7 Tracks. Can be set to output midi on serial, device or host ports
- 8 Patterns per track currently, but lots of room for expansion
- 64 Scenes, i.e combination of patterns

- Patterns are edited on the Launchpad. Works during playing
- Patterns can be set to any length between 1 and 128 steps
- Patterns have configurable speed

- Patterns and other settings stored on SD card

Component list:
- Teensy 4.1
- SD card
- ILI9341 display (touch is not used atm)
- 2 encoders
- 8 buttons via an MPC23017 expander
- Powered usb hub
- Novation Launchpad Mini Mk3
