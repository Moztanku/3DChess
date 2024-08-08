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
cmake -S . -B build
cmake --build build

# Run the project
./build/3DChess
```

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