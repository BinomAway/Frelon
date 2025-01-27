# Frelon

The code for stm32 regroups :
- filters implementation, RIF and IIF
- the audio_application.c, whose default behavior is listening in stereo 48kHz, processing only the right channel with a filter, and lighting a led to signal detection if the energy is sufficient in the watched frequency band
- some old files from the previous group, containing the frequency analysis algorithm (not in use here)
- there's a 0-order blocker feature not yet operational, which would allow to use a 8kHz sampling rate even when the periphal is configured as a 48kHz microphone
- a draft for a double buffering scheme

JSFX scripts are meant to be used within Reaper
(install : drop the scripts somewhere and set the path of Reaper to look up for new plugins there)
- AGC is a port of the original Automatic Gain Control algorithm in C, to see how it behaves in real time
- FIR is a Finite Impulse Response filter, with a custom sized buffer
- IIR is an Infinite Impulse Response filter of order 2. Write your designed coefficients to check how they act

Test_filtre is a folder containing scripts used for designing filters, as well a library called kissFFT and test wav files
