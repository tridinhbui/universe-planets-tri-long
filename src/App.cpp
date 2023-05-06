#include "App.h"

#define FONTSTASH_IMPLEMENTATION
#include <fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <glfontstash.h>

#include <config/VRDataIndex.h>

#include <glm/gtx/orthonormalize.hpp>
#include <string>

using namespace basicgraphics;
using namespace std;
using namespace glm;

const int PLAYBACK_WINDOW = 365 * 24 * 60 * 60; // Year in seconds

App::App(int argc, char** argv) : VRApp(argc, argv)
{
	_lastTime = 0.0;
    _curFrameTime = 0.0;
    
    eye_world = glm::vec3(3, 0, 6);
    look_at = glm::vec3(0,0,0);
    mouseDown = false;
    
    // Check that the config.h paths exist
    std::ifstream infile(EARTH_PATH);
    if (!infile.good()) {
        std::cout<< "ERROR: Unable to load the EARTH_PATH: \n\t"<< EARTH_PATH<<std::endl;
        std::cout << "\t Check that the path exists and modify config.h if needed"<<std::endl;
        assert(false);
    }
    else {
        infile.close();
    }

  
        
    std::ifstream infile2(DATA_PATH);
    if (!infile2.good()) {
        std::cout<< "ERROR: Unable to load the DATA_PATH: \n\t"<< DATA_PATH<<std::endl;
        std::cout << "\t Check that the path exists and modify config.h if needed"<<std::endl;
        assert(false);
    }
    else {
        infile2.close();
    }
    
    eqd = EarthquakeDatabase(DATA_PATH);
    playbackScale = 86400;
    currentTime = eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds();
    
    
    rotation = mat4(1.0);
    playing = true;
}

App::~App()
{
	glfonsDelete(fs);
	shutdown();
}

void App::onAnalogChange(const VRAnalogEvent &event) {
    // This routine is called for all Analog_Change events.  Check event->getName()
    // to see exactly which analog input has been changed, and then access the
    // new value with event->getValue().
    
	if (event.getName() == "FrameStart") {
		_lastTime = _curFrameTime;
		_curFrameTime = event.getValue();
	}


}

void App::onButtonDown(const VRButtonEvent &event) {
    // This routine is called for all Button_Down events.  Check event->getName()
    // to see exactly which button has been pressed down.
	
	//std::cout << "ButtonDown: " << event.getName() << std::endl;
    
    string name = event.getName();
    
    // Speedup or slowdown the playback
    if (name == "KbdLeft_Down") {
        look_at = look_at + vec3(-0.5,0,0);
    }
    if (name == "KbdRight_Down") {
        look_at = look_at  + vec3(0.5,0,0);
    }
    
    // Dolly the camera closer or farther away from the earth
    if (name == "KbdUp_Down") {
        vec3 newCamPos = eye_world + vec3(0,0,-0.5);
        if (newCamPos.z > 1.2) {
            eye_world = newCamPos;
        }
    }
    else if (name == "KbdDown_Down") {
        vec3 newCamPos = eye_world + vec3(0,0,0.5);
        eye_world = newCamPos;
    }
    
    // Rotate the earth when the user clicks and drags the mouse
    else if (name == "MouseBtnLeft_Down") {
        mouseDown = true;
    }
}

void App::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.

	//std::cout << "ButtonUp: " << event.getName() << std::endl;
    if(event.getName() == "MouseBtnLeft_Up"){
        mouseDown = false;
    }
}

void App::onCursorMove(const VRCursorEvent &event) {
	// This routine is called for all mouse move events. You can get the absolute position
	// or the relative position within the window scaled 0--1.
	
	//std::cout << "MouseMove: "<< event.getName() << " " << event.getPos()[0] << " " << event.getPos()[1] << std::endl;
    
    if (mouseDown){
        vec2 dxy = vec2(event.getPos()[0], event.getPos()[1]) - lastMousePos;

        if (length(dxy) != 0) {
            float scalingAngleVariable = 1 / 180.0f;
            vec2 perpendicularVec = vec2(dxy.y/2.0f, dxy.x/2.0f);
            vec3 vectorAxis = vec3(perpendicularVec.x, perpendicularVec.y, 0);
            rotation = toMat4(angleAxis(radians(45.0f/360.0f), vectorAxis)) * rotation;
        }
        
        
    }
    lastMousePos = vec2(event.getPos()[0], event.getPos()[1]);
}


void App::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame.
    
	// Is this the first frame that we are rendering after starting the app?
    if (renderState.isInitialRenderCall()) {

		//For windows, we need to initialize a few more things for it to recognize all of the
		// opengl calls.
		#ifndef __APPLE__
			glewExperimental = GL_TRUE;
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Error initializing GLEW." << std::endl;
			}
		#endif     


        glEnable(GL_DEPTH_TEST);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_MULTISAMPLE);

		// This sets the background color that is used to clear the canvas
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();

		initializeText();
    
        
        // Create planet objects
        sun.reset(new Planet(0.0f, 0.0f, 0.0f, 2.0f, "sun"));
        planet0.reset(new Planet(0.0f, 0.0f, 0.0f, 2.0f, "earth"));
        mercury.reset(new Planet(1.158f, 0.0f, -0.2f, 0.2f, "mercury"));
        venus.reset(new Planet(2.164f, 0.0f, -0.4f, 0.5f, "venus"));
        earth.reset(new Planet(2.992f, 0.0f, -0.6f, 0.6f, "earth"));
        moon.reset(new Planet(0.0f, 0.00768f, -0.01f, 0.2f, "moon"));
        mars.reset(new Planet(4.56f, 0.0f, -0.8f, 0.4f, "mars"));
        jupiter.reset(new Planet(15.57f, 0.0f, -1.0f, 1.5f, "jupiter"));
        saturn.reset(new Planet(28.64f, 0.0f, -1.2f, 1.3f, "saturn"));
        uranus.reset(new Planet(57.34f, 0.0f, -1.4f, 1.1f, "uranus"));
        neptune.reset(new Planet(90.3f, 0.0f, -1.6f, 1.2f, "neptune"));
        pluto.reset(new Planet(118.13f, 0.0f, -1.8f, 0.2f, "pluto"));
    }
    
    // Advance the current time and loop back to the start if time is past the last earthquake
    float rdt = _curFrameTime - _lastTime;
    currentTime += playbackScale * rdt;
    std::cout << "current time: \n\t" << currentTime << std::endl;

    // update info for planets moving
    earthPositions = updateEarthPositions(currentTime);
    std::cout << "earth x: \n\t" << earthPositions.x << std::endl;
    std::cout << "y: \n\t" << earthPositions.y << std::endl;
    std::cout << "z: \n\t" << earthPositions.z << std::endl;
    
}

vec3 App::updateEarthPositions(float currentTime) {
    const float earthOrbitRadius = 8.96f;
    const float earthOrbitPeriod = 365.25f;
    float earthOrbitAngle = glm::radians(360.0f * (currentTime / (earthOrbitPeriod * 86400.0f)));

    glm::vec3 earthPosition = glm::vec3(earthOrbitRadius * cos(earthOrbitAngle), 0, earthOrbitRadius * sin(earthOrbitAngle));
    return earthPosition;
}

void App::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the view matrix to set where the camera is located in the scene
    glm::mat4 view = glm::lookAt(eye_world, look_at, glm::vec3(0,1,0));

	// Setup the projection matrix so that things are rendered in perspective
	GLfloat windowHeight = renderState.index().getValue("FramebufferHeight");
	GLfloat windowWidth = renderState.index().getValue("FramebufferWidth");
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
	
	// Setup the model matrix
	glm::mat4 model = rotation;
    
	// Tell opengl we want to use this specific shader.
	_shader.use();
	
	_shader.setUniform("view_mat", view);
	_shader.setUniform("projection_mat", projection);
	
	_shader.setUniform("model_mat", model);
	_shader.setUniform("normal_mat", mat3(transpose(inverse(model))));
	_shader.setUniform("eye_world", eye_world);
    
    
    // Draw the planets
    
    sun->draw(_shader);

    
    mat4 translation = translate(mat4(1.0), earthPositions);
    mat4 newModel = translation * model;
    _shader.setUniform("model_mat", newModel);
    _shader.setUniform("normal_mat", mat3(transpose(inverse(newModel))));
    planet0->draw(_shader);
    
    // earth->draw(_shader);
    // saturn->draw(_shader);
    // venus->draw(_shader);
    // mercury->draw(_shader);
    // moon->draw(_shader);
    // mars->draw(_shader);
    // jupiter->draw(_shader);
    // uranus->draw(_shader);
    // neptune->draw(_shader);
    // pluto->draw(_shader);
        
    }
    




void App::drawText(const std::string text, float xPos, float yPos, GLfloat windowHeight, GLfloat windowWidth) {
	//float lh = 0;
	//fonsVertMetrics(fs, NULL, NULL, &lh);
	//double width = fonsTextBounds(fs, text.c_str(), NULL, NULL) + 40;
	//double height = lh + 40;

	_textShader.use();
	_textShader.setUniform("projection_mat", glm::ortho(0.f, windowWidth, windowHeight, 0.f, -1.f, 1.f));
	_textShader.setUniform("view_mat", glm::mat4(1.0));
	_textShader.setUniform("model_mat", glm::mat4(1.0));
	_textShader.setUniform("lambertian_texture", 0);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	fonsDrawText(fs, xPos, yPos, text.c_str(), NULL);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	_shader.use();

}

void App::reloadShaders()
{
	_shader.compileShader("texture.vert", GLSLShader::VERTEX);
	_shader.compileShader("texture.frag", GLSLShader::FRAGMENT);
	_shader.link();
	_shader.use();
}

void App::initializeText() {
	int fontNormal = FONS_INVALID;
	fs = nullptr;

	fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	if (fs == NULL) {
		assert(false);//Could not create stash
	}

	fontNormal = fonsAddFont(fs, "sans", "DroidSansMono.ttf");
	if (fontNormal == FONS_INVALID) {
		assert(false);// Could not add font normal.
	}

	unsigned int black = glfonsRGBA(255, 255, 255, 255);

	fonsClearState(fs);
	fonsSetSize(fs, 20);
	fonsSetFont(fs, fontNormal);
	fonsSetColor(fs, black);
	fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);

	_textShader.compileShader("textRendering.vert", GLSLShader::VERTEX);
	_textShader.compileShader("textRendering.frag", GLSLShader::FRAGMENT);
	_textShader.link();
}
