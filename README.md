# Chemu8
A simple Chip8 emulator written in C++ using SDL3.

## How to use
You can run the program through the terminal. Simply open a terminal in the folder with the .exe file and use the following command: .\Chemu8.exe .\programNameHere

Replace the programNameHere with the program you want to boot up in the emulator. Make sure the program is in the same folder as the exe file. If it is in a deeper folder then add the entire path, starting from the folder the exe is in. Eg. If you store your programs in a sub-folder called ROMS then you would provide .\ROMS\ProgramNameHere instead.

The controls are mapped as follows:
Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+

For the provided tetris program the CHIP-8 keypad controls are:
- 6: move block right
- 5: move block left
- 4: rotate block
- 7: soft drop (move the block down faster)

## Project Structure
The emulator exists out of 6 separate parts as well as a main loop.

### Main
The main function is repsonsible for creating the 6 main parts of the program as well as handeling the main loop and executing the respective parts their Update functions, as well as cleaning up once the main loop is ended.

### CPU
The CPU is the beating heart of the program. It fetches opcodes from the memory and then executes them.

The execution of opcodes happens through function tables. This way we can use the opcode as an index by selecting specific nibbles of the opcode each step of the way. The alternative to this method would be to write a massive switch case but I find the function table to be more readable, even with the low amount of 35 opcodes.

Depending on the opcode the CPU will also have to interact with other parts of the project. These parts are the Memory, the Graphics, InputHandler and Audio.

### Memory
The memory holds all of the emulated memory. The CHIP-8 functions purely on RAM memory. I chose to allocate 4kb of RAM, which would be accurate fort the devices it would have ran on. On top of this the memory also holds the call stack.

### InputHandler
The InputHandler handles the input. I initially did this using a big switch statement to map corresponding SDL key codes to the keypad of the CHIP-8. I later reworked this to instead use a hashmap (std::unordered_map) to map the SDL key codes to their corresponding keypad index values and then simply use this to set or unset the keypad keys. This makes it far more readable and easier to debug than using a switch statement.

### Graphics
The Graphics store all the emulator parts to render graphics. I specifically made this class to separate the SDL rendering code from the emulation of the graphics of the original hardware as much as possible.

In essence, this holds the screen's pixel states. The CHIP-8 doesn't use colours so each pixel is simply on or off. The CPU then sets this screen when using the drawing opcode.

### Renderer
The Renderer handles the actual rendering to your screen using SDL3. It creates the SDL window with specified dimensions and then translates the pixels stored in Graphics to the correct dimensions for this SDL window and then uses SDL to actual render them onto the window.

### Audio
The Audio class handles the playing of audio using SDL3. It initializes the audiostream and allows the CPU to stop or start it. Since the CHIP-8 makes noise through a simple one-tone buzzer the Audio class also only emits a single tone.

## More Info on the CHIP-8
If you want more information on the CHIP-8 and CHIP-8 emulation then there are some great resources out there.
- For a great technical reference doc that doesn't overcomplicate it: https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
- For information on the available opcodes: https://chip8.gulrak.net/
- A fantastic test-suite with a bunch of useful test programs and some extra information on the CHIP-8 specific quirks: https://github.com/Timendus/chip8-test-suite

## Programs
The release build on here comes together with a simple tetris program but if you would like some other programs you could always run the test programs of the test-suite mentioned above.

For some additional games you can always look on https://johnearnest.github.io/chip8Archive/.
