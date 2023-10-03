#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _brickTexture;
uniform sampler2D _noiseTexture;
uniform float _time;

void main(){
	//FragColor = vec4(UV.x,UV.y,0.0,1.0);
	vec4 movingNoise = texture(_noiseTexture, vec2(sin(UV.x  + _time), UV.y + _time));
	FragColor = mix(texture(_brickTexture, UV), movingNoise, 0.7);
}