#ifndef UTIL_HPP
#define UTIL_HPP

#define VERT 0
#define VERT_NORM 1
#define VERT_NORM_TEX 2
#define VERT_TEX 3


class Shader {
public:
	Shader(const char* vs_source, const char* fs_source, const char* gm_source);
	Shader(const char* vs_source, const char* fs_source);
	Shader();
	void use(void);
	void uniform1i(const char* name, int i);
	void uniform1f(const char* name, float f);
	void uniform3f(const char* name, glm::vec3 vec);
	void uniform2f(const char* name, glm::vec2 vec);
	void uniformMat4f(const char* name, glm::mat4 mat);
	void uniform4f(const char* name, glm::vec4 vec);
	void del(void);


private:
	unsigned int fragmentShader, vertexShader, shaderProgram, geometryShader;

};


class Texture {
public:
	Texture();
	Texture(const char* filename);
	void bind(void);
	void changeWrapMode(int S_mode, int T_mode);
private:
	unsigned int textureID;
};

class Cubemap {
public:
	Cubemap(const char* front, const char* back, const char* left, const char* right, const char* top, const char* bottom);
	void bind(void);
private:
	unsigned int textureID;
};

class Solid {
public:
	Solid();
	Solid(const float* vertex_array, unsigned long size, int mode);
	void bind(void);
	void draw(void);
	void draw_pts(void);

private:
	unsigned int VAO, VBO;
	int nVertex;

};


#endif


