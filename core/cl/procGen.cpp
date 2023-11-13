#include "procGen.h"


namespace celLib 
{
	ew::MeshData createSphere(float radius, int numSegments)
	{
		//TODO: fill out function
		ew::MeshData returnValue;
		ew::Vertex v;
		float thetaStep = (ew::TAU) / numSegments;
		float phiStep = ew::PI / numSegments;
		for (int row = 0; row <= numSegments; row++) 
		{
			float phi = row * phiStep;
			for (int col = 0; col <= numSegments; col++) 
			{
				float theta = col * thetaStep;
				v.pos.x = radius * std::cos(theta) * std::sin(phi);
				v.pos.y = radius * std::cos(phi);
				v.pos.z = radius * std::sin(theta) * std::sin(phi);
				returnValue.vertices.push_back(v);
			}
		}

		//Sphere caps
		int poleStart = 0;
		int sideStart = numSegments + 1;

		for (int i = 0; i < numSegments; i++) 
		{
			returnValue.indices.push_back(sideStart+i);
			returnValue.indices.push_back(poleStart+i);
			returnValue.indices.push_back(sideStart+i+1);
		}

		//Sphere row indices
		int columns = numSegments + 1;
		for (int row = 1; row < numSegments-1; row++) 
		{
			for (int col = 0; col < numSegments; col++) 
			{
				int start = row * columns + col;
				//Triangle 1
				returnValue.indices.push_back(start);
				returnValue.indices.push_back(start+1);
				returnValue.indices.push_back(start+columns);
				//Triangle 2
				returnValue.indices.push_back(start+1);
				returnValue.indices.push_back(start+columns+1);
				returnValue.indices.push_back(start+columns);
			}
		}

		return returnValue;
	}

	ew::MeshData createCylinder(float height, float radius, int numSegments)
	{
		//TODO: fill out function
		ew::Vertex v;
		ew::MeshData returnValue;
		float topY = height / 2;
		float bottomY = -topY;
		float thetaStep = (ew::TAU) / numSegments;
		float theta = 0;

		//Top center
		v.pos.x = 0;
		v.pos.y = topY;
		v.pos.z = 0;
		v.normal = ew::Vec3(0,1,0);
		returnValue.vertices.push_back(v);
		//Top ring
		for (int i = 0; i <= numSegments; i++) 
		{
			theta = i * thetaStep;
			v.pos.x = std::cos(theta) * radius;
			v.pos.y = topY;
			v.pos.z = std::sin(theta) * radius;
			v.normal = ew::Vec3(0,topY,0);
			returnValue.vertices.push_back(v);
		}
		//Top ring 2
		for (int i2 = 0; i2 <= numSegments; i2++) 
		{
			theta = i2 * thetaStep;
			v.pos.x = std::cos(theta) * radius;
			v.pos.y = topY;
			v.pos.z = std::sin(theta) * radius;
			v.normal = ew::Vec3(std::cos(theta), 0, std::sin(theta));
			returnValue.vertices.push_back(v);
		}
		//Bottom ring
		for (int j = 0; j <= numSegments; j++) 
		{
			theta = j * thetaStep;
			v.pos.x = std::cos(theta) * radius;
			v.pos.y = bottomY;
			v.pos.z = std::sin(theta) * radius;
			v.normal = ew::Vec3(std::cos(theta), 0, std::sin(theta));
			returnValue.vertices.push_back(v);
			
		}
		for (int j2 = 0; j2 <= numSegments; j2++) 
		{
			theta = j2 * thetaStep;
			v.pos.x = std::cos(theta) * radius;
			v.pos.y = bottomY;
			v.pos.z = std::sin(theta) * radius;
			v.normal = ew::Vec3(0, bottomY, 0);
			returnValue.vertices.push_back(v);
		}
		//Bottom center
		v.pos.x = 0;
		v.pos.y = bottomY;
		v.pos.z = 0;
		v.normal = ew::Vec3(0,-1,0);
		returnValue.vertices.push_back(v);

		//indices
		//cap
		int start = 1;
		int center = 0;
		for (int i = 0; i < numSegments*2 + 1; i++)
		{
			returnValue.indices.push_back(start + i);
			returnValue.indices.push_back(center);
			returnValue.indices.push_back(start + i + 1);
		}
		//side indices
		int sideStart = numSegments+2;
		int columns = numSegments+1;
		for (int i = 0; i < columns; i++)
		{
			start = sideStart + i;
			//Triangle 1
			returnValue.indices.push_back(start);
			returnValue.indices.push_back(start + 1);
			returnValue.indices.push_back(start + columns);
			//Triangle 2
			returnValue.indices.push_back(start + columns);
			returnValue.indices.push_back(start + 1);
			returnValue.indices.push_back(start + columns + 1);
		}
		//bottom cap
		//int botCenter = numSegments + 1;
		center = returnValue.vertices.size() -1;
		start = returnValue.vertices.size() - 2;

		for (int i = 0; i < numSegments*2; i++)
		{
			returnValue.indices.push_back(start - i);
			returnValue.indices.push_back(center);
			returnValue.indices.push_back(start - i - 1);
		}

		return returnValue;
	}

	ew::MeshData createPlane(float width, float height, int subdivisions)
	{
		//TODO: fill out function
		ew::Vertex v;
		ew::MeshData returnValue;
		for (int row = 0; row <= subdivisions; row++) 
		{
			for (int col = 0; col <= subdivisions; col++) 
			{
				v.pos.x = width * (col / subdivisions);
				v.pos.z = -height * (row / subdivisions);
				v.normal = ew::Normalize(ew::Vec3(0, 1, 0));
				ew::Vec2 uv;
				uv.x = col / subdivisions;
				uv.y = row / subdivisions;
				v.uv = uv;
				returnValue.vertices.push_back(v);
			}
		}
		//indices
		int columns = subdivisions + 1;
		int start = 0;
		for (int row = 0; row < subdivisions; row++) 
		{
			for (int col = 0; col < subdivisions; col++) 
			{
				start = row * columns + col;
				//Bottom Right Triangle
				returnValue.indices.push_back(start);
				returnValue.indices.push_back(start+1);
				returnValue.indices.push_back(start + columns + 1);
				//Bottom Left Triangle
				returnValue.indices.push_back(start+columns);
				returnValue.indices.push_back(start);
				returnValue.indices.push_back(start+columns+1);
			}
		}
		return returnValue;
	}
}