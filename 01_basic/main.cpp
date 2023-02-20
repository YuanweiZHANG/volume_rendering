#include <glad/glad.h>
#include <openvdb/openvdb.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "glfwFunc.h"
#include "io.h"
#include "shader.h"

#include <iostream>
#include <vector>
#include <string>

// setting
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;

// mouse information
double mouseX = 0;
double mouseY = 0;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// density information
std::vector<float> density; // [x, y, z, density]

// color
glm::vec3 bgColor = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 smokeColor = glm::vec3(1.0f, 1.0f, 1.0f);

// process all input: query GLFW whether relevent keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // reset camera's moving position
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
        firstMouse = true;

    // rotation along one axis
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        float angle = 0.01;
        Axis axis = X_AXIS;
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        {
            axis = X_AXIS;
        }
        else if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            axis = Y_AXIS;
        }
        else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            axis = Z_AXIS;
        }
        else
            return;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            rotateCamera(axis, camera, angle, glm::vec3(0.0f));
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            rotateCamera(axis, camera, -angle, glm::vec3(0.0f));
        }
    }
}

void mouse_button_callback([[maybe_unused]] GLFWwindow *window, int button, 
                           int action, [[maybe_unused]] int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        firstMouse = true;
    }
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = ypos - lastY;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback([[maybe_unused]] GLFWwindow *window, 
                     [[maybe_unused]] double xoffset, 
                     double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(
        static_cast<int>(SCR_WIDTH),
        static_cast<int>(SCR_HEIGHT),
        "Volume Rendering",
        nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    std::string pathSrc = std::string(PATH_SOURCE_DIR);
    Shader smokeShader((pathSrc + "/shaders/volume.vs").c_str(),
                       (pathSrc + "/shaders/volume.fs").c_str());

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};
    
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), 
    // and then configure vertex attribute(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // density attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // read smoke file and generate 3D texture
    int factor = 1;  // to make smoke brighter
    readGrid(pathSrc + "/vdb/smoke.vdb", "density", density);

    unsigned int xMin = 0, xMax = 0, yMin = 0, yMax = 0, zMin = 0, zMax = 0;
    for (decltype(density.size()) i = 0; i < density.size() / 4; ++i)
    {
        if (fabs(density[i * 4 + 3]) < 1e-7)
            continue;
        if (density[i * 4 + 0] < xMin)
            xMin = density[i * 4 + 0];
        else if (density[i * 4 + 0] > xMax)
            xMax = density[i * 4 + 0];
        if (density[i * 4 + 1] < yMin)
            yMin = density[i * 4 + 1];
        else if (density[i * 4 + 1] > yMax)
            yMax = density[i * 4 + 1];
        if (density[i * 4 + 2] < zMin)
            zMin = density[i * 4 + 2];
        else if (density[i * 4 + 2] > zMax)
            zMax = density[i * 4 + 2];
    }

    int xyzMax = std::max(std::max(xMax - xMin, yMax - yMin), zMax - zMin) + 1;
    // printf("xyzMax: %d, density size: %d\n", xyzMax, int(density.size()));
    // printf("x: [%d, %d], y: [%d, %d], z: [%d, %d]\n", xMin, xMax, yMin, yMax, zMin, zMax);

    std::vector<float> textureData(xyzMax * xyzMax * xyzMax, 0.0f);
    for (decltype(density.size()) i = 0; i < density.size() / 4; ++i)
    {
        int x = density[i * 4 + 0] + (xyzMax - xMax - xMin) / 2;
        int y = density[i * 4 + 1] + (xyzMax - yMax - yMin) / 2;
        int z = density[i * 4 + 2] + (xyzMax - zMax - zMin) / 2;
        textureData[x * xyzMax * xyzMax + y * xyzMax + z] = density[i * 4 + 3]*factor;
    }

    glEnable(GL_TEXTURE_3D);

    unsigned int smokeTexture;
    glGenTextures(1, &smokeTexture);
    glBindTexture(GL_TEXTURE_3D, smokeTexture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, xyzMax, xyzMax, xyzMax, 0, GL_RED, GL_FLOAT, textureData.data());
    glGenerateMipmap(GL_TEXTURE_3D);

    smokeShader.use();
    smokeShader.setInt("densityTexture", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view, projection;
        glm::vec3 cameraPos;
        
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        cameraPos = camera.Position;

        // pass them to the shaders
        smokeShader.use();
        smokeShader.setMat4("model", model);
        smokeShader.setMat4("view", view);
        smokeShader.setMat4("projection", projection);
        smokeShader.setVec3("camPos", cameraPos);
        smokeShader.setVec3("bgColor", bgColor);
        smokeShader.setVec3("smokeColor", smokeColor);

        // render smoke
        glBindVertexArray(VAO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}