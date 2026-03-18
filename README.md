# ChromeDino-in-Cpp-

A C++ remake of the classic Chrome Dino game, featuring embedded PNG assets and a night/day cycle.  

## Features  

- Smooth gameplay with increasing speed over time  
- Double jump support  
- Obstacles include cactuses and birds  
- Night/Day cycle based on score  
- Embedded assets in `.h` files – no external images required  
- High score tracking  

## Screenshots  

![GameOver Screen](IMG_3692.png)  
![Jumping](IMG_3690.png)  
![Gameplay](IMG_3691.png)  
![default look](IMG_3693.png)  

## Getting Started  

### Requirements  

- C++17 compatible compiler (tested with `clang++` on macOS)  
- [Raylib](https://www.raylib.com/) installed  

### Compiling from Source  

1. Clone the repository:  
```bash
git clone https://github.com/yourusername/ChromeDinoCplusplus.git
cd ChromeDinoCplusplus

2.	Compile the game:

clang++ -std=c++17 -Wall main.cpp -o ChromeDino \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib -lraylib \
    -framework OpenGL -framework Cocoa -framework IOKit \
    -framework CoreAudio -framework CoreVideo \
    -framework CoreFoundation -framework CoreGraphics \
    -framework CoreMedia

3.	Run the game:

./ChromeDino

The embedded .h files contain all the PNG assets, so no external images are needed.

Playable Executable

Precompiled executables are available in the Releases section.

Controls
	•	SPACE: Jump / Double jump
	•	ENTER: Restart after game over
	•	Mouse Click: Restart via reset button

Notes
	•	The game gradually increases in speed to make it more challenging.
	•	The embedded .h files ensure anyone compiling the code will see the correct graphics without needing the original PNG files.

License

This project is licensed under the MIT License. See LICENSE for details.

P.S. I’m a 23-year-old programmer exploring C and C++ for the first time—learning new languages and making games has been really fun!




 





