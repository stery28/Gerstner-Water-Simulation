#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 170) out;

layout(location = 0) in vec3 g_color[];

uniform mat4 View;
uniform mat4 Projection;

//uniform int instances;

uniform float time;
uniform int waves_count;
uniform vec2 directions[10];
//uniform float steepness[];
uniform float wavelength[10];

layout(location = 0) out vec3 f_color;

const float PI = 3.1415926;

//const float wavelength = 50.0f;
//const float speed = 0.2f;
//const float amplitude = 5.0f;
const float steepness = 0.5f;
//const vec2 direction = vec2(1, 0.5f);

// Fragment shader stuff for reflect/refract
in vec3 w_normal[];
out vec3 world_position;
out vec3 world_normal;

vec3 gerstner(vec3 pos, inout vec3 normal)
{
	/*float k = 2 * PI / wavelength;
	float c = sqrt(9.8 / k);
	vec2 direction = normalize(vec2(1, 0));
	float f = k * (dot(direction, pos.xz), -c * time);
	float a = steepness / k;
	return vec3(direction.x * (a * cos(f)), a*sin(f), direction.y*(a*cos(f)));*/
	vec3 result_pos = pos;
	vec3 tangent = vec3(0);
	vec3 binormal = vec3(0);
	normal = vec3(0);
	for (int i = 0; i < waves_count; i++)
	{
		float k = 2 * PI / wavelength[i];
		float speed = sqrt(9.8 / k);
		float amplitude = steepness / k;
		int j = 0;
		vec2 dir = normalize(directions[i]);
		float f = k * (dot(dir, pos.xz) - speed * time);
		result_pos.x += dir.x * (amplitude * cos(f));
		result_pos.y = amplitude * sin(f);
		result_pos.z += dir.y * (amplitude * cos(f));

		tangent += vec3(
			-dir.x * dir.x * (steepness * sin(f)),
			dir.x * (steepness * cos(f)),
			-dir.x * dir.y * (steepness * sin(f)));
		binormal += vec3(
			-dir.x * dir.y * (steepness * sin(f)),
			dir.y * (steepness * cos(f)),
			-dir.y * dir.y * (steepness * sin(f)));

		float wa = wavelength[i] * amplitude;
		float tmp = dot(wavelength[i] * dir, result_pos.xz);
		float s = sin(tmp + speed * time);
		float c = cos(tmp + speed * time);
		float qi = steepness / (wa * waves_count);
		normal += vec3(
			(dir.x * wa * c),
			(dir.y * wa * c),
			(qi * wa * s)
		);
	}
	//normal = normalize(cross(binormal, tangent));
	normal = vec3(-normal.x, -normal.y, 1 - normal.z);
	return result_pos;
	/*float dir = dot(pos.xz, direction);
	float w = 2 * PI / wavelength;
	float C = cos(w*dir + time * 100);
	float S = sin(w*dir + time * 100);

	return vec3(pos.x + steepness * amplitude*C*direction.x, amplitude*S, pos.z + steepness * amplitude*C*direction.y);*/
}

void EmitPoint(vec3 pos, vec3 offset, vec3 normal)
{
	/*float k = 2 * PI / wavelength;
	float f = k * (pos.x - speed * time * 100);
	pos.x += amplitude * cos(f);
	pos.y = amplitude * sin(f);*/
	//vec3 new_pos = gerstner(pos, world_normal);
	vec3 new_pos = pos;
	world_normal = normal;
	gl_Position = Projection * View * vec4(new_pos, 1.0);
	world_position = new_pos;
	EmitVertex();
}

void main()
{
	vec3 p1 = gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[1].gl_Position.xyz;
	vec3 p3 = gl_in[2].gl_Position.xyz;
	//vec3 p4 = gl_in[3].gl_Position.xyz;

	//p2.y += 2;

	//p4.z += 5;



	//for (int i = 0; i <= instances; i++)
	//{
		//TODO modify offset so that instances are displayed on 6 columns
		vec3 offset = vec3(0, 0, 0);



		//TODO modify the points so that the triangle shrinks relative to its center
		f_color = g_color[0];
		EmitPoint(p1, offset, w_normal[0]);

		f_color = g_color[1];
		EmitPoint(p2, offset, w_normal[1]);

		f_color = g_color[2];
		EmitPoint(p3, offset, w_normal[2]);

		/*f_color = g_color[3];
		EmitPoint(p4, offset);*/

		EndPrimitive();
	//}
}
