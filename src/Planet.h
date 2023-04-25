#ifndef Planet_h
#define Planet_h

#include <GLSLProgram.h>
#include <Texture.h>
#include <Mesh.h>
#include "Config.h";
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <unordered_map>

class Planet {
public:
  Planet(float centerX, float centerY, float centerZ, float radius, string planetName);
    ~Planet(){};
  
  // Given latitude and longitude, calculate 3D position
    glm::vec3 getPosition(double latitude, double longitude);
  
  // Calculate and set up the buffers to render to screen
  void setupGeometry();
  
  // Draw the Sun to screen
    void draw(basicgraphics::GLSLProgram &shader);
  // Mesh varriable 
    std::unique_ptr<basicgraphics::Mesh> _mesh;
    std::vector<basicgraphics::Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
protected:
  // Stores the Sun texture data
    std::vector< std::shared_ptr<basicgraphics::Texture> > textures;
    float radius;
    float centerX;
    float centerY;
    float centerZ;
    string planetName;
    

};
#endif
  
