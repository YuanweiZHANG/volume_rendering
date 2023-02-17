# volume_rendering
Volume rendering in OpenGL.

## Preparation for the project.
```
git clone https://github.com/YuanweiZHANG/volume_rendering.git
```
### GLFW
```
git submodule update --init external/glfw
cd external/glfw
mkdir build
cd build
cmake ..
make             # windows: cmake --build .
cmake --install . --prefix ../../glfwlib
cd ../../..
```

### GLM
```
git submodule update --init external/glm
cd external/glm
mkdir build
cd build
cmake ..
make             # windows: cmake --build .
cmake --install . --prefix ../../glmlib
cd ../../..
```

### OpenVDB
```
git submodule update --init external/openvdb
cd external/openvdb
mkdir build
cd build
cmake ..
make             # windows: cmake --build .
cmake --install . --prefix ../../openvdblib
cd ../../..
```

## Compile
```
mkdir build
cd build
cmake ..
make             # windows: cmake --build .
```
