

#ifndef PROJECT_THRUSTER_PARTICLES_HPP
#define PROJECT_THRUSTER_PARTICLES_HPP

struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    int life = 0;
    Particle(glm::vec3 pos)
            : Position(pos), Velocity(-0.007f,0.0f,0.0f), Color(0.0f,0.99f,0.8f,1.0f) { }
    void update_color(){
        glm::vec4 purple = glm::vec4(128.0f/255.0f,0.0f,107.0/255.0f,0.9), yellow = glm::vec4(1.0f,1.0f,0,0.4);
        glm::vec4 yellow_transparant = glm::vec4(255,255,0,0.2);
        Color = purple * (1.0f-(float)life/250.0f) + yellow * (float)life/250.0f;

    }
};

class Shader;

class ParticleManager {
private:
    std::vector<Particle> thruster_position {
        glm::vec3(-1.68f, 0.12f, -0.12f),
        glm::vec3(-1.68f, 0.12f, 0.12f),
        glm::vec3(-1.68f, -0.12f, -0.12f),
        glm::vec3(-1.68f, -0.12f, 0.12f)};
    std::vector<Particle> particlesContainer;
    std::vector<glm::vec3> particlesPos;
    std::vector<glm::vec4> particlesCol;
    int nbrParticlesMax = 1100;
    int nbrParticles = 0;
    GLuint particleVAO;
    GLuint particles_position_buffer;
    GLuint particles_color_buffer;
    GLuint  particleVBO;

    void updateBuffer();
public:
    ParticleManager()= default;
    void init();

    void draw(Shader shader, glm::mat4 mvp);

    void fill();
};

#endif //PROJECT_THRUSTER_PARTICLES_HPP
