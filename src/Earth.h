#ifndef Earth_h
#define Earth_h

#include <GLSLProgram.h>
#include "Config.h"
#include <Texture.h>
#include <Mesh.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Earth {
public:
  Earth();
    ~Earth(){};
  
  // Given latitude and longitude, calculate 3D position
    glm::vec3 getPosition(double latitude, double longitude);
  
  // Calculate and set up the buffers to render to screen
  void setupGeometry();
  
  // Draw the Earth to screen
    void draw(basicgraphics::GLSLProgram &shader);
  // Mesh varriable 
    std::unique_ptr<basicgraphics::Mesh> _mesh;
    std::vector<basicgraphics::Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;

protected:
  // Stores the earth texture data
    std::vector< std::shared_ptr<basicgraphics::Texture> > textures;

    // radius and center
    float radius;
    float centerX;
    float centerY;
    float centerZ;
    

};
#endif
  
