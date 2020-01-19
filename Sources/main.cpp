#include <iostream>
#include "Shader.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Model.hpp"
#include "skybox.hpp"
#include "shadow.hpp"
#include "thruster_particles.hpp"
#include "weather_particles.h"


using namespace std;


bool reset_orientation = true, auto_move = false;
glm::vec3 viewerPos;
float lastX, lastY, yaw, pitch;
glm::vec3 cameraFront;
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
float fov = 45;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_key(GLFWwindow* window, float &angle, float &radius, float &longitude);



int main(int argc, char * argv[]) {

    // Load GLFW and Create a Window
    if (!glfwInit())
    {
        cout << "GLFW Initialization failed " << endl;
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    int scr_width = 1920;
    int scr_height = 1080;
    auto mWindow = glfwCreateWindow(scr_width, scr_height, "INFO-H-502 - Thomas Meessen", nullptr, nullptr);
    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
    // Change Viewport
    int width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);
    glViewport(0, 0, width, height);
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    // ## MOUSE -- camera setting
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);



    // ## SATELLITE
    // -- Model
    string prefix = "../Resources/";
    string strModelPath = prefix + "model/satellite/Satellite.obj";
    const GLchar *modelPath = strModelPath.c_str();
    Model aquaModel(modelPath );
    // -- Reduction in size
    // Natural orientation is X axis
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.09f, 0.09f, 0.09f));
    // -- Shader
    prefix = "../";
    string strTexShaderVertPath = prefix + "Sources/Shaders/satellite.vert";
    string strTexShaderFragPath = prefix + "Sources/Shaders/satellite.frag";
    Shader shader(strTexShaderVertPath.c_str(),
                  strTexShaderFragPath.c_str());
    shader.compile();


    // # Sky-Box Loading
    GLuint skyboxVAO, skyboxTexture;
    Shader skyboxShader = generateSkybox(skyboxVAO, skyboxTexture);
    // Putting the skybox far
    // Skybox ranging from -10 to 10
    glm::mat4 skboxModel = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f));


    // ## SHADOW
    // -- shaders
    string dephtVert = prefix + "Sources/Shaders/dephtSimple.vert";
    string dephtFrag = prefix + "Sources/Shaders/dephtSimple.frag";
    Shader simpleDepht(dephtVert.c_str(), dephtFrag.c_str());
    simpleDepht.compile();
    // -- Texture and VBO generation
    GLuint dephtFBO, depthMap;
    generateShadowFBO(dephtFBO, depthMap);
    shader.setInteger("depthMap", 0);
    // -- Light viewpoint configuration
    glm::vec3 lightPos (0.00f,10.0f,0.001f);
    float near_plane = 5.0f, far_plane = 15.0f;
    glm::mat4 lightProjection = glm::ortho(-0.45f, 0.45f, -0.45f, 0.45f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(
            lightPos,
            glm::vec3( 0.0f, 0.0f,  0.0f),
            glm::vec3( 0.0f, -1.0f,  0.0f));
    glm::mat4 lightSpace = lightProjection * lightView;


    // ## Particles Thruster Effect loading + initialisation
    string partVert = prefix + "Sources/Shaders/particules_thruster.vert";
    string partFrag = prefix + "Sources/Shaders/particules_thruster.frag";
    Shader parShader (partVert.c_str(), partFrag.c_str());
    parShader.compile();
    ParticleManager particleManager;
    particleManager.init();

    // # WEATHER
    // ## Particles weather effect
    string weather_vert = prefix + "Sources/Shaders/particles_athmo.vert";
    string weather_frag = prefix + "Sources/Shaders/particles_athmo.frag";
    string weather_geom = prefix + "Sources/Shaders/particles_athmo.geom";
    Shader weather_shader (weather_vert.c_str(), weather_frag.c_str(), weather_geom.c_str());
    weather_shader.compile();
    // Origin and direction of the flux
    weatherManager weather_manager(glm::vec3(10.0f,0,0), glm::vec3(-1,0,0) );

    // ## Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // ## Camera
    float angle =  180.0f;
    float radius = 1.5;
    float angle2 = 0.0f;

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while (glfwWindowShouldClose(mWindow) == 0) {
        process_key(mWindow, angle, radius, angle2);

        // ### Camera rotation around Y axis
        angle = (auto_move)? angle + 0.0025f: angle;

        viewerPos = (glm::vec3) (glm::vec4(radius,radius ,radius,0.0) * glm::rotate(glm::mat4(1.0f), angle2, glm::vec3(1.0,0.0,0.0))
                * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0,1.0,0.0)));

        if(reset_orientation || auto_move){
            cameraFront = - viewerPos;
        }
        glm::mat4 Projection = glm::perspective(glm::radians(fov), (float) width / (float)height, 0.1f, 100.0f);
        glm::mat4 viewCamera = glm::lookAt(
                viewerPos, // Camera is at variable position, in World Space
                viewerPos + cameraFront, // and looks at the origin
                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        // ### Model rotation around X axis
        Model = glm::rotate(Model,0.01f, glm::vec3(1.0,0.0,0.0));


        // ### Shadow rendering in fbo
        glBindFramebuffer(GL_FRAMEBUFFER, dephtFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepht.use();
        // Provide matrix for model orientation and light source at infinity
        simpleDepht.setMatrix4("LightSpaceModel", lightSpace * Model );
        aquaModel.Draw(simpleDepht);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Close FBO

        // ### Scene cleanup
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ### Space weather effect
        auto luminescent_particles_position = weather_manager.draw(weather_shader, Projection * viewCamera, Model );

        // ### Satellite Rendering
        shader.use();
        // -- Linking the depthMap to reserved 0 slot
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        shader.setVector3f("lightPos", lightPos);
        shader.setFloat("ambientStrength", 0.10);
        glm::mat4 mvp = Projection * viewCamera * Model;
        shader.setMatrix4("MVP", mvp);
        shader.setMatrix4("lightSpace", lightSpace );
        shader.setMatrix4("Model", glm::mat4(Model));
        shader.setVector3f("viewPos", viewerPos );
        aquaModel.Draw(shader);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);


        // ### Render thrust effect
        particleManager.draw(parShader,  Projection * viewCamera * Model );

        // ### Render Sky-Box
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMatrix4("projection", Projection * viewCamera * skboxModel);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(reset_orientation)
    {
    lastX = xpos;
    lastY = ypos;
    pitch = glm::degrees(asin(-glm::normalize(viewerPos).y));
    yaw = glm::degrees(acos(-glm::normalize(viewerPos).x / cos(glm::radians(pitch))));
    cameraFront = glm::normalize(- viewerPos);
    reset_orientation = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
    pitch = 89.0f;
    if(pitch < -89.0f)
    pitch = -89.0f;


    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;
}

void process_key(GLFWwindow* window, float &angle, float &radius, float &longitude){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        reset_orientation = true;
        angle = 0.1f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        angle = 180.0f;
        reset_orientation = true;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        angle = 270.0f;
        reset_orientation = true;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        angle = 90.0f;
        reset_orientation = true;
    }
    if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        auto_move = true;
    }
    if(glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        auto_move = false;
    }

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        angle -= 0.01;
        auto_move = false;
    }

    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        angle += 0.01;
        auto_move = false;
    }

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        radius -= 0.01;
        auto_move = false;
    }

    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        radius += 0.01;
        auto_move = false;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        longitude -= 0.01;
        auto_move = false;
    }

    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        longitude += 0.01;
        auto_move = false;
    }
}