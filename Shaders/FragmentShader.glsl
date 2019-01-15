#version 430

layout(location = 0) in vec3 f_color;

uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;
uniform vec3 camera_position;
uniform vec3 light_position;
uniform vec3 Color;

in vec3 world_position;
in vec3 world_normal;

//const vec3 light_position = vec3(10, 7, 0);
const vec3 light_color = vec3(0.5f);
const float light_radius = 30.0f;

layout(location = 0) out vec4 out_color;

// ----------------------- Phong -----------------------
const vec3 ld = vec3(0.3);	// Diffuse factor
const vec3 ls = vec3(0.3);	// Specular factor
const float specular_exponent = 40.0;	// Specular exponent
const vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
//const vec3 Color = vec3(0, 0.2f, 0.7f);
vec4 phong2() {
	vec3 L = normalize(light_position - world_position);
	vec3 N = world_normal;
	vec3 V = normalize(camera_position - world_position);
	vec3 H = normalize(L + V);
	//vec3 diffuse = ld * light_color * max(N * L, 0);
	vec3 diffuse = Color;
	float intensity = max(dot(N, L), 0);
	vec3 specular = vec3(0);
	if (intensity > 0)
	{
		specular = ls * light_color * pow(max(dot(N, H), 0), specular_exponent);
		float d = distance(light_position, world_position);
		float f = 1 / pow(d, 2);
	}
	//return vec4(f*(diffuse + specular), 1.0);
	return vec4(max(intensity*diffuse + specular, ambient), 1.0f);
}

/*vec4 phong(vec3 w_pos, vec3 w_N)
{
	vec3 L = normalize(light_position - w_pos);

	float dist = distance(light_position, w_pos);

	// TODO
	// Ignore fragmets outside of the light influence zone (radius)
	if (dist > light_radius)
		return vec4(1);

	float att = pow(light_radius - dist, 2);

	float dot_specular = dot(w_N, L);
	vec3 specular = vec3(0);
	if (dot_specular > 0)
	{
		vec3 V = normalize(camera_position - w_pos);
		vec3 H = normalize(L + V);
		specular = ls * pow(max(dot(w_N, H), 0), specular_exponent);
	}

	vec3 diffuse = ld * max(dot_specular, 0);
	//vec3 diffuse = vec3(0, 0.2f, 0.7f);

	return vec4(att * (diffuse + specular), 1.0);
}*/

vec4 phong(vec3 w_pos, vec3 w_N)
{
	vec3 L, V, H;

	float ka = 0.25, kd = 0.5, ks = 0.5;

	float factor;

	//vec3 diffuse = vec3(0, 0, 0);
	vec3 diffuse = Color;
	vec3 specular = vec3(0);
	int material_shininess = 2;

	float lighting = 0;
	float diffuseFact, specularFact;

	//vec3 tmp_color = ((factor + ka) * diffuse.xyz + factor * specular.xyz) * light_color;
	vec3 tmp_color = ambient;

	L = normalize(light_position - w_pos);
	V = normalize(camera_position - w_pos);
	H = normalize(L + V);

	//factor = 1/(pow(distance(world_position, L), 2));
	factor = 1 / (pow(distance(light_position, world_position), 2));

	lighting = 0;
	if (max(dot(L, w_N), 0) > 0) {
		lighting = 1;
	}
	//factor = factor * lighting;
	diffuseFact = max(dot(L, w_N), 0);
	specularFact = lighting * pow(max(dot(w_N, H), 0), material_shininess);

	tmp_color += ((diffuseFact + ka) * diffuse.xyz + specularFact * specular.xyz) * light_color * factor;

	//tmp_color += ((diffuseFact + ka) * kd + specularFact * ks) * light_color * factor * Color;
	return vec4(tmp_color, 1);
}

void main()
{
	//out_color = vec4(f_color, 0);
	//out_color = vec4(0, 0.2f, 0.7f, 0);
	out_color = phong(world_position, world_normal);
	//out_color = vec4(world_position, 1);


	//out_color = phong2(); // Best one
	//out_color = vec4(world_normal, 1);

	/*if (world_normal.xyz == vec3(0))
		out_color = vec4(0, 1, 0, 1);*/
}