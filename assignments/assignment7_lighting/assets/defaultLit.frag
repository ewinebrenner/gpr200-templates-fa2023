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
uniform float _DiffuseStrength = 1.0;
uniform vec3 _ViewPos;
uniform float _SpecularStrength = 0.5;
uniform float _Shininess = 32;

void main(){

	//TODO: Lighting calculations using corrected normal
	vec3 normal = normalize(fs_in.WorldNormal);
	
	//ambient 
	vec3 ambient = _AmbientStrength * _Light.color;

	//diffuse
	vec3 lightDirection = normalize(_Light.position - fs_in.WorldPosition); //w
	float lightDiff = max(dot(normal,lightDirection),0.0);
	vec3 diffuse = _DiffuseStrength * (lightDiff * _Light.color);

	//specular
	vec3 viewDirection = normalize(_ViewPos - fs_in.WorldPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), _Shininess);
	vec3 specular = _SpecularStrength * spec * _Light.color;
	
	
	vec4 result = vec4(ambient+diffuse+specular,1.0); //vec4(ambient + diffuse + specular) * model_color
	FragColor = result * texture(_Texture,fs_in.UV); //is a vec4
}