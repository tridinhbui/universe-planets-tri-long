#include "App.h"

#define FONTSTASH_IMPLEMENTATION
#include <fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <glfontstash.h>

#include <config/VRDataIndex.h>

#include <glm/gtx/orthonormalize.hpp>

using namespace basicgraphics;
using namespace std;
using namespace glm;

const int PLAYBACK_WINDOW = 365 * 24 * 60 * 60; // Year in seconds

App::App(int argc, char** argv) : VRApp(argc, argv)
{
	_lastTime = 0.0;
    _curFrameTime = 0.0;
    
    eye_world = glm::vec3(0, 0, 4);
    mouseDown = false;
    
    // Check that the config.h paths exist
    std::ifstream infile(TEXTURE_PATH);
    if (!infile.good()) {
        std::cout<< "ERROR: Unable to load the TEXTURE_PATH: \n\t"<< TEXTURE_PATH<<std::endl;
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
        playbackScale /= 1.3;
    }
    if (name == "KbdRight_Down") {
        playbackScale *= 1.3;
    }
    
    // Dolly the camera closer or farther away from the earth
    if (name == "KbdUp_Down") {
        vec3 newCamPos = eye_world + vec3(0,0,-0.01);
        if (newCamPos.z > 1.2) {
            eye_world = newCamPos;
        }
    }
    else if (name == "KbdDown_Down") {
        vec3 newCamPos = eye_world + vec3(0,0,0.01);
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
        // TODO: Update the "rotation" matrix based on how the user has dragged the mouse
        // Note: the mouse movement since the last frame is stored in dxy.

        if (length(dxy) != 0) {
            float scalingAngleVariable = 1 / 180.0f;
            float resultAngle = length(dxy) * scalingAngleVariable;
            vec2 perpendicularVec = vec2(dxy.y, dxy.x);
            vec3 vectorAxis = vec3(perpendicularVec.x, perpendicularVec.y, 0);
            // rotation = toMat4(angleAxis(radians(45.0f / 360.0f), vectorAxis)) * rotation;
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
    
        earth.reset(new Earth());
        sun.reset(new Sun());
 
    }
    
    // Advance the current time and loop back to the start if time is past the last earthquake
    float rdt = _curFrameTime - _lastTime;
    currentTime += playbackScale * rdt;
    if (currentTime > eqd.getByIndex(eqd.getMaxIndex()).getDate().asSeconds()) {
        currentTime = eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds();
    }
    if (currentTime < eqd.getByIndex(eqd.getMinIndex()).getDate().asSeconds()) {
        currentTime = eqd.getByIndex(eqd.getMaxIndex()).getDate().asSeconds();
    }
}

void App::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the view matrix to set where the camera is located in the scene
    glm::mat4 view = glm::lookAt(eye_world, glm::vec3(0,0,0), glm::vec3(0,1,0));

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
    
    
    // Draw the earth
    earth->draw(_shader);
    
    sun->draw(_shader);
        
        
    }
    
    Date d(currentTime);
    stringstream s;
    s << "Current date: " << d.getMonth()
    << "/" << d.getDay()
    << "/" << d.getYear()
    << "  " << d.getHour()
    << ":" << d.getMinute();
    const std::string text = s.str();
    drawText(text, 20, 20, windowHeight, windowWidth);

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
