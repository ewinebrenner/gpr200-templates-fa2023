#version 450
out vec4 FragColor;

in Surface{
	vec2 UV; //Per-fragment interpolated UV
	vec3 WorldPosition; // Per-fragment interpolated world position
	vec3 WorldNormal; // Per-fragment interpolated world normal
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};

#define MAX_LIGHTS 4
uniform Light _Lights[MAX_LIGHTS];
uniform Light _Light;
uniform sampler2D _Texture;

uniform float _AmbientStrength = 0.1;

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);
	//TODO: Lighting calculations using corrected normal
	//ambient 
	vec3 ambient = _AmbientStrength * _Light.color;

	//diffuse

	//specular
	
	vec4 result = vec4(ambient,1.0); //vec4(ambient + diffuse + specular) * model_color
	FragColor = result * texture(_Texture,fs_in.UV); //is a vec4
}