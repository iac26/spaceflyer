

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "util.hpp"
#include "stb_image.h"

Shader::Shader(const char* vs_source, const char* fs_source) {

	FILE* sourceFile = fopen(vs_source, "r");
	fseek(sourceFile, 0, SEEK_END);
	int size = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_SET);
	char* buffer = (char*) malloc(size*sizeof(char) + 1);
	fread(buffer, 1, size, sourceFile);
	fclose(sourceFile);
	buffer[size] = '\0';

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &buffer, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(buffer);

	sourceFile = fopen(fs_source, "r");
	fseek(sourceFile, 0, SEEK_END);
	size = ftell(sourceFile);
	rewind(sourceFile);
	buffer = (char*) malloc(size*sizeof(char) + 1);
	fread(buffer, sizeof(char), size, sourceFile);
	fclose(sourceFile);
	buffer[size] = '\0';

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &buffer, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(buffer);

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	printf("shader compiled!!\n");

}

Shader::Shader(const char* vs_source, const char* fs_source, const char* gm_source) {

	FILE* sourceFile = fopen(vs_source, "r");
	fseek(sourceFile, 0, SEEK_END);
	int size = ftell(sourceFile);
	fseek(sourceFile, 0, SEEK_SET);
	char* buffer = (char*) malloc(size*sizeof(char) + 1);
	fread(buffer, 1, size, sourceFile);
	fclose(sourceFile);
	buffer[size] = '\0';

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &buffer, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(buffer);

	sourceFile = fopen(fs_source, "r");
	fseek(sourceFile, 0, SEEK_END);
	size = ftell(sourceFile);
	rewind(sourceFile);
	buffer = (char*) malloc(size*sizeof(char) + 1);
	fread(buffer, sizeof(char), size, sourceFile);
	fclose(sourceFile);
	buffer[size] = '\0';

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &buffer, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(buffer);

	sourceFile = fopen(gm_source, "r");
	fseek(sourceFile, 0, SEEK_END);
	size = ftell(sourceFile);
	rewind(sourceFile);
	buffer = (char*) malloc(size*sizeof(char) + 1);
	fread(buffer, sizeof(char), size, sourceFile);
	fclose(sourceFile);
	buffer[size] = '\0';

	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &buffer, NULL);
	glCompileShader(geometryShader);
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	free(buffer);

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, geometryShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	printf("gshader compiled!!\n");

}
Shader::Shader(void)  {

}

void Shader::use(void) {
	glUseProgram(shaderProgram);
}

void Shader::uniform1i(const char* name, int i) {
	int loc = glGetUniformLocation(shaderProgram, name);
	glUniform1i(loc, i);
}
void Shader::uniform1f(const char* name, float f) {
	int loc = glGetUniformLocation(shaderProgram, name);
	glUniform1f(loc, f);
}

void Shader::uniform3f(const char* name, glm::vec3 vec) {
	int loc = glGetUniformLocation(shaderProgram, name);
	glUniform3fv(loc, 1, glm::value_ptr(vec));
}

void Shader::uniform2f(const char* name, glm::vec2 vec) {
	int loc = glGetUniformLocation(shaderProgram, name);
	glUniform2fv(loc, 1, glm::value_ptr(vec));
}

void Shader::uniformMat4f(const char* name, glm::mat4 mat) {
	int loc = glGetUniformLocation(shaderProgram, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::uniform4f(const char* name, glm::vec4 vec) {
	int loc = glGetUniformLocation(shaderProgram, name);
	glUniform4fv(loc, 1, glm::value_ptr(vec));
}

void Shader::del(void) {
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);  
}

Texture::Texture(const char * name) {
	int w,h,nChan;
	unsigned char* data = stbi_load(name, &w, &h, &nChan, 0);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	printf("texture loaded!!\n");
}

Texture::Texture(void) {

}

void Texture::bind(void) {
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::changeWrapMode(int S_mode, int T_mode) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, S_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, T_mode);
}

Cubemap::Cubemap(const char* front, const char* back, const char* left, const char* right, const char* top, const char* bottom) {
	int w,h,nChan;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	unsigned char* data;

	data = stbi_load(back, &w, &h, &nChan, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    data = stbi_load(front, &w, &h, &nChan, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    data = stbi_load(bottom, &w, &h, &nChan, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    data = stbi_load(top, &w, &h, &nChan, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    data = stbi_load(left, &w, &h, &nChan, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    data = stbi_load(right, &w, &h, &nChan, 0);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 


	stbi_image_free(data);

	printf("cubemap loaded !!\n");

}

void Cubemap::bind(void) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

Solid::Solid(const float* vertex_array, unsigned long size, int mode) {
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size, vertex_array, GL_STATIC_DRAW);

	if(mode == VERT_NORM_TEX) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		nVertex = size/(sizeof(float)*8);
	}
	if(mode == VERT_NORM) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		nVertex = size/(sizeof(float)*6);

	}
	if(mode == VERT) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		nVertex = size/(sizeof(float)*3);
	}
	if(mode == VERT_TEX) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		nVertex = size/(sizeof(float)*5);
	}

	printf("object loaded (%d) !!\n", nVertex);

}

Solid::Solid(void) {

}

void Solid::bind(void) {
	glBindVertexArray(VAO);
}

void Solid::draw(void) {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, nVertex);
}
void Solid::draw_pts(void) {
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, nVertex);
}