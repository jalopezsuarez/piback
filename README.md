# PiBack
Utility application to help RaspberryPi (Debian) exit from Emulators with a combination of two keys of your remote controller. It can be configured with 4 remote controllers and 4 key combinations. It can be applied to any application.

There is a bug with EmulationStation initialisation. If you boot you RaspberryPi system and EmulationStation is on boot init process, EmulationStation needs the gamepads linked and working before start, otherwise EmulationStation can't recognise these gamepads (bluetooth) and ask you for setup again.

For these reason you can use PiBack application, which block system until a gamepad is detected. Once is detected any gamepad (bluetooth), PiBack goes to background and will monitorize every gamepad keypress to allow you exists from any Emulator with a combination of two keys with a 1 second delay. For example you can use PiBack to exit "FCEUX" emulator with START+SELECT keys pressing it 1 second.
