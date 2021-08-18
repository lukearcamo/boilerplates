#include <glad/gl.h>
// #define GLFW_DLL
#define GLFW_INCLUDE_NONE // Just to be sure
#include <GLFW/glfw3.h>

#include <iostream>

#include <fstream>
#include <string>
#include <sstream>

#include <thread>
#include <vector>

namespace demo {
    GLFWwindow* window;

    int width = 960, height = 540;
    bool hasResized = false;
    void onResize(GLFWwindow* window, int w, int h) { width = w; height = h; hasResized = true; }

    void onError(int error, const char* description) { std::cerr << "Error" << error << ": " << description << "\n"; }

    int init() {
        glfwSetErrorCallback(onError);
        if (!glfwInit()) {
            std::cerr << "GLFW initialization failed\n";
            return -1;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(width, height, "Example", NULL, NULL);
        if (!window) {
            std::cerr << "Window creation failed\n";
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window); // Setting up GLAD requires active context
        // if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cerr << "GLAD initialization failed\n";
            return -1;
        }
        glfwMakeContextCurrent(NULL); // Will set context on the other thread
        return 0;
    }

    GLuint compileShader(GLenum shaderType, std::string path) {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string code = buffer.str();
        // Can do shader preprocessing here
        const char* source = code.c_str();
        file.close();

        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        GLint success, logSize;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
            GLchar infoLog[logSize];
            glGetShaderInfoLog(shader, logSize, NULL, infoLog);
            std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        }
        return shader;
    }

    void render() {
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, onResize);

        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glEnable(GL_PROGRAM_POINT_SIZE);

        GLuint triangleProgram = glCreateProgram();

        GLuint vs = compileShader(GL_VERTEX_SHADER, "./shaders/triangle/vs.glsl");
        glAttachShader(triangleProgram, vs);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, "./shaders/triangle/fs.glsl");
        glAttachShader(triangleProgram, fs);

        glLinkProgram(triangleProgram);
        glDeleteShader(vs);
        glDeleteShader(fs);

        // glUseProgram(triangleProgram);
        // glUniform3f(glGetUniformLocation(triangleProgram, "color"), value.x, value.y, value.z);

        GLuint triangleVAO;
        glGenVertexArrays(1, &triangleVAO);
        glBindVertexArray(triangleVAO);
            std::vector<GLfloat> positions = {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                0.0f, 0.5f, 0.0f
            };
            GLuint positionsVBO;
            glGenBuffers(1, &positionsVBO);
            glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
                glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), positions.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                glEnableVertexAttribArray(0);

            std::vector<GLubyte> colors = {
                0, 0, 0,
                255, 0, 0,
                0, 255, 0
            };
            GLuint colorsVBO;
            glGenBuffers(1, &colorsVBO);
            glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLubyte), colors.data(), GL_STATIC_DRAW);
                glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
                glEnableVertexAttribArray(1);

        int triangleDrawCount = positions.size() / 3;

        // ======= Render loop =======

        glfwSwapInterval(1);
        float prevFrameTime = glfwGetTime();
        while(!glfwWindowShouldClose(window)) {
            float now = glfwGetTime();
            float deltaTime = now - prevFrameTime; // This is currently unused, but this could be useful for physics, camera movement, etc.
            prevFrameTime = now;

            if (hasResized) {
                glViewport(0, 0, width, height);
                // Here could also be where you update your camera's projection matrix
                hasResized = false;
            }

            glClearColor(0.25, 0.25, 0.25, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glUseProgram(triangleProgram);
            glUniform1f(glGetUniformLocation(triangleProgram, "time"), now);
            glBindVertexArray(triangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, triangleDrawCount);

            glfwSwapBuffers(window);
        }
    }
}
int main() {
    if (demo::init()) return -1;
    std::thread renderThread(demo::render);
    while(!glfwWindowShouldClose(demo::window)) {
        glfwWaitEvents();
    }
    renderThread.join();
    glfwTerminate();
    return 0;
}
// g++ -o example.exe example.cpp dependencies/glad/src/gl.c dependencies/glfw/lib-mingw-w64/libglfw3.a -lgdi32 -Idependencies/glad/include -Idependencies/glfw/include