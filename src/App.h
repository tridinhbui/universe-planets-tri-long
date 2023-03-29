#ifndef APP_H
#define APP_H

#include <api/MinVR.h>
using namespace MinVR;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <BasicGraphics.h>
#include "EarthquakeDatabase.h"
#include "Earth.h"
#include "Config.h"

#include <iostream>
#include <sstream>
#include <fstream>


class App : public VRApp {
public:
    
    /** The constructor passes argc, argv, and a MinVR config file on to VRApp.
     */
	App(int argc, char** argv);
    virtual ~App();

    
    /** USER INTERFACE CALLBACKS **/
    virtual void onAnalogChange(const VRAnalogEvent &state);
    virtual void onButtonDown(const VRButtonEvent &state);
    virtual void onButtonUp(const VRButtonEvent &state);
	virtual void onCursorMove(const VRCursorEvent &state);
    
    
    /** RENDERING CALLBACKS **/
    virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    
private:

	double _lastTime;
	double _curFrameTime;
    
    glm::vec3 eye_world;
    bool mouseDown; // Signifies whether the left mouse button is currently held down.
    glm::vec2 lastMousePos;
    
    double currentTime;
    bool playing;
    double playbackScale;
    
    // Database through which you can access the earthquakes
    EarthquakeDatabase eqd;
    
    // Object representing the earth
    std::shared_ptr<Earth> earth;
    
    // Rotation matrix for the earth and displayed earthquakes
    glm::mat4 rotation;
    
    virtual void reloadShaders();
    basicgraphics::GLSLProgram _shader;

	void initializeText();
	void drawText(const std::string text, float xPos, float yPos, GLfloat windowHeight, GLfloat windowWidth);
	struct FONScontext* fs;
    basicgraphics::GLSLProgram _textShader;
};


#endif //APP_H
