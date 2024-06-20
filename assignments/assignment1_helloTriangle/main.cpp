#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

unsigned int createVAO(float* vertexData, int numVertices);
unsigned int createShader(GLenum shaderType, const char* sourceCode);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);


float vertices[21] = {
	 //x   //y  //z	 //r  //g  //b  //a 
	-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0,
	 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0,
	 0.0,  0.5, 0.0, 0.0, 0.0, 1.0, 1.0
};

const char* vertexShaderSource = R"(
#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vColor;
out vec4 Color;
uniform float _Time;
void main(){
	Color = vColor;
	vec3 offset = vec3(0,sin(vPos.x + _Time),0)*0.5;
	gl_Position = vec4(vPos + offset, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 450
out vec4 FragColor;
in vec4 Color;
uniform float _Time;
void main(){
	FragColor = Color * abs(sin(_Time));
}
)";


int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Add VBOs and stuff here
	unsigned int vao = createVAO(vertices, 3);
	unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);

		float time = (float)glfwGetTime();
		int timeLocation = glGetUniformLocation(shaderProgram, "_Time");
		glUniform1f(timeLocation, time);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(float* vertexData, int numVertices) 
{
	//VBO
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertexData, GL_STATIC_DRAW);

	//VAO
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Define Position
	glVertexAttribPointer(0, numVertices, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)0);
	glEnableVertexAttribArray(0);
	//Color Attribute
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);

	return vao;
}

unsigned int createShader(GLenum shaderType, const char* sourceCode) 
{
	int success;
	if (shaderType == GL_VERTEX_SHADER) 
	{
		//VertexShader compile
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			printf("Failed to compile vertex shader: %s", infoLog);
		}
		return vertexShader;
	}
	if (shaderType == GL_FRAGMENT_SHADER) 
	{
		//FragmentShader compile
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			printf("Failed to compile fragment shader: %s", infoLog);
		}
		return fragmentShader;
	}
}

unsigned int createShaderProgram(const char* _vertexShaderSource, const char* _fragmentShaderSource) 
{
	int success;
	unsigned int vertexShader = createShader(GL_VERTEX_SHADER, _vertexShaderSource);
	unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, _fragmentShaderSource);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}




