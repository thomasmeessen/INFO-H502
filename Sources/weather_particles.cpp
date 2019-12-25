#include <cstdlib>
#include <glm/vec4.hpp>
#include <iostream>
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
    glBufferData(GL_ARRAY_BUFFER, nb_particle_max * sizeof(glm::vec3), nullptr, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &particles_time_collided_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_time_collided_buffer);
    glBufferData(GL_ARRAY_BUFFER, nb_particle_max * sizeof(GLuint), nullptr, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    do{
        particles.emplace_back(weather_particles(weather_origin, weather_direction));
    }while (particles.size()<nb_particle_max);
}

std::vector<glm::vec3> weatherManager::draw(Shader shader, const glm::mat4 &view_projection, const glm::mat4 &model) {

    // COMPUTE COLLISION
    detectCollision(model);

    // BUFFER UPDATE AND PARTICLES MOVEMENT
    particlesPos.clear();
    particlesCol.clear();
    particles_collided_time.clear();

    auto it = particles.begin();
    while (it != particles.end()){
        // UPDATE POSITION AND REMOVED CRASHED && OLD Particles
        if (!it->collided)
        {
            it->position += it->direction;
        } else {
            it->lifetime_collided -= 1;
            if (it->lifetime_collided <= 0){
                particles.erase(it);
            }
        }
        // REMOVE OUT OF SIGHT PARTICLE AND UPDATE BUFFER STEP 1 - create dense struct
        if (it->position.x < -13){
            particles.erase(it);
        } else {
            particlesPos.emplace_back(it->position);
            particlesCol.emplace_back(it->color);
            particles_collided_time.emplace_back(it->lifetime_collided);
            ++it;
        }
    }

    // REFILL TO REACH MAX NUMBER OF PART
    while(particles.size()< nb_particle_max){
        particles.emplace_back(weather_particles(weather_origin, weather_direction));
    }


    // BUFFER STEP 2
    shader.use();
    shader.setMatrix4("view_projection", view_projection);
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
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(GLfloat) * 3, &particlesPos[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 3nd attribute buffer : information about particle explosion
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_time_collided_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(GLuint), &particles_collided_time[0]);
    glVertexAttribIPointer(2, 1, GL_INT, GL_FALSE, 0);


    // Always reuse the same vertex
    glVertexAttribDivisor(0, 0);
    // Change the position
    glVertexAttribDivisor(1, 1);
    // Change the timer
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 14, particles.size());
    glBindVertexArray(0);
    return particlesPos;
}

void weatherManager::detectCollision(const glm::mat4 &model) {
    // Point to cylinder collision detection

    const glm::vec3 dish_pos_forward = glm::vec3( model * glm::vec4(2.8,0,0, 1.0));
    const glm::vec3 dish_pos_backward = glm::vec3( model * glm::vec4(1.8,0,0, 1.0));
    const glm::vec3 dish_pos_center = glm::vec3( model * glm::vec4(2.20,0,0, 1.0));
    const glm::vec3 dish_pos_extremity = glm::vec3( model * glm::vec4(2.56,-1.31,-1.31, 1.0));
    const auto satellite_dish_radius = (float) sqrt(pow(dish_pos_extremity.y, 2) + pow(dish_pos_extremity.z, 2));
    const float linear_correction_amplitude = dish_pos_extremity.x - dish_pos_center.x ;
    auto it = particles.begin();
    while (it != particles.end()){
        if( !it->collided ){
            float axial_position = it->position.x;
            if (dish_pos_backward.x < axial_position && axial_position < dish_pos_forward.x) {
                // axial position
                auto radius = (float) sqrt(pow(it->position.y, 2) + pow(it->position.z, 2));
                if (radius < satellite_dish_radius) {
                    // collision
                    // particle replacement with linear correction for dish shape
                    it->position.x = dish_pos_extremity.x - linear_correction_amplitude * (1 - radius/satellite_dish_radius);
                    it->collided = true;
                }
            }
        }
        ++it;
    }
}


weather_particles::weather_particles(glm::vec3 spawn_pos, glm::vec3 direction) {
    glm::vec3 perpendicular_plane_axe_1 (0,1,0);
    glm::vec3 perpendicular_plane_axe_2 (0,0,1);
    if (rand()%15  == 1){

        position = spawn_pos + perpendicular_plane_axe_1 * (0.16f - (float) (rand() % 160) / 500.0f)
                   + perpendicular_plane_axe_2 * (0.16f - (float) (rand() % 160) / 500.0f);
        this->direction = direction * ( 0.02f + (float)(rand()%100) /1000.0f);
    }
    else {
        position = spawn_pos + perpendicular_plane_axe_1 * (1 - (float) (rand() % 200) / 100.0f)
                   + perpendicular_plane_axe_2 * (1 - (float) (rand() % 200) / 100.0f)
                   + direction * (1 - (float) (rand() % 40) / 10.0f);
        // a range of speed
        this->direction = direction * ( 0.01f + (float)(rand()%100) /1000.0f)
                          + perpendicular_plane_axe_1 * (0.005f - (float)(rand()%100) /10000.0f)
                          + perpendicular_plane_axe_2 * (0.005f - (float)(rand()%100) /10000.0f);
    }

}
