#version 450
out vec4 FragColor;
//uniform vec3 _Color;
//uniform float _Brightness;
in vec2 UV; 
uniform float _Time = 1.0;
uniform vec2 _iResolution;

uniform vec3 _sunColorNight;
uniform vec3 _sunColorDay;

uniform vec3 _foregroundColor;

uniform vec3 _skyColorNightA;
uniform vec3 _skyColorNightB;
uniform vec3 _skyColorDayA;
uniform vec3 _skyColorDayB;

uniform float _radius;

uniform float _sunSpeed;

vec2 fragCoord = gl_FragCoord.xy;
void main(){
	
	//Setup
	vec2 uv = fragCoord/_iResolution;
	uv = uv * 2.0 - 1.0;
	uv.x *= _iResolution.x / _iResolution.y;

	//Sun
	vec2 center = vec2(0.0, sin(_Time * _sunSpeed) - 0.5);
	float d = distance(center, uv);
	float radius = _radius;
	float t = smoothstep(radius - 0.08, radius + 0.08, d);
	
	//Sun Color
	vec3 sunColor = vec3(1.0, 1.0, 0.0);
	vec3 sColor_Night = _sunColorNight;
	vec3 sColor_Day = _sunColorDay;
	sunColor = mix(sColor_Night, sColor_Day, sin(_Time));

	//Sky Color
	vec3 skyColor = vec3(0.0, 0.0, 1.0);

	vec3 ColorB_Night = _skyColorNightB;
	vec3 ColorA_Night = _skyColorNightA;
	vec3 nightColor = mix(ColorA_Night, ColorB_Night, uv.y);

	vec3 ColorA_Day = _skyColorDayA;
	vec3 ColorB_Day = _skyColorDayB;
	vec3 dayColor = mix(ColorA_Day, ColorB_Day, uv.y);

	skyColor = mix(nightColor, dayColor, sin(_Time) + 0.5);

	//Hills
	float hills = 1.0 - step(sin(uv.x * 4.0 + _Time)*0.2 - 0.3, uv.y);
	vec3 hillColor = _foregroundColor;

	//Draw Scene
	vec3 sky = mix(sunColor, skyColor, t);
	vec3 col = mix(sky, hillColor, hills);

	FragColor = vec4(col, 1.0);
}