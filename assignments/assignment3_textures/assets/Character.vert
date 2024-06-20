#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
uniform float _time;
uniform float _scale; 
uniform float _speed;
void main(){
	UV = vUV;
	vec3 offset = vec3(sin(vPos.x * 6.28 + (_time * _speed)), cos(vPos.y * 6.28 + (_time * _speed)),0) * 1.0;
	gl_Position = vec4(vPos + offset,_scale);
}