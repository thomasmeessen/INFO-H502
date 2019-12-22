
#ifndef PROJECT_WEATHER_PARTICLES_H
#define PROJECT_WEATHER_PARTICLES_H

#include <glm/vec3.hpp>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Shader.hpp"

struct weather_particles{
    glm::vec3 position;
    glm::vec3 direction;
    bool collided = false;
    int lifetime_collided = 100;
    glm::vec4 color = glm::vec4 (0.5,0.5,0.8,1.0);
    explicit weather_particles(glm::vec3 spawn_pos, glm::vec3 direction);
};

class weatherManager {
private:
    std::vector<weather_particles> particles;
    std::vector<glm::vec3> particlesPos;
    std::vector<glm::vec4> particlesCol;
    int nb_particle_max = 300;
    glm::vec3 weather_origin, weather_direction;
    GLuint particleVAO;
    GLuint particles_position_buffer;
    GLuint particleVBO;

    void detectCollision(const glm::mat4 &Model);
public:
    explicit weatherManager(glm::vec3 origin, glm::vec3 direction);
    void init();
    std::vector<glm::vec3> draw(Shader shader,const glm::mat4 &view_projection,const glm::mat4 &Model);


};

#endif //PROJECT_WEATHER_PARTICLES_H
