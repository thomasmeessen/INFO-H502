#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <Shader.hpp>
#include <iostream>
#include "thruster_particles.hpp"




void ParticleManager::draw(Shader particleShader, glm::mat4 mvp) {

    updateBuffer();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    particleShader.use();
    particleShader.setMatrix4("mvp", mvp);
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
    glBufferSubData(GL_ARRAY_BUFFER, 0, nbrParticles * sizeof(GLfloat) * 3, &particlesPos[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // 3rd attribute buffer : particles' colors
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nbrParticles * sizeof(GLfloat) * 4, &particlesCol[0]);
    glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);


    // Always reuse the same vertex
    glVertexAttribDivisor(0, 0);
    // Change the position
    glVertexAttribDivisor(1, 1);
    // Change the color
    glVertexAttribDivisor(2, 1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 14, nbrParticles);
    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleManager::init() {
    // Lazyness cube
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
    glBufferData(GL_ARRAY_BUFFER, nbrParticlesMax * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);

    // The VBO containing the colors of the particles
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, nbrParticlesMax * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    fill();
}

void ParticleManager::fill() {
    while(nbrParticles<nbrParticlesMax*5/10){
        updateBuffer();
    }
}

// -0.007f,0.002 - (float)(rand()%4) /1000.0f, 0.002 - (float)(rand()%4) /1000.0f

void ParticleManager::updateBuffer() {
    if( particlesContainer.size() < nbrParticlesMax - 5){
        for (auto thruster : thruster_position){
            do {
                Particle new_particle(thruster);
                int inv_scale = 300.0f;
                float dx = ((float) (rand() % 20) - 10.0f) / inv_scale, dy =
                        ((float) (rand() % 20) - 10.0f) / inv_scale, dz = ((float) (rand() % 20) - 10.0f) / inv_scale;
                new_particle.Position += glm::vec3(dx, dy, dz);
                new_particle.Velocity += glm::vec3 (-0.007f,0.001 - (float)(rand()%10) /5000.0f, 0.001 - (float)(rand()%10) /5000.0f );
                particlesContainer.push_back(new_particle);
                nbrParticles++;
            }while (rand()%2 );
        }
    }
    //std::cout<<"fin du spawning" << std::endl;
    particlesPos.clear();
    particlesCol.clear();
    auto it = particlesContainer.begin();
    while (it != particlesContainer.end()){
        if ( nbrParticles < nbrParticlesMax - 20){
            it->update_color();
            it->Position += it->Velocity;
            it->life++;
            particlesPos.emplace_back(it->Position);
            particlesCol.emplace_back(it->Color);
            ++it;
        } else {
            particlesContainer.erase(it);
            nbrParticles--;
        }
    }

}
