# RolandControl

This is a VST plugin created with JUCE and the Windows MIDI API.
It is specifically designed to output MIDI to a Roland FP-30 to control tone selection, and tone and effect properties.

The capabilities of the VST are based on the FP-30's MIDI Implementation document, some features of which are included in the General MIDI 2 standard.
Tones are defined by a JSON file, which include tone categories. Each tone is defined by a name and 3 numbers for MIDI program counter, bank MSB, and bank MSB values.

The FP-30 includes Reverb and Chorus effects which are always active, and a third effect "EFX" which can be one of several predefined effects.
EFX are also defined by a JSON file, including effects separated into categories. Each effect defines its own parameters, allowing for a custom UI for each individual effect.
The position, size, type, and display value properties of each parameter are controllable.

Although this software was designed specifically for the FP-30, some features may work with all GM2-compatible devices or other Roland devices.

Features include:
- Selecting FP-30's 16 MIDI channels
- Modifying tone GM2 parameters including filters, vibrato, and effect send values
- Changing Reverb and Chorus parameters
- Selecting EFX and setting EFX send to Reverb/Chorus
- Modifying EFX parameters

Note that since this project was created for personal use, some variables and file paths are hardcoded to certain values.
