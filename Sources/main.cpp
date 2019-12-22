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
    auto mWindow = glfwCreateWindow(1920, 1080, "INFO-H-502 - Thomas Meessen", nullptr, nullptr);
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
    // -- Light viewpoint configuration
    glm::vec3 lightPos (0.01f,30.0f,0.01f); // Far to get te impression of // rays
    float near_plane = 25.0f, far_plane = 35.0f;
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
    Shader weather_shader (weather_vert.c_str(), weather_frag.c_str());
    weather_shader.compile();
    // Origin and direction of the flux
    weatherManager weather_manager(glm::vec3(10.0f,0,0), glm::vec3(-1,0,0) );

    // ## Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // ## Camera
    float angle = 0;
    glm::mat4 viewCamera;
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);



    int  sens_angle = 1;
    while (glfwWindowShouldClose(mWindow) == 0) {

        // ### Camera rotation around Y axis
        angle = (sens_angle == 1)? angle +0.003f: angle -0.003f;
        sens_angle = (abs(angle) > 6.0)? sens_angle *-1: sens_angle;
        glm::vec3 viewerPos (1.8*cos(angle),1.5,1.8*sin(angle));
        viewCamera = glm::lookAt(
                viewerPos, // Camera is at variable position, in World Space
                glm::vec3(0.0f,0.0f,0.0f), // and looks at the origin
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
        glUniform1i(glGetUniformLocation(shader.ID, "depthMap"), 0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.setVector3f("lightPos", lightPos);
        shader.setFloat("ambientStrength", 0.13);
        glm::mat4 mvp = Projection * viewCamera * Model;
        shader.setMatrix4("MVP", mvp);
        shader.setMatrix4("lightSpace", lightSpace );
        shader.setMatrix4("Model", glm::mat4(Model));
        shader.setVector3f("viewPos",viewerPos );
        shader.setList3("light_part_list", luminescent_particles_position);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        aquaModel.Draw(shader);


        // ### Render thrust effect
        particleManager.draw(parShader,  Projection * viewCamera * Model );




        // ### Render Sky-Box
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setMatrix4("projection", Projection * viewCamera * skboxModel);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
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



