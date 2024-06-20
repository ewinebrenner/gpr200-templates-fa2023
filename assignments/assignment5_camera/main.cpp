#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <cl/Camera.h>


void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Projection will account for aspect ratio!
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

celLib::Camera cam;
celLib::CameraControls camControls;

void moveCamera(GLFWwindow* window, celLib::Camera* camera, celLib::CameraControls* controls, float dt);

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

	//Camera settings
	ew::Vec3 defaultPos = ew::Vec3(0, 0, 5);
	cam.position = defaultPos;

	ew::Vec3 defaultTarget = ew::Vec3(0,0,0);
	cam.target = defaultTarget;

	float defaultFOV = 60;
	cam.fov = defaultFOV;

	cam.aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	float defaultNearPlane = 0.1;
	cam.nearPlane = defaultNearPlane;

	float defaultFarPlane = 100;
	cam.farPlane = defaultFarPlane;

	bool defaultOrthographic = false;
	cam.orthographic = defaultOrthographic;

	float defaultOrthoSize = 6;
	cam.orthoSize = defaultOrthoSize;

	bool orbit = false;
	cam.orbiting = orbit;

	float defaultOrbitSpeed = 0.5f;

	//Cube positions
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES / 2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES / 2) - 0.5;
	}

	float prevTime = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		moveCamera(window, &cam , &camControls, deltaTime);
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use();

		shader.setMat4("_Projection", cam.ProjectionMatrix());
		shader.setMat4("_View", cam.ViewMatrix());
		
		//TODO: Set model matrix uniform
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::Text("Cubes");
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::Text("Camera");
			ImGui::Checkbox("Orbit", &cam.orbiting); 
			ImGui::DragFloat("Orbit Speed", &cam.orbitSpeed,defaultOrbitSpeed,0.0f, 10.0f);
			ImGui::DragFloat3("Position", &cam.position.x, 0.05f);
			ImGui::DragFloat3("Target", &cam.target.x, 0.05f);
			ImGui::Checkbox("Orthographic", &cam.orthographic);
			ImGui::DragFloat("FOV", &cam.fov, 0.05f);
			ImGui::DragFloat("Near Plane", &cam.nearPlane, 0.05f);
			ImGui::DragFloat("Far Plane", &cam.farPlane, 0.05f);
			//Reset button
			//ImGui::Button("Reset");
			if (ImGui::Button("Reset"))
			{
				cam.position = defaultPos;
				cam.target = defaultTarget;
				cam.fov = defaultFOV;
				cam.nearPlane = defaultNearPlane;
				cam.farPlane = defaultFarPlane;
				cam.orthographic = defaultOrthographic;
				cam.orthoSize = defaultOrthoSize;
			}
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void moveCamera(GLFWwindow* window, celLib::Camera* camera, celLib::CameraControls* controls, float dt) 
{
	ew::Vec3 worldUp = ew::Vec3(0, 1, 0);

	float yaw = -90.0f;
	float pitch = 0.0f;

	controls->prevMouseX = SCREEN_WIDTH / 2;
	controls->prevMouseY = SCREEN_HEIGHT / 2;

	//If right mouse is not held, release cursor and return early
	if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) 
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		controls->firstMouse = true;
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	if (controls->firstMouse) 
	{
		controls->firstMouse = false;
		controls->prevMouseX = mouseX;
		controls->prevMouseY = mouseY;
	}

	// Get mouse position delta for this frame
	float mouseSensitivity = 0.5f;
	float mouseDeltaX = (mouseX - controls->prevMouseX)*mouseSensitivity;
	float mouseDeltaY = (controls->prevMouseY - mouseY)*mouseSensitivity;
	
	// Add to yaw and pitch
	yaw += mouseDeltaX;
	pitch += mouseDeltaY;
	
	// Clamp pitch between -89 and 89 degrees
	if (pitch > 89.0f) 
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f) 
	{
		pitch = -89.0f;
	}

	//Remember previous mouse position
	controls->prevMouseX = mouseX;
	controls->prevMouseY = mouseY;

	// construct forward vector using yaw and pitch. Don't forget to convert to radians
	//ew::Vec3 forward = ???
	float yawRad = (yaw * 3.14) / 180;
	float pitchRad = (pitch * 3.14) / 180;
	ew::Vec3 forward = ew::Vec3(cos(yawRad) * cos(pitchRad), sin(pitchRad), sin(yawRad)* cos(pitchRad));

	ew::Vec3 normalForward = ew::Normalize(forward);
	ew::Vec3 right = ew::Normalize(ew::Cross(normalForward, ew::Normalize(worldUp)));
	ew::Vec3 up = ew::Normalize(ew::Cross(right, normalForward));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
	{
		camera->position += normalForward * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
	{
		camera->position += -normalForward * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
	{
		camera->position += right * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	{
		camera->position += -right * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS ) 
	{
		camera->position += up * controls->moveSpeed * dt;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
	{
		camera->position += -up * controls->moveSpeed * dt;
	}

	//cam.position += forward * controls->moveSpeed * deltaTime;
	camera->target = camera->position + forward;


}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

