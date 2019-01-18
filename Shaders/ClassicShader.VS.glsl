#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

//uniform float time;

//const float PI = 3.1415926;

//const float wavelength = 50.0f;
//const float speed = 0.2f;
//const float amplitude = 5.
out vec3 world_position;
out vec3 world_normal;

uniform vec4 clip_plane;

layout(location = 0) out vec3 f_color;
out vec2 texcoord;
out vec4 clipSpace;

void main()
{
	f_color = v_color;
	world_position = (Model * vec4(v_position, 1)).xyz;
	world_normal = (Model * vec4(v_normal, 1)).xyz;
	texcoord = v_texture_coord;

	gl_ClipDistance[0] = dot(Model * vec4(v_position, 1), clip_plane);
	/*vec3 pos = v_position;
	float k = 2 * PI / wavelength;
	float f = k * (pos.x - speed * time * 100);
	pos.x += amplitude * cos(f);
	pos.y = amplitude * sin(f);*/

	//gl_Position = Projection * View * Model * vec4(pos, 1);
	gl_Position = Projection * View * Model * vec4(v_position, 1);
}
