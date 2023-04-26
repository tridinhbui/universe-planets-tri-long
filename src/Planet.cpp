#include "Planet.h"

using namespace std;
using namespace glm;
using namespace basicgraphics;
#include <unordered_map>;

Planet::Planet(float centerX, float centerY, float centerZ, float radius, string planetName) {
    // Set member variables
    this->radius = radius;
    this->centerX = centerX;
    this->centerY = centerY;
    this->centerZ = centerZ;
    this->planetName = planetName;

    unordered_map<string, string> umap;
    umap["earth"] = EARTH_PATH;
    umap["venus"] = VENUS_PATH;
    umap["pluto"] = PLUTO_PATH;
    umap["mercury"] = MERCURY_PATH;
    umap["sun"] = SUN_PATH;
    umap["moon"] = MOON_PATH;
    umap["mars"] = MARS_PATH;
    umap["saturn"] = SATURN_PATH;
    umap["uranus"] = URANUS_PATH;
    umap["neptune"] = NEPTUNE_PATH;
    umap["jupiter"] = JUPITER_PATH;
    
    // Note: TEXTURE_PATH is set in config.h
    shared_ptr<Texture> tex = Texture::create2DTextureFromFile(umap[planetName]);
    tex->setTexParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
    tex->setTexParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);
    tex->setTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    tex->setTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    textures.push_back(tex);
    // Create the vertex and index lists
    std::vector<Mesh::Vertex> cpuVertexArray;
    std::vector<int> cpuIndexArray;
	setupGeometry();
}

void Planet::setupGeometry() {
    const int STACKS = 20;
    const int SLICES = 40;
    float sectorStep = 2 * glm::pi<float>()/SLICES;
    float stackStep = glm::pi<float>()/STACKS;
    float theta, phi;
    int k1,k2;

    for(int i = 0 ; i<= STACKS; i++){
        phi = glm::half_pi<float>() - i * stackStep;
        k1 = i * SLICES;
        k2 = k1 + SLICES+1;
        
        for (int j = 0; j <= SLICES; j++,k1++,k2++) {
            theta = j * sectorStep;
            Mesh::Vertex vert;
            vert.position = vec3(radius * cos(phi) * sin(theta) + centerX, radius * sin(phi) + centerY, radius * cos(phi) * cos(theta) + centerZ);
            vert.normal = vec3(radius * cos(phi) * sin(theta) + centerX, radius * sin(phi) + centerY, radius * cos(phi) * cos(theta) + centerZ);
            vert.texCoord0 = glm::vec2(float(j)/SLICES,float(i)/STACKS);
            cpuVertexArray.push_back(vert);
            //if(i != 0){
                cpuIndexArray.push_back(k1);
                cpuIndexArray.push_back(k2);
                cpuIndexArray.push_back(k1+1);
         //   }
            
          //  if(i != STACKS - 1 ){
                cpuIndexArray.push_back(k1+1);
                cpuIndexArray.push_back(k2);
                cpuIndexArray.push_back(k2+1);

//}
        }
    }
    
    const int numVertices = cpuVertexArray.size();
    const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
    const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();
    _mesh.reset(new Mesh(textures, GL_TRIANGLES, GL_STATIC_DRAW,
        cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray,
        cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
    
}

glm::vec3 Planet::getPosition(double latitude, double longitude) {
    latitude = radians(latitude);
    longitude = radians(longitude);
    float x = cos(longitude) * cos(latitude);
    float y = sin(latitude);
    float z = sin(longitude) * cos(latitude);
    
    return vec3(x,y,z); 
}


void Planet::draw(GLSLProgram &shader) {

	_mesh->draw(shader);


    

    
}

