#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light 
{
	ew::Vec3 position; // world space
	ew::Vec3 color;	   // RGB
};

struct Material 
{
	float ambientK; // Ambient coefficient (0-1)
	float diffuseK; // Diffuse coefficient (0-1)
	float specular; // Specular coefficient (0-1)
	float shininess; // Shininess
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
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

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader unlitShader("assets/unlit.vert","assets/unlit.frag");

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	ew::Mesh sphereLightMesh(ew::createSphere(0.2,64));
	ew::Mesh red_sphereLightMesh(ew::createSphere(0.2,64));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	ew::Vec3 defaultLightPosition = ew::Vec3(0.0, 2.0f, 0.0f);
	ew::Transform sphereLightTransform;
	sphereLightTransform.position = defaultLightPosition;

	ew::Vec3 redLightPosition = ew::Vec3(1.0f, 2.0f, 0.0f);
	ew::Transform redLightTransform;
	redLightTransform.position = redLightPosition;

	ew::Vec3 defaultLightColor = ew::Vec3(1.0,1.0,1.0);
	Light defaultLight;
	defaultLight.color = defaultLightColor;
	defaultLight.position = defaultLightPosition;

	ew::Vec3 redLightColor = ew::Vec3(1.0,0.0,0.0);
	Light redLight; 
	redLight.color = redLightColor;
	redLight.position = redLightPosition;

	//Need to create an array of colors and positions to swap out in the render loop
	

	Material material;
	material.ambientK = 0.1;
	material.shininess = 32;
	material.specular = 0.5;
	material.diffuseK = 1.0;

	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
		cylinderMesh.draw();

		shader.setVec3("_Light.position", defaultLightPosition);
		shader.setVec3("_Light.color", defaultLightColor);
		shader.setFloat("_AmbientStrength", material.ambientK);
		shader.setFloat("_SpecularStrength", material.specular);
		shader.setFloat("_Shininess", material.shininess);
		shader.setFloat("_DiffuseStrength", material.diffuseK);

		shader.setVec3("_ViewPos", camera.position);

		//TODO: Render point lights
		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
		unlitShader.setMat4("_Model", sphereLightTransform.getModelMatrix());
		unlitShader.setVec3("_Color",defaultLightColor);
		sphereLightMesh.draw();
		unlitShader.setMat4("_Model", redLightTransform.getModelMatrix());
		unlitShader.setVec3("_Color", redLightColor);
		red_sphereLightMesh.draw();

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
			ImGui::DragFloat("Ambient", &material.ambientK, 0.01f ,0.0f, 1.0f);
			ImGui::DragFloat("Diffuse", &material.diffuseK, 0.01f, 0.0, 1.0f);
			ImGui::DragFloat("Specular", &material.specular, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Shininess", &material.shininess, 1.0f, 0.0f, 256.0f);
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


