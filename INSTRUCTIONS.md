Step by step how to build SFML:
- Clone or download the repository from https://github.com/SFML/SFML?tab=readme-ov-file and put it anywhere you want.
- Make a new folder inside SFML called build and open a terminal inside that directory, it should be "SFML/build"
- Now prepare the CMake build using "cmake -G "MinGW Makefiles""
- Compile SFML with "mingw32-make"
- Install the compiled SFML so you can use it in your projects using "mingw32-make install", it should go to either C:\Program Files\SFML or C:\Program Files (x86)\SFML
- Check if there is an "include" file, it should be "SFML\include", if yes it is ready to be used. If not, the author of this README will explode.
