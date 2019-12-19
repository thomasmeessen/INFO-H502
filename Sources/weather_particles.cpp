#include <cstdlib>
#include "weather_particles.h"

weatherManager::weatherManager(glm::vec3 origin): weather_origin(origin) {
    init();
}

void weatherManager::init() {
    do{
        // decide particle spawn
        particles.emplace_back(weather_particles(weather_origin));
    }while (particles.size()<nb_part);
}

void weatherManager::draw() {
    for(auto part : particles){
        part.update();
    }
}

weather_particles::weather_particles(glm::vec3 spawn_pos) {
    direction = glm::vec3(1,0,0);
    glm::vec3 perpendicular_plane_axe_1 (0,1,0);
    glm::vec3 perpendicular_plane_axe_2 (0,0,1);
    position =  spawn_pos + perpendicular_plane_axe_1 *(10 - (float)(rand()%20))
                + perpendicular_plane_axe_2 * (10 - (float)(rand()%5));
    // a range of speed
    speed = 0.05f + (float)(rand()%10) /100.0f;
}
