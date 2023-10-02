#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _characterTexture;
uniform float _characterTime;

void main(){
	//FragColor = vec4(UV.x,UV.y,0.0,1.0);
	FragColor = texture(_characterTexture, UV * 2.0);
}