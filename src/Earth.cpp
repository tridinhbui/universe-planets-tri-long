#include "Earth.h"

using namespace std;
using namespace glm;
using namespace basicgraphics;

Earth::Earth() {

    // Note: TEXTURE_PATH is set in config.h
    shared_ptr<Texture> tex = Texture::create2DTextureFromFile(TEXTURE_PATH);
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

void Earth::setupGeometry() {
    // TODO: Write your own code here to tesselate and create texture coordinates for the Earth
    // and then save the data to a mesh (i.e. VBO). The tesselation
    // should be based on the STACKS and SLICES parameters.
    const int STACKS = 20;
    const int SLICES = 40;
    // Getting Inspiration from http://www.songho.ca/opengl/gl_sphere.html
    float sectorStep = 2 * glm::pi<float>()/SLICES;
    float stackStep = glm::pi<float>()/STACKS;
    float theta, phi;
    int k1,k2;
    //Also with the inspiration from http://www.songho.ca/opengl/gl_sphere.html
    //k1 - - k1+1
    // |  /   |
    // | /    |
    //k2 - - k2+1
    //Putting all the indexes out there
    for(int i = 0 ; i<= STACKS; i++){
        phi = glm::half_pi<float>() - i * stackStep;
        k1 = i * SLICES;
        k2 = k1 + SLICES+1;
        
        for (int j = 0; j <= SLICES; j++,k1++,k2++) {
            theta = j * sectorStep;
            Mesh::Vertex vert;
            vert.position = vec3(cos(phi)*sin(theta),sin(phi),cos(phi)*cos(theta));
            vert.normal = vec3(cos(phi)*sin(theta),sin(phi),cos(phi)*cos(theta));
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
    
//        Mesh::Vertex vert;
//        vert.position = vec3(1, 1, 0);
//        vert.normal = vec3(0, 0, 1);
//        vert.texCoord0 = glm::vec2(1, 1);
//        cpuVertexArray.push_back(vert);
//        cpuIndexArray.push_back(0);
//
//        Mesh::Vertex vert1;
//        vert1.position = vec3(0, 1, 0);
//        vert1.normal = vec3(0, 0, 1);
//        vert1.texCoord0 = glm::vec2(0, 1);
//        cpuVertexArray.push_back(vert1);
//        cpuIndexArray.push_back(1);
//
//        Mesh::Vertex vert2;
//        vert2.position = vec3(0, 0, 0);
//        vert2.normal = vec3(0, 0, 1);
//        vert2.texCoord0 = glm::vec2(1, 0);
//        cpuVertexArray.push_back(vert2);
//        cpuIndexArray.push_back(2);

}

glm::vec3 Earth::getPosition(double latitude, double longitude) {
    
    // TODO: Given a latitude and longitude as input, return the corresponding 3D x,y,z position on your Earth geometry.
    //Your spherical coordinates should have a radius of 1, and the north pole should map to (0, 1, 0).
    
    latitude = radians(latitude);
    longitude = radians(longitude);
    float x = cos(longitude) * cos(latitude);
    float y = sin(latitude);
    float z = sin(longitude) * cos(latitude);
    
    return vec3(x,y,z); 
}


void Earth::draw(GLSLProgram &shader) {
	// TODO: Draw your mesh.


	_mesh->draw(shader);


//   
    

    
}

