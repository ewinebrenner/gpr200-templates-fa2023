#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cl/shader.h>
struct Vertex {
	float x, y, z;
	float u, v;
};

unsigned int createShader(GLenum shaderType, const char* sourceCode);
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;



Vertex vertices[4] = {
	  // x    y    z    u    v
	{ -1.0, -1.0, 0.0, 0.0, 0.0 }, //bottom left
	{  1.0, -1.0, 0.0, 1.0, 0.0 }, //bottom right
	{  1.0,  1.0, 0.0, 1.0, 1.0 }, //top right
	{ -1.0,  1.0, 0.0, 0.0, 1.0 }, //top left
};

unsigned int indices[6] = {
	0,1,2, // first triangle
	2,3,0  // second triangle
};

float triangleColor[3] = { 1.0f, 0.5f, 0.0f };
float triangleBrightness = 1.0f;
bool showImGUIDemoWindow = true;

float sunColorNight[3] = {0.8f, 0.2f, 0.2f};
float sunColorDay[3] = {1.0f, 1.0f, 0.0f};
float foregroundColor[3] = {0.1f, 0.1f, 0.1f};
float skyColorBottomA[3] = {0.9f, 0.0f, 0.5f};
float skyColorBottomB[3] = {0.2f, 0.0f, 0.6f};
float skyColorTopA[3] = {0.0f, 0.0f, 1.0f};
float skyColorTopB[3] = {0.0f, 1.0f, 1.0f};
float radius = 0.5f;
float sunSpeed = 1.0f;

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
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	celLib::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	shader.use();
	
	unsigned int vao = createVAO(vertices, 12, indices, 6);

	//glUseProgram(shader);
	glBindVertexArray(vao);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set uniforms
		float time = (float)glfwGetTime();
		shader.setVec3("_Color", triangleColor[0], triangleColor[1], triangleColor[2]);
		shader.setFloat("_Brightness", triangleBrightness);
		shader.setVec2("_iResolution", SCREEN_WIDTH, SCREEN_HEIGHT);
		shader.setFloat("_Time", time);
		//set sun color
		shader.setVec3("_sunColorNight", sunColorNight[0], sunColorNight[1], sunColorNight[2]);
		shader.setVec3("_sunColorDay", sunColorDay[0], sunColorDay[1], sunColorDay[2]);
		//set foreground color
		shader.setVec3("_foregroundColor", foregroundColor[0], foregroundColor[1], foregroundColor[2]);
		//set sky color
		shader.setVec3("_skyColorNightA", skyColorBottomA[0], skyColorBottomA[1], skyColorBottomA[2]);
		shader.setVec3("_skyColorNightB", skyColorBottomB[0], skyColorBottomB[1], skyColorBottomB[2]);
		shader.setVec3("_skyColorDayA", skyColorTopA[0], skyColorTopA[1], skyColorTopA[2]);
		shader.setVec3("_skyColorDayB", skyColorTopB[0], skyColorTopB[1], skyColorTopB[2]);
		//set radius
		shader.setFloat("_radius", radius);
		//set speed
		shader.setFloat("_sunSpeed", sunSpeed);


		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);

			ImGui::ColorEdit3("Sun Color Night", sunColorNight);
			ImGui::ColorEdit3("Sun Color Day", sunColorDay);

			ImGui::ColorEdit3("Foreground Color", foregroundColor);

			ImGui::ColorEdit3("Sky Color A (Day)", skyColorTopA);
			ImGui::ColorEdit3("Sky Color B (Day)", skyColorTopB);
			ImGui::ColorEdit3("Sky Color A (Night)", skyColorBottomA);
			ImGui::ColorEdit3("Sky Color B (Night)", skyColorBottomB);

			ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
			ImGui::SliderFloat("Sun Speed", &sunSpeed, 0.0f, 2.0f);

			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//EBO
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, vertexData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex,x)));
	glEnableVertexAttribArray(0);

	//UV
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex), (const void*)(offsetof(Vertex,u)));
	glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

