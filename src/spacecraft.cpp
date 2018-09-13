#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "glad.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "spacecraft.hpp"
#include "util.hpp"
#include "PerlinNoise.hpp"

#define SENSITIVITY 2.0f//(speed > 1.0f? 2.0f*speed : 2.0f)
#define SPEED 0.01f
#define MAX_SPEED 4.0f

Spacecraft::Spacecraft(void) {
	pos = glm::vec3(-20.0f, 0.0f, 20.0f);
	front = glm::vec3(-1.0f, 0.0f, 0.0f);
	left = glm::vec3(0.0f, 0.0f, 1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	rotation = glm::mat4(1.0f);
	speed = 0.0f;
	Lrot = glm::mat4(1.0f);
	Rrot = glm::mat4(1.0f);

}

#define ANGLE 60.0f

void Spacecraft::pitchUp(void) {
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(SENSITIVITY), left) * rotation;
}

void Spacecraft::pitchDown(void) {
	rotation = glm::rotate(glm::mat4(1.0f), -glm::radians(SENSITIVITY), left) * rotation;
}

void Spacecraft::rollLeft(void) {
	rotation = glm::rotate(glm::mat4(1.0f), -glm::radians(2.0f*SENSITIVITY), front) * rotation;
}

void Spacecraft::rollRight(void) {
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(2.0f*SENSITIVITY), front) * rotation;
}

void Spacecraft::yawLeft(void) {
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(SENSITIVITY), up) * rotation;
}

void Spacecraft::yawRight(void) {
	rotation = glm::rotate(glm::mat4(1.0f), -glm::radians(SENSITIVITY), up) * rotation;
}

void Spacecraft::accelerate(void) {
	speed -= SPEED;
}

void Spacecraft::decelerate(void) {
	speed += SPEED;
}

void Spacecraft::stop(void) {
	speed = 0.0f;
}

glm::vec3 Spacecraft::getPos(void) {
	return pos;
}

void Spacecraft::update(void) {
	if(speed > MAX_SPEED)
		speed = MAX_SPEED;
	if(speed < -MAX_SPEED)
		speed = MAX_SPEED;
	front = glm::normalize(glm::vec3(rotation*glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f))); 
	left = glm::normalize(glm::vec3(rotation*glm::vec4(0.0f, 0.0f, 1.0f, 0.0f))); 
	up = glm::normalize(glm::vec3(rotation*glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))); 
	pos += speed*front;
}

glm::mat4 Spacecraft::getModel(void) {
	glm::mat4 model = glm::translate(glm::mat4(1.0), pos);
	model = glm::scale(model, glm::vec3(0.2f));
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, -0.5f));
	return model * rotation * trans;
}

float Spacecraft::getSpeed(void) {
	return speed;
}

glm::mat4 Spacecraft::getLeft(void) {
	glm::mat4 tmp = Lrot;
	Lrot = glm::mat4(1.0f);
	return tmp;
}

glm::mat4 Spacecraft::getRight(void) {
	glm::mat4 tmp = Rrot;
	Rrot = glm::mat4(1.0f);
	return tmp;
}

Asteroid::Asteroid(glm::vec3 p, Solid type, Texture texture, Texture reflection, Texture light) {
	pos = p;
	body = type;
	tex = texture;
	ref = reflection;
	li = light;
	speed = glm::vec3(0.0f);
	rot_speed = 0.0f;
	rot = glm::mat4(1.0f);
	rot_axis = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);

}

Asteroid::Asteroid (void) {

}

void Asteroid::setRot(glm::vec3 r_axis, float spd) {
	rot_axis = r_axis;
	rot_speed = spd;
}

void Asteroid::setSpeed(glm::vec3 spd) {
	speed = spd;
}

void Asteroid::setScale(glm::vec3 scl) {
	scale = scl;
}

void Asteroid::setTex(Texture texture) {
	tex = texture;
}

glm::vec3 Asteroid::getPos(void) {
	return pos;
}

int Asteroid::update(glm::vec3 p_pos) {
	pos += speed;
	rot = glm::rotate(rot, glm::radians(rot_speed), rot_axis);

	if(glm::distance(p_pos, pos) > 1000) {
		pos = p_pos - pos + p_pos;
		return 1;
	}
	return 0;
}

void Asteroid::draw(void) {

	
	glActiveTexture(GL_TEXTURE0);
	tex.bind();
	glActiveTexture(GL_TEXTURE1);
	li.bind();
	glActiveTexture(GL_TEXTURE2);
	ref.bind();

	body.draw();

}
glm::mat4 Asteroid::getModel(void) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), pos)*rot;
	model = glm::scale(model, scale);
	return model;
}