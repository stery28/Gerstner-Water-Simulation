#version 430

layout(location = 0) in vec3 f_color;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

in vec3 world_position;
in vec3 world_normal;

layout(location = 0) out vec4 out_color;

void main()
{
	//out_color = vec4(f_color, 0);
	out_color = vec4(0, 0.2f, 0.7f, 0);
}