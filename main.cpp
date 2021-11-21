#include "camera.h"
#include "map.h"
#include "entity.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <set>
#include <cmath>



// -----------------------------------------------------------------------------
// ENTRY POINT
// -----------------------------------------------------------------------------
int main()
{
    // Initialization of GLFW
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed." << '\n';
        std::cin.get();

        return EXIT_FAILURE;
    }

    // Setting window hints
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    //lfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    int width = 0, height = 0, res = 20;
    srand(time(NULL));
    std::ifstream inn("levels/level0");
    if (inn) { inn >> width; inn.ignore(1); inn >> height; }

    GLFWwindow* window = glfwCreateWindow(width * res, height * res, "Pacman", NULL, NULL);

    if (window == nullptr) {
        std::cerr << "GLFW failed on window creation." << '\n';
        std::cin.get();

        glfwTerminate();

        return EXIT_FAILURE;
    }

    // Setting the OpenGL context.
    glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

    Map* level0 = new Map("levels/level0");
    Pacman* Player = new Pacman(level0->getSpawnX(), level0->getSpawnY(), level0);

    std::pair<int, int> ghostPos = level0->getSpawnGhost(width, height);
    Ghost* Red = new Ghost(ghostPos.first, ghostPos.second, level0, Player);
    ghostPos = level0->getSpawnGhost(width, height);
    Ghost* Pink = new Ghost(ghostPos.first,ghostPos.second, level0, Player, 1, 0.013f);
    ghostPos = level0->getSpawnGhost(width, height);
    Ghost* Orange = new Ghost(ghostPos.first, ghostPos.second, level0, Player, 2, 0.024f);
    ghostPos = level0->getSpawnGhost(width, height);
    Ghost* Cyan = new Ghost(ghostPos.first, ghostPos.second, level0, Player, 3, 0.017f);
    


    Shader* mapShader = new Shader("shaders/default.vert", "shaders/default.frag");

    Camera* camera = new Camera(width, height, glm::vec3(width / 2.f, height / 2.f, 44.0f));

    Player->setCamera(camera);

    Red->setCamera(camera);
    Pink->setCamera(camera);
    Orange->setCamera(camera);
    Cyan->setCamera(camera);

    
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        // Scale with aspect ratio
        int tempwidth, tempheight;
        glfwGetFramebufferSize(window, &tempwidth, &tempheight);
        glViewport(0, 0, tempwidth, tempheight);
        glfwSetWindowAspectRatio(window, level0->getWidth(), level0->getHeight());

        // Specify the color of the background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // Clean the back buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Tell OpenGL which Shader Program we want to use

        // draws map
        mapShader->Activate();
        camera->Matrix(45.0f, 0.1f, 100.0f, *mapShader, "camMatrix");
        level0->draw(window);

        // draws Player
        Player->move(window);
        Player->draw();

        // draws Ghost 
        Red->move();
        Red->draw();
        Pink->move();
        Pink->draw();
        Orange->move();
        Orange->draw();
        Cyan->move();
        Cyan->draw();
        
        // Swap the back buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of all GLFW events
        glfwPollEvents();

        if (!level0->getGameStatus())
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}