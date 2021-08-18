# Very Simple OpenGL Setup
Bare minimum external libraries, no need to build external libraries, and, for me at least, no IDE required.
This setup on GitHub is for 64-bit Windows, so if you are using another platform follow the instructions below to create your own. Note that this assumes that you already have a C++ compiler installed.

## GLFW
1. Download pre-compiled binaries at the [official GLFW website download page](https://www.glfw.org/download.html), so there's no need to compile GLFW yourself.
2. Follow the README to see which folder in the .zip you should use. Since I'm not using an IDE, I used the `lib-mingw-w64` folder from the 64-bit windows binaries.

## GLAD
1. GLAD 2 is a web service for generating OpenGL loaders, found [here](https://gen.glad.sh/). The settings I used are:
   - Generator: C++
   - gl: 4.6, the latest version
   - Core, Common
   - No extensions, though you can add any if you want
   - Leave everything unchecked

## Includes
In your sources, if it uses GLFW stuff, include the `dependencies/glfw/include/GLFW/glfw3.h`, and if it uses OpenGL stuff, include the `dependencies/glad/include/glad/gl.h`. Note that the order matters: include GLAD before GLFW, otherwise you'll need to define `GLFW_INCLUDE_NONE` before the GLFW header. See https://www.glfw.org/docs/latest/build_guide.html for more information.

In example.cpp, I used `<this>` instead of `"this"` for the includes because I'm adding custom include paths, so the preprocessor knows where to find these files. Otherwise, `"this"` can be used instead to specify a local path relative to the source's directory.

## Structure
If you are creating your own setup, you may follow the existing file structure shown here on GitHub.

As for the code, it may look bizarre with threading and stuff, but there is a reason why, which I wrote an article on [here](https://lukearcamo.github.io/articles/glfw_freezing.html).

The command I use to compile example.cpp is this: `g++ -o example.exe example.cpp dependencies/glad/src/gl.c dependencies/glfw/lib-mingw-w64/libglfw3.a -lgdi32 -Idependencies/glad/include -Idependencies/glfw/include`. Basically, however you want to compile your code, if you are following this file structure, ensure that you:
- compile with `dependencies/glad/src/gl.c`
- link with the GLFW library files. On my platform, I chose the static library route, so I simply had to link with `dependencies/glfw/lib-mingw-w64/libglfw3.a` and the `gdi32` library.
- add the appropriate include paths, or avoid this altogether and, in your code, just use local paths to include the headers.