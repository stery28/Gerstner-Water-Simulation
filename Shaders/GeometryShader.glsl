#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 170) out;

layout(location = 0) in vec3 g_color[];

uniform mat4 View;
uniform mat4 Projection;

//uniform int instances;

uniform float time;

layout(location = 0) out vec3 f_color;

const float PI = 3.1415926;

const float wavelength = 50.0f;
const float speed = 0.2f;
const float amplitude = 5.0f;

void EmitPoint(vec3 pos, vec3 offset)
{
	float k = 2 * PI / wavelength;
	float f = k * (pos.x - speed * time * 100);
	pos.x += amplitude * cos(f);
	pos.y = amplitude * sin(f);
	gl_Position = Projection * View * vec4(pos + offset, 1.0);
	EmitVertex();
}

void main()
{
	vec3 p1 = gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[1].gl_Position.xyz;
	vec3 p3 = gl_in[2].gl_Position.xyz;
	//vec3 p4 = gl_in[3].gl_Position.xyz;

	p2.y += 2;

	//p4.z += 5;



	//for (int i = 0; i <= instances; i++)
	//{
		//TODO modify offset so that instances are displayed on 6 columns
		vec3 offset = vec3(0, 0, 0);



		//TODO modify the points so that the triangle shrinks relative to its center
		f_color = g_color[0];
		EmitPoint(p1, offset);

		f_color = g_color[1];
		EmitPoint(p2, offset);

		f_color = g_color[2];
		EmitPoint(p3, offset);

		/*f_color = g_color[3];
		EmitPoint(p4, offset);*/

		EndPrimitive();
	//}
}
