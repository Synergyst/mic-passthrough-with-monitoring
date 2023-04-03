# Microphone passthrough with monitoring
This is a simple [testing] utility to make the Octo AudioInjector (https://www.audioinjector.net/rpi-octo-hat) work as a very basic passthrough device with monitoring ports.

This utility uses Miniaudio (https://github.com/dr-soft/miniaudio) for handling the connection to JACK (some minor modifications will need to be made for use with other audio subsystems (see here for supported audio subsystems: https://github.com/dr-soft/miniaudio#backends).

# Channel mapping
Channel mapping for standard ports:
All 6 left/right input channels are mapped to their respective 6 left/right output channels.

Channel mapping for monitoring ports:
All 3 left input channels are mapped to the left output channel 7 (the left-channel).
All 3 right input channels are mapped to the left output channel 8 (the right-channel).
