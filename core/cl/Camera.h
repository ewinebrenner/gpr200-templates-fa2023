#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include "../cl/Transformations.h"
#include <GLFW/glfw3.h>

namespace celLib 
{
	struct CameraControls 
	{
		double prevMouseX, prevMouseY; //Mouse position from previous frame
		float yaw = 0, pitch = 0; //Degrees
		float mouseSensitivity = 0.1f; //How fast to turn with mouse
		bool firstMouse = true; //Flag to store initial mouse position
		float moveSpeed = 5.0f; //How fast to move with arrow keys (M/S)
	};

	struct Camera 
	{
		ew::Vec3 position; // Camera body position
		ew::Vec3 target; // Position to look at
		float fov; // Vertical field of view in degrees
		float aspectRatio; // Screen width / Screen height
		float nearPlane; // Near plane distance (+Z)
		float farPlane; // Far plane distance (+Z)
		bool orthographic; // Perspective or Orthographic
		float orthoSize; // Height of orthographic frustum
		bool orbiting;
		float orbitSpeed = 0.5f;
		ew::Mat4 ViewMatrix()
		{
			// World -> View
			//will use LookAt()
			//LookAt(position,target,up);
			
			ew::Vec3 up = ew::Vec3(0, 1, 0);
			if (orbiting == true) 
			{
				float camX = sin(orbitSpeed * glfwGetTime());
				float camZ = cos(orbitSpeed * glfwGetTime());
				position = ew::Vec3(camX * 5.0, 0.0, camZ * 5.0);
				return celLib::LookAt(position, target, up);
			}
			else 
			{
				return celLib::LookAt(position, target, up);
			}
		};
		ew::Mat4 ProjectionMatrix() 
		{
			// View -> Clip
			//will use Orthographic or Perspective based on the orthographic bool
			if (orthographic) 
			{
				return Orthographic(orthoSize,aspectRatio,nearPlane,farPlane);
			}
			else 
			{
				float fovRadians = (fov * 3.14) / 180;
				return Perspective(fovRadians,aspectRatio,nearPlane,farPlane);
			}
		};
	};
}