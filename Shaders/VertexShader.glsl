#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
//uniform mat4 View;
//uniform mat4 Projection;

//uniform float time;

//const float PI = 3.1415926;

//const float wavelength = 50.0f;
//const float speed = 0.2f;
//const float amplitude = 5.0f;
out vec3 w_normal;

layout(location = 0) out vec3 g_color;

void main()
{
	g_color = v_color;
	w_normal = v_normal;
	/*vec3 pos = v_position;
	float k = 2 * PI / wavelength;
	float f = k * (pos.x - speed * time * 100);
	pos.x += amplitude * cos(f);
	pos.y = amplitude * sin(f);*/

	//gl_Position = Projection * View * Model * vec4(pos, 1);
	gl_Position = Model * vec4(v_position, 1);
}
