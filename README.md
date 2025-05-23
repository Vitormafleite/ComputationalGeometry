Geometric Modeling App

In this project my goal is to setup an app in which the user can load 3D objects and do some operations with them, operations will include. 

-ConvexHull
-Triangulation

Most dependencies are included in the project, however you still want to install OpenGL and GLFW3 support to run this properly.

If you are on mac or windows, you are on your own to set it up.

For linux users, here is a simple install script.

## 🛠 Installation

```bash
$ git clone https://github.com/Vitormafleite/ComputationalGeometry.git
$ cd ComputationalGeometry
$ chmod +x install.sh
$ ./install.sh

## 🛠 Build & Run

create build directory if doesnt exist

$ mkdir -p build
$ cd build

inside build simply run

$ cmake ..
$ make
$ ./build/GeometricModeling



