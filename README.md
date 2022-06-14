# MSXVR Unleashed
I am a big fan of the MSXVR. It is a solidly build machine with an injection moulded case and mechanical keys in a retro wedge-shaped design.

But can you run OpenMSX on it? Can you create your own software for it using modern tools and programming languages? Can you run a modern GUI on it and integrate all things MSX with it? Now you can!

After a lot of research I was able to tap into the special MSXVR hardware and create an hardware abstraction layer or API for it. This API I then used to create a command-line-interface that can interact with the cartridges, joysticks, leds, switches, power relais, etc. For example dumping a ROM from a cartridge from the command-line is now a piece of cake. Or create a simple shell script that plays a melody on the SCC or FMPAC.

And last, but not least, I am creating a special build of OpenMSX that runs on the VR as well. This allows us to emulate what we want to emulate and do the rest on real hardware. A kind of hybrid emulator. The machine-config.xml, menu and console allow you to switch this at runtime.

## My wishlist looks like this:

* Access the Buttons and Leds. **done**
* Access the Joysticks. **done**
* Access cartridge ports. **done**
* Get the cartridge sound working. 
* Investigate some timing issues around I/O.
* Finish the OpenMSX integration with real joystick and cassette support.
* Create a Python wrapper for the CLI.
* Run a Pyxel or PICO-8 game from a cartridge.