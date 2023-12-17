[Co-Compiler](https://github.com/Trico-Everfire/Co-Compiler/tree/master)
A QT standalone map compiler window. It allows you to syncronously call processes. A big difference over the existing map compiler window is that:
- The console is build into the application, no longer opening a separate terminal for the processes.
- Import existing hammer Command Sequences configruations.
- The "Wait for keypress when done compiling." is now a per process. This is the [intended behaviour](https://developer.valvesoftware.com/wiki/Command_Sequences) of the checkbox, with it's placement in the Window and the VDC Wiki backing this up.
- You can set your own global and runtime variables, Allowing you to set up paths, predefine keywords and patch paths if need be.
- Allow for search in console asserts that can warn you or terminate the process list if a match is found.

It's not perfect yet. I want to add a small custom scripting language that allows you to perform asserts more easily and write custom messages for them, write things to a file like the console's output or the total process runtime. I hope to write up documentation for this application, but I've never been good at it. If asked I can explain how stuff works, but taking everything it can do and putting it into a cohesively written document... It scares me. Another thing I'd like to add is the ability to export to a hammer config in case you wanna get back to hammer. (The custom functionality obviously wouldn't work)
