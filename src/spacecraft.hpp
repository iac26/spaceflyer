#ifndef SPACECRAFT_HPP
#define SPACECRAFT_HPP

#include "util.hpp"
#include "PerlinNoise.hpp"


class Spacecraft {
public:
	Spacecraft(void);
	void pitchUp(void);
	void pitchDown(void);
	void rollLeft(void);
	void rollRight(void);
	void yawLeft(void);
	void yawRight(void);
	void accelerate(void);
	void decelerate(void);
	void stop(void);
	glm::vec3 getPos(void);
	void update(void);
	glm::mat4 getModel(void);
	glm::mat4 getLeft(void);
	glm::mat4 getRight(void);
	float getSpeed(void);

private:
	float speed;
	glm::vec3 up, left, front, pos;
	glm::mat4 rotation, Lrot, Rrot;

};

class Asteroid {
public:
	Asteroid(void);
	Asteroid(glm::vec3 p, Solid type, Texture texture, Texture reflection, Texture light);
	void setRot(glm::vec3 r_axis, float spd);
	void setSpeed(glm::vec3 spd);
	void setScale(glm::vec3 scl);
	void setTex(Texture texture);
	glm::vec3 getPos(void);
	int update(glm::vec3 p_pos);
	void draw(void);
	glm::mat4 getModel(void);

private:
	glm::vec3 pos, rot_axis, speed, scale;
	float rot_speed; 
	Solid body;
	glm::mat4 rot;
	Texture tex, ref, li;

};


#endif