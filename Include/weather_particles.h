
#ifndef PROJECT_WEATHER_PARTICLES_H
#define PROJECT_WEATHER_PARTICLES_H

#include <glm/vec3.hpp>
#include <vector>

struct weather_particles{
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    explicit weather_particles(glm::vec3 spawn_pos);
    void update(){
        position += direction * speed;
    }
};

class weatherManager {
private:
    std::vector<weather_particles> particles;
    int nb_part = 20;
    glm::vec3 weather_origin;
public:
    explicit weatherManager(glm::vec3 origin);
    void init();
    void draw();
};

#endif //PROJECT_WEATHER_PARTICLES_H
