# 3DChess

### Requirements
- C++20 (tested with Clang 18)
- CMake (tested with 3.30)
- OpenGL (4.6)
- Linux (haven't tried compiling on Windows)

### How to build
```bash
# Clone the repository and enter the directory
git clone https://github.com/Moztanku/3DChess.git
cd 3DChess

# Create build files and build the project
cmake -S . -B build {-DCMAKE_BUILD_TYPE=Debug/Release}
cmake --build build

# Run the project (optionally provide a path to a board config file like res/boards/standard.cfg)
./build/3DChess {path/to/board/config/file}
```

### How to play
- It's chess.
- Currently doesn't support AI, so you have to play against yourself or another person.
- Possible to use custom board configurations, see `res/boards/` for examples.
- Doesn't implement 50-move rule, 3-fold repetition, or insufficient material draw conditions (yet).

### Controls
- `Left Mouse Button` - Select/Deselect piece, make move
- `Q` - Rotate camera left
- `E` - Rotate camera right
- `A` - Toggle camera auto-rotate
- `T` - Toggle top camera view
- `R` - Undo move
- `M` - Reset chess board

### Used Libraries
- [OpenGL](https://www.opengl.org/)
- [GLAD-2](https://gen.glad.sh/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)
- [STB](https://github.com/nothings/stb)
- [Assimp](https://assimp.org/)

### Credits
- [Low Poly Chess Set](https://sketchfab.com/robie1)
- [Clouds Image](https://www.pexels.com/photo/blue-skies-53594/)
