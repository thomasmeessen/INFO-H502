#include <cstdlib>
#include <glm/vec4.hpp>
#include "weather_particles.h"

weatherManager::weatherManager(glm::vec3 origin, glm::vec3 direction): weather_origin(origin), weather_direction(direction) {
    init();
}

void weatherManager::init() {

    static const GLfloat g_vertex_buffer_data[] = {
            -1.f, 1.f, 1.f,     // Front-top-left
            1.f, 1.f, 1.f,      // Front-top-right
            -1.f, -1.f, 1.f,    // Front-bottom-left
            1.f, -1.f, 1.f,     // Front-bottom-right
            1.f, -1.f, -1.f,    // Back-bottom-right
            1.f, 1.f, 1.f,      // Front-top-right
            1.f, 1.f, -1.f,     // Back-top-right
            -1.f, 1.f, 1.f,     // Front-top-left
            -1.f, 1.f, -1.f,    // Back-top-left
            -1.f, -1.f, 1.f,    // Front-bottom-left
            -1.f, -1.f, -1.f,   // Back-bottom-left
            1.f, -1.f, -1.f,    // Back-bottom-right
            -1.f, 1.f, -1.f,    // Back-top-left
            1.f, 1.f, -1.f      // Back-top-right
    };

    glGenVertexArrays(1, &particleVAO);
    glBindVertexArray(particleVAO);

    glGenBuffers(1, &particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

// The VBO containing the positions and sizes of the particles

    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    // Initialize with empty buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, nb_particle_max * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    do{
        particles.emplace_back(weather_particles(weather_origin, weather_direction));
        nb_particle++;
    }while (particles.size()<nb_particle_max);
}

std::vector<glm::vec3> weatherManager::draw(Shader shader, glm::mat4 mvp) {


    particlesPos.clear();
    particlesCol.clear();
    auto it = particles.begin();
    while (it != particles.end()){
        it->position += it->direction;
        if (it->position.x < -15){
            particles.erase(it);
            particles.emplace_back(weather_particles(weather_origin, weather_direction));
        } else {
            particlesPos.emplace_back(it->position);
            particlesCol.push_back(it->color);
            ++it;
        }
    }

    shader.use();
    shader.setMatrix4("mvp", mvp);
    glBindVertexArray(particleVAO);
    // ## Setting the pointers to the data and updating the reserved buffers
    // ### Data are all tightly pack arrays
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 2nd attribute buffer : positions of particles' centers
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nb_particle * sizeof(GLfloat) * 3, &particlesPos[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


    // Always reuse the same vertex
    glVertexAttribDivisor(0, 0);
    // Change the position
    glVertexAttribDivisor(1, 1);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 14, nb_particle);
    glBindVertexArray(0);
    return particlesPos;
}

weather_particles::weather_particles(glm::vec3 spawn_pos, glm::vec3 direction) {
    glm::vec3 perpendicular_plane_axe_1 (0,1,0);
    glm::vec3 perpendicular_plane_axe_2 (0,0,1);
    position =  spawn_pos + perpendicular_plane_axe_1 *(1 - (float)(rand()%200)/100.0f)
                + perpendicular_plane_axe_2 * (1 - (float)(rand()%200)/100.0f)
                + direction *(1 - (float)(rand()%40)/10.0f) ;
    // a range of speed
    this->direction = direction * ( 0.03f + (float)(rand()%100) /1000.0f)
            + perpendicular_plane_axe_1 * (0.005f - (float)(rand()%100) /10000.0f)
              + perpendicular_plane_axe_2 * (0.005f - (float)(rand()%100) /10000.0f);
}
