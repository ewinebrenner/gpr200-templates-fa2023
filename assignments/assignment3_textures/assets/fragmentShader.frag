#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _brickTexture;
uniform sampler2D _noiseTexture;
uniform sampler2D _extraTexture;
uniform float _time;

void main(){
	vec4 bgTexture = mix(texture(_brickTexture,UV),texture(_extraTexture,UV * 2.0 - 0.5),0.5);
	vec4 movingNoise = texture(_noiseTexture, vec2(sin(UV.x  + _time), UV.y + _time));
	FragColor = mix(bgTexture, movingNoise, 0.7);
}