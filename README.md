# PiBack
Utility application to help RaspberryPi (Debian) exit from Emulators with a combination of two keys of your remote controller. It can be configured with 4 remote controllers and 4 key combinations. It can be applied to any application.

There is a bug with EmulationStation initialisation. If you boot you RaspberryPi system and EmulationStation is on boot init process, EmulationStation needs the gamepads linked and working before start, otherwise EmulationStation can't recognise these gamepads (bluetooth) and ask you for setup again.

For these reason you can use PiBack application, which block system until a gamepad is detected. Once is detected any gamepad (bluetooth), PiBack goes to background and will monitorize every gamepad keypress to allow you exists from any Emulator with a combination of two keys with a 1 second delay. For example you can use PiBack to exit "FCEUX" emulator with START+SELECT keys pressing it 1 second.

### Usage Example

When a user logs in, environment variables are set from various places.  That includes `/etc/profile` (for all users). Then all the files in the `/etc/profile.d` directory and then `~/.profile`. Then `~/.bash_profile`, then `~/.bashrc`. 

Autostart EmulationStation with PiBack on RaspberryPi boot startup. Add the follow to the end of the file `/etc/profile`:

```
[ -n "${SSH_CONNECTION}" ] || /vemu/piback/piback --scan --ini /vemu/piback/piback.ini
[ -n "${SSH_CONNECTION}" ] || nohup /vemu/piback/piback --monitor --ini /vemu/piback/piback.ini > /dev/null 2>&1 &
[ -n "${SSH_CONNECTION}" ] || emulationstation --no-exit > /dev/null 2>&1
```

###  Gamepad Controller (Mapping Codes)

You can then use the following command to list the connected gamepad joysticks:
```
ls /dev/input/js*
```

At least one should be shown, probably /dev/input/js0. Finally you can test the controller is working using the device name from the last command with jstest:
```
jstest /dev/input/js0
```

The numbers shown should change as you move the joysticks around, if so everything is working properly.

### PiBack Configuration

PiBack needs to be configured with a INI file in order to work correctly `piback.ini`. The configuration file has 3 sections: 

* Input devices where the gamepad joysticks are connected, this is generic for most of Linux. 
* The list of applications that will be QUITTED from PiBack when a hotkey is detected. This applications is a list of proccess. To obtain this you can try  `ps aux` in command line to get the name of the process usually it is the same as executable filename.
* List of combinations of keys to detect. All this keys are detected with a delay of 1 seconds before kill an application. The combination of keys you can get from `jstest /dev/input/js0` command and get the codes.

```
[device]

device0_input = /dev/input/js0
device1_input = /dev/input/js1
device2_input = /dev/input/js2
device3_input = /dev/input/js3

[application]

application_process = snes9x,fceux,PicoDrive_rpi2,mame

[hotkey]

hotkey0_button_a = 10
hotkey0_button_b = 11

hotkey1_button_a = 0
hotkey1_button_b = 3

hotkey2_button_a = 0
hotkey2_button_b = 0

hotkey3_button_a = 0
hotkey3_button_b = 0

```


