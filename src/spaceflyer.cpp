
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>


#include "glad.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define IRRKLANG_STATIC
#include "irrklang/irrKlang.h"

#include "util.hpp"
#include "PerlinNoise.hpp"
#include "spacecraft.hpp"
#include "vertices.h"



#define FULLSCREEN

#ifdef FULLSCREEN
#define WIDTH 1920
#define HEIGHT 1080
#else
#define WIDTH 800
#define HEIGHT 600
#endif

#define SENSITIVITY 0.1f

#define W_SCALE 10.0f

#pragma comment(lib, "irrKlang.lib") 

float width, height;
glm::vec3 cam_eye = glm::vec3(1.0f, 0.0f, 0.0f), cam_up = glm::vec3(0.0f, 1.0f, 0.0f), cam_pos;
Spacecraft Bolide;
bool first = true;

void resize_callback(GLFWwindow* window, int w, int h);  
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void immediate_input(GLFWwindow* window);



int main()
{
	srand(time(NULL));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	width = WIDTH;
	height =  HEIGHT;
	//FULLSCREEN
	#ifdef FULLSCREEN
	GLFWwindow* window = glfwCreateWindow(width, height, "space", glfwGetPrimaryMonitor(), NULL);
	//NOT FULLSCREEN
	#else
	GLFWwindow* window = glfwCreateWindow(width, height, "space", NULL, NULL);
	#endif


	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED | GLFW_CURSOR_HIDDEN);

	glfwMakeContextCurrent(window);


	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	//no openGL function before this line /!\
	//callbacks linking
	glfwSetFramebufferSizeCallback(window, resize_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	
	//opengl init stuff
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	//framebuffer for hdr
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int colorBuffer[2];
	glGenTextures(2, colorBuffer);
	for(int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, colorBuffer[i], 0);
	}

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

    }

    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
    irrklang::ISound* ambiance = engine->play2D("sound/space_ambiance.wav", true, false, true);
    irrklang::ISound* motor = engine->play2D("sound/engine2.wav", true, false, true);
    irrklang::ISound* error = engine->play2D("sound/alarm.wav", true, false, true);
   	ambiance->setVolume(0.5f);
   	motor->setVolume(0.0f);
   	error->setVolume(0.0f);

	Shader normalShader = Shader("shaders/normal.vert", "shaders/normal.frag");
	Shader skyboxShader = Shader("shaders/skybox.vert", "shaders/skybox.frag");
	Shader postShader = Shader("shaders/post.vert", "shaders/post.frag");
	Shader blurShader = Shader("shaders/blur.vert", "shaders/blur.frag");

	Cubemap skyboxCubemap = Cubemap("textures/bkg/blue/front.jpg", "textures/bkg/blue/back.jpg",
									"textures/bkg/blue/left.jpg","textures/bkg/blue/right.jpg",
									"textures/bkg/blue/top.jpg","textures/bkg/blue/bot.jpg");

	Texture Flame = Texture("textures/flame.jpg");
	Texture Trail = Texture("textures/trail.jpg");
	Texture Craft_tex = Texture("textures/craft.jpg");
	Texture Lightmap = Texture("textures/lightmap.jpg");
	Texture Reflectionmap = Texture("textures/refmap.jpg");
	Texture Black = Texture("textures/black.jpg");
	Texture White = Texture("textures/white.jpg");
	Texture Rock1 = Texture("textures/rock1.jpg");
	Texture Rock2 = Texture("textures/rock2.jpg");
	Texture Rock3 = Texture("textures/rock3.jpg");
	Texture Rock4 = Texture("textures/rock4.jpg");

	Solid Tri = Solid(tri_vertices, sizeof(tri_vertices), VERT_TEX);
	Solid Quad = Solid(quad_vertices, sizeof(quad_vertices), VERT_TEX);
	Solid Cube = Solid(cube_vertices, sizeof(cube_vertices), VERT);
	Solid Craft = Solid(craft_vertices, sizeof(craft_vertices), VERT_NORM_TEX);
	Solid Asteroid1 = Solid(asteroid1_vertices, sizeof(asteroid1_vertices), VERT_NORM_TEX);
	Solid Asteroid2 = Solid(asteroid2_vertices, sizeof(asteroid2_vertices), VERT_NORM_TEX);

	Bolide = Spacecraft();

	siv::PerlinNoise Noise = siv::PerlinNoise(rand());

	Asteroid Asteroids[1000];
	int a = 0;
	Texture tex;
	

	for(int x = 0; x < 10; x++) {
		for(int y = 0; y < 10; y++) {
			for(int z = 0; z < 10; z++) {
				glm::vec3 aPos = W_SCALE*glm::vec3((x*10 + 5.0f*(float)rand()/RAND_MAX - 10.0f)-50, (y*10  + 5.0f*(float)rand()/RAND_MAX - 10.0f)-50, (z*10 + 5.0f*(float)rand()/RAND_MAX - 10.0f)-50);
				if(glm::length(aPos) > 1000) {
					aPos = 10.0f*W_SCALE*glm::vec3(((float)rand()/RAND_MAX-0.5), ((float)rand()/RAND_MAX-0.5), ((float)rand()/RAND_MAX-0.5));
				}
				Solid type;
				double freq = 1000.0;
				if(rand()%2 == 0) {
					type = Asteroid1;
				} else {
					type = Asteroid2;
				}
				if(Noise.noise0_1(aPos.x / freq, aPos.y / freq, aPos.z / freq) < 0.25) {
					tex = Rock1;
				} else if(Noise.noise0_1(aPos.x / freq, aPos.y / freq, aPos.z / freq) < 0.5) {
					tex = Rock2;
				} else if(Noise.noise0_1(aPos.x / freq, aPos.y / freq, aPos.z / freq) < 0.75) {
					tex = Rock3;
				} else {
					tex = Rock4;
				}
				Asteroids[a] = Asteroid(aPos, type, tex, Black, Black);
				Asteroids[a].setRot(glm::vec3((float)rand()/RAND_MAX - 1.0f, (float)rand()/RAND_MAX - 1.0f, (float)rand()/RAND_MAX - 1.0f), (float)rand()/RAND_MAX*0.5f);
				Asteroids[a].setScale(W_SCALE*glm::vec3((float)rand()/RAND_MAX + 0.7f, (float)rand()/RAND_MAX + 0.7f, (float)rand()/RAND_MAX + 0.7f));
				a++;
			}
		}
	}

	#define NB_TRAIL 70

	glm::mat4 trailMat[NB_TRAIL];
	int trailCount = 0;


	normalShader.use();
	normalShader.uniform1i("Texture", 0);
	normalShader.uniform1i("Lightmap", 1);
	normalShader.uniform1i("Reflectmap", 2);
	normalShader.uniform1i("Skybox", 3);
	normalShader.uniform1i("JustLight", 4);

	postShader.use();
	postShader.uniform1i("Texture", 0);
	postShader.uniform1i("Blur", 1);


	
	glm::mat4 view;

	motor->setVolume(0.2);
	motor->setPlaybackSpeed(1.5f);
	float time = 0;
	//mainloop!
	while(!glfwWindowShouldClose(window)) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		normalShader.use();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		immediate_input(window);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 1000.0f);
		glm::mat4 model = glm::mat4(1.0f);
		

		skyboxShader.use();
		view = glm::lookAt(glm::vec3(0.0f), cam_eye , cam_up);
		skyboxShader.uniformMat4f("projection", projection);
		skyboxShader.uniformMat4f("view", view);
		glDepthMask(GL_FALSE);
		skyboxCubemap.bind();
		Cube.draw();
		glDepthMask(GL_TRUE);

		normalShader.use();
		

		Bolide.update();
		glm::vec3 Bpos = Bolide.getPos();



		glm::vec3 cam_pos = Bpos - 2.5f*cam_eye;
		view = glm::lookAt(cam_pos, Bpos, cam_up);
		//view = glm::lookAt(glm::vec3(2.0f, 0.0f, 0.0f), Bpos, glm::vec3(0.0f, 1.0f, 0.0f));
		normalShader.uniformMat4f("projection", projection);
		normalShader.uniformMat4f("model", model);
		normalShader.uniformMat4f("view", view);

		
		model = Bolide.getModel();

		normalShader.uniformMat4f("model", model);

		

		normalShader.uniform1f("ambientIntensity", 0.1f);
		normalShader.uniform3f("ambientColor", glm::vec3(1.0f, 1.0f, 1.0f));

		normalShader.uniform1f("diffuseIntensity", 0.8f);
		normalShader.uniform3f("diffuseColor", glm::vec3(1.0f, 1.0f, 1.0f));

		normalShader.uniform1f("specularIntensity", 0.6f);
		normalShader.uniform3f("specularColor", glm::vec3(1.0f, 1.0f, 1.0f));

		normalShader.uniform3f("lightPos", Bpos+glm::vec3(0.0f, 0.0f, 1000.0f));
		normalShader.uniform3f("viewPos", cam_pos);



		
		glActiveTexture(GL_TEXTURE0);
		Craft_tex.bind();
		glActiveTexture(GL_TEXTURE1);
		Lightmap.bind();
		glActiveTexture(GL_TEXTURE2);
		Reflectionmap.bind();
		glActiveTexture(GL_TEXTURE3);
		skyboxCubemap.bind();
		glActiveTexture(GL_TEXTURE4);
		Black.bind();

		motor->setPlaybackSpeed(fabs(Bolide.getSpeed())+1.5f);
		motor->setVolume(0.2);
		
		Craft.draw();

		normalShader.uniform3f("diffuseColor", glm::vec3(1.0f, 0.8f, 0.6f));

		for(int i = 0; i < 1000; i++) {
			if(Asteroids[i].update(Bpos)){
				Texture tex;
				double freq = 1000.0;
				glm::vec3 aPos = Asteroids[i].getPos();
				if(Noise.noise0_1(aPos.x / freq, aPos.y / freq, aPos.z / freq) < 0.25) {
					tex = Rock1;
				} else if(Noise.noise0_1(aPos.x / freq, aPos.y / freq, aPos.z / freq) < 0.5) {
					tex = Rock2;
				} else if(Noise.noise0_1(aPos.x / freq, aPos.y / freq, aPos.z / freq) < 0.75) {
					tex = Rock3;
				} else {
					tex = Rock4;
				}
				Asteroids[i].setTex(tex);
			}
			normalShader.uniformMat4f("model", Asteroids[i].getModel());
			Asteroids[i].draw();
		}

		

		

		
		glm::mat4 tmp = model;

		glActiveTexture(GL_TEXTURE0);
		Trail.bind();
		glActiveTexture(GL_TEXTURE1);
		White.bind();
		glActiveTexture(GL_TEXTURE2);
		Black.bind();
		glActiveTexture(GL_TEXTURE4);
		White.bind();

		if(Bolide.getSpeed() >= 0.0f) {

			for(int i = 0; i < NB_TRAIL; i++){
				model = trailMat[i];
				model = glm::translate(model, glm::vec3(1.4f+0.5*Bolide.getSpeed(), 0.58f, -0.93f));
				model = glm::scale(model, glm::vec3(1.0f+9*Bolide.getSpeed(), 1.0f, 1.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

				normalShader.uniformMat4f("model", model);

				Tri.draw();

				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				normalShader.uniformMat4f("model", model);

				Tri.draw();

				model = glm::translate(model, glm::vec3(2.82f, 0.0f, 0.0f));

				normalShader.uniformMat4f("model", model);

				Tri.draw();

				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

				normalShader.uniformMat4f("model",  model);

				Tri.draw();
			}

			trailMat[trailCount] = tmp;
			trailCount++;
			if(trailCount >= NB_TRAIL)
				trailCount = 0;
		}

		model = tmp;
		glActiveTexture(GL_TEXTURE0);
		Flame.bind();
		glActiveTexture(GL_TEXTURE1);
		White.bind();
		glActiveTexture(GL_TEXTURE2);
		Black.bind();
		glActiveTexture(GL_TEXTURE4);
		White.bind();

		model = glm::translate(model, glm::vec3(1.4f, 0.58f, -0.93f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		normalShader.uniformMat4f("model", model);

		Tri.draw();

		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		normalShader.uniformMat4f("model", model);

		Tri.draw();

		model = glm::translate(model, glm::vec3(2.82f, 0.0f, 0.0f));

		normalShader.uniformMat4f("model", model);

		Tri.draw();

		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		normalShader.uniformMat4f("model",  model);

		Tri.draw();

		


		

		//blur
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE0);
		bool horizontal = true, first_iteration = true;
        unsigned int amount = 4;
        blurShader.use();
        blurShader.uniform1f("speed", fabs(Bolide.getSpeed()));
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.uniform1i("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffer[1] : pingpongColorbuffers[!horizontal]);  
            Quad.draw();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		postShader.use();
		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer[0]);
       	glActiveTexture(GL_TEXTURE1);
      	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
      	time += 1.8;
      	postShader.uniform1f("time", time);
      	if(fabs(Bolide.getSpeed()) > 3.7f) {
      		postShader.uniform1f("speed", fabs(Bolide.getSpeed()));
      		postShader.uniform1i("shake", 1);
      		error->setVolume(fabs(Bolide.getSpeed())-3.0f);
      	} else {
      		postShader.uniform1i("shake", 0);
      		error->setVolume(0.0f);
      	}

		Quad.draw();



 
		glfwSwapBuffers(window);
		glfwPollEvents(); 
	}
	normalShader.del();
	blurShader.del();
	skyboxShader.del();
	postShader.del();
	ambiance->drop();
	engine->drop();

	glfwTerminate();
	return EXIT_SUCCESS;
}

void resize_callback(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
    glViewport(0, 0, width, height);
} 

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static float lastx, lasty, yaw , pitch;
	if(first) {
		lastx = xpos;
		lasty = ypos;
		yaw  = 0;
		pitch = 0;
		first = false;
		printf("first\n");
	}

	float xoffset = xpos - lastx;
	float yoffset = ypos - lasty;
	lastx = xpos;
	lasty = ypos;

	yaw += SENSITIVITY*xoffset;
	pitch -= SENSITIVITY*yoffset;

	// printf("first %i, yaw %f Pitch %f\n",first, yaw, pitch);

	glm::vec3 front, roof ;
	front.x = cosf(glm::radians(pitch)) * cosf(glm::radians(yaw));
	front.y = sinf(glm::radians(pitch));
	front.z = cosf(glm::radians(pitch)) * sinf(glm::radians(yaw));

	roof.x = cosf(glm::radians(pitch+90)) * cosf(glm::radians(yaw));
	roof.y = sinf(glm::radians(pitch+90));
	roof.z = cosf(glm::radians(pitch+90)) * sinf(glm::radians(yaw));
	// glm::vec3 side = glm::cross(eye, up);
	// glm::vec3 vert = up;
	// front =glm::rotate(front, glm::radians(yaw), vert);
	// front =glm::rotate(front, glm::radians(pitch), side);
	// roof = glm::rotate(roof, glm::radians(yaw), vert);
	// roof = glm::rotate(roof, glm::radians(pitch), side);

	cam_eye = glm::normalize(front);
	cam_up = glm::normalize(roof);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(key == GLFW_KEY_C && action == GLFW_PRESS) {
    	printf("cam reset\n");
		first = true;
    }
}

void immediate_input(GLFWwindow* window)
{

	if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		Bolide.stop();

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		Bolide.pitchUp();
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		Bolide.pitchDown();
	}

	if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
		Bolide.accelerate();
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
		Bolide.decelerate();
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		Bolide.rollLeft();
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		Bolide.rollRight();
	}

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
		Bolide.yawLeft();
	}
	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
		Bolide.yawRight();
	}
		
}