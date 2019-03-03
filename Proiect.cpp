#include "Proiect.h"

#include <vector>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stb/stb_image.h>
#include <cmath>

#include <Core/Engine.h>
#include "Laboratoare/Laborator2/Laborator2.h"

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/World.cpp

Proiect::Proiect()
{
}

Proiect::~Proiect()
{
}

void Proiect::Init()
{
	auto camera = GetSceneCamera();
	camera->SetPositionAndRotation(glm::vec3(0, 4, 4), glm::quat(glm::vec3(-30 * TO_RADIANS, 0, 0)));
	//camera->SetPositionAndRotation(glm::vec3(0, 20, 48), glm::quat(glm::vec3(-40 * TO_RADIANS, 0, 0)));
	// camera->SetPositionAndRotation(glm::vec3(0, 0, 25), glm::quat(glm::vec3(0)));
	camera->Update();

	srand(time(NULL));
	std::string texturePath = "Source/Teme/Proiect/Textures/Cubemap/";

	ToggleGroundPlane();

	// Create a shader program for surface generation
	{
		Shader *shader = new Shader("SurfaceGeneration");
		shader->AddShader("Source/Teme/Proiect/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Teme/Proiect/Shaders/GeometryShader.glsl", GL_GEOMETRY_SHADER);
		shader->AddShader("Source/Teme/Proiect/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// CubeMap shader
	{
		Shader *shader = new Shader("CubeMap");
		shader->AddShader("Source/Teme/Proiect/Shaders/CubeMap.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Teme/Proiect/Shaders/CubeMap.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Textures shader
	// Create a shader program for particle system
	{
		Shader *shader = new Shader("TextureShader");
		shader->AddShader("Source/Teme/Proiect/Shaders/Texture.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Teme/Proiect/Shaders/Texture.FS.glsl", GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Shader for objects with no texture, using Phong for illumination
	{
		Shader *shader = new Shader("ClassicShader");
		shader->AddShader("Source/Teme/Proiect/Shaders/ClassicShader.VS.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Teme/Proiect/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	cubeMapTextureID = UploadCubeMapTexture(
		texturePath + "posx.png",
		texturePath + "posy.png",
		texturePath + "posz.png",
		texturePath + "negx.png",
		texturePath + "negy.png",
		texturePath + "negz.png"
	);

	{
		// Water plane
		vector<glm::vec3> vertices
		{
			glm::vec3(water_size, 0, water_size),// water_color),
			glm::vec3(water_size, 0, -water_size),// water_color),
			glm::vec3(-water_size, 0, -water_size),// water_color),
			glm::vec3(-water_size, 0, water_size)// water_color)
			/*VertexFormat(glm::vec3(2*water_size, 0, 2*water_size), water_color),
			VertexFormat(glm::vec3(2*water_size, 0, water_size), water_color),
			VertexFormat(glm::vec3(water_size, 0, water_size), water_color),
			VertexFormat(glm::vec3(water_size, 0, 2*water_size), water_color)*/
		};

		vector<unsigned short> indices =
		{
			0, 1, 3,
			1, 2, 3
			/*4, 5, 7,
			5, 6, 7*/
		};

		vector<glm::vec3> normals
		{
			glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0)
			/*glm::vec3(0, 1, 1),
			glm::vec3(1, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 1, 0)*/
		};

		vector<glm::vec2> textureCoords
		{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f)
			/*glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f)*/
		};
		meshes["simple_plane"] = new Mesh("simple_plane"); // To map the reflection texture for testing purpose
		meshes["simple_plane"]->InitFromData(vertices, normals, textureCoords, indices);

		meshes["water"] = new Mesh("water");
		meshes["test"] = new Mesh("test");
		//meshes["water"]->InitFromData(vertices, normals, textureCoords, indices);
		//meshes["test"]->InitFromData(vertices, indices);
		meshes["test"]->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");

		//meshes["water"]->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		meshes["water"]->LoadMesh("Source/Teme/Proiect/Models", "plane100.obj");
		meshes["water"]->UseMaterials(false);
		//meshes["water"]->SetDrawMode(GL_TRIANGLES);

		//directions.push_back(glm::vec2(1, 0.5f));
		//wavelength.push_back(50.0f);
		for (int i = 0; i < max_waves_count; i++) {
			directions.push_back(glm::vec2((float)(rand() % 10) / 10, (float)(rand() % 10) / 10));
			wavelength.push_back((float)(rand() % 10 + 1));
		}
	}

	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		mesh->UseMaterials(false);
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("scene");
		mesh->LoadMesh("Source/Teme/Proiect/Models", "scene.obj");
		mesh->UseMaterials(true);
		meshes[mesh->GetMeshID()] = mesh;
	}

	// Create a bogus mesh with 2 points (a line)
	{
		vector<VertexFormat> vertices
		{
			VertexFormat(glm::vec3(-4.0, -2.5,  1.0), glm::vec3(0, 1, 0)),
			VertexFormat(glm::vec3(-4.0, 5.5,  1.0), glm::vec3(0, 1, 0))
		};

		vector<unsigned short> indices =
		{
			0, 1
		};

		meshes["surface"] = new Mesh("generated initial surface points");
		meshes["surface"]->InitFromData(vertices, indices);
		meshes["surface"]->SetDrawMode(GL_LINES);
	}

	{
		const string texture_loc = "Source/Teme/Proiect/Textures/";
		// Load texture
		dudv_texture = TextureManager::LoadTexture(texture_loc + "dudv.png", nullptr, "dudv", true, true);
		// ground_texture = TextureManager::LoadTexture(texture_loc + "dirt.jpg", nullptr, "ground", true, true);
		// river_texture = TextureManager::LoadTexture(texture_loc + "river.jpg", nullptr, "river", true, true);
		// ground_texture = TextureManager::LoadTexture(RESOURCE_PATH::TEXTURES + "ground.jpg", nullptr, "image", true, true);
		// ground_texture = TextureManager::LoadTexture(texture_loc + "dirt.jpg", nullptr, "image", true, true);
		// Texture2D *texture = new Texture2D();
		// ground_texture = new Texture2D();
		// ground_texture->Load2D((texture_loc + "dirt.jpg").c_str(), GL_REPEAT);
		// texture->Load2D((texture_loc + "dirt.jpg").c_str(), GL_REPEAT);
		// mapTextures["ground"] = texture;
	}


	{
		// FBOs for reflection and refraction
		auto resolution = window->GetResolution();
		fbo_reflection = new FrameBuffer();
		fbo_reflection->Generate(resolution.x, resolution.y, 1);

		fbo_refraction = new FrameBuffer();
		fbo_refraction->Generate(resolution.x, resolution.y, 1);
	}
}

unsigned int Proiect::UploadCubeMapTexture(const std::string &posx, const std::string &posy, const std::string &posz, const std::string& negx, const std::string& negy, const std::string& negz)
{
	int width, height, chn;

	unsigned char* data_posx = stbi_load(posx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posy = stbi_load(posy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_posz = stbi_load(posz.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negx = stbi_load(negx.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negy = stbi_load(negy.c_str(), &width, &height, &chn, 0);
	unsigned char* data_negz = stbi_load(negz.c_str(), &width, &height, &chn, 0);

	// TODO - create OpenGL texture
	unsigned int textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// TODO - bind the texture

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	float maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// TODO - load texture information for each face
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_posz);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negx);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negy);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_negz);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// free memory
	SAFE_FREE(data_posx);
	SAFE_FREE(data_posy);
	SAFE_FREE(data_posz);
	SAFE_FREE(data_negx);
	SAFE_FREE(data_negy);
	SAFE_FREE(data_negz);

	return textureID;
}

void Proiect::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Proiect::RenderMeshInstanced(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader 
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "river_texture"), 1);

	glUniform1f(glGetUniformLocation(shader->program, "time"), delta_time);

	// Draw the object instanced
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElementsInstanced(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, (void*)0, 1);

}

void Proiect::RenderWater(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture) 
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glUniform1i(glGetUniformLocation(shader->program, "textureImage"), 0);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}


void Proiect::RenderSkybox()
{
	auto camera = GetSceneCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		Shader *shader = shaders["CubeMap"];
		shader->Use();

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(100));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		meshes["cube"]->Render();
		//RenderMesh(meshes["cube"], shader, glm::mat4(1));
	}
}

void Proiect::RenderEnvironment(glm::vec4 &clip_plane)
{
	auto camera = GetSceneCamera();
	Shader *shader;

	{
		shader = shaders["ClassicShader"];
		shader->Use();
		glUniform3fv(glGetUniformLocation(shader->program, "camera_position"), 1, glm::value_ptr(camera->transform->GetWorldPosition()));
		glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(shader->program, "Color"), 1, glm::value_ptr(glm::vec3(0, 0.7f, 0)));
		glUniform1i(glGetUniformLocation(shader->program, "reflective"), 0);
		glUniform4fv(glGetUniformLocation(shader->program, "clip_plane"), 1, glm::value_ptr(clip_plane));
		RenderMesh(meshes["test"], shader, glm::mat4(1));
		RenderMesh(meshes["sphere"], shader, glm::translate(glm::mat4(1), light_position));
	}

	{
		shader = shaders["ClassicShader"];
		shader->Use();
		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.4f));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);
		glUniform1i(glGetUniformLocation(shader->program, "shininess"), 1);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->program, "textureImage"), 0);

		//meshes["scene"]->Render();
		RenderMesh(meshes["scene"], shader, modelMatrix);
		glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 0);
	}
}


void Proiect::Update(float deltaTimeSeconds)
{
	ClearScreen();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	delta_time += deltaTimeSeconds;
	move_factor += wave_speed * deltaTimeSeconds;
	move_factor = fmod(move_factor, 10);

	auto camera = GetSceneCamera();

	float distance = 2 * (camera->transform->GetWorldPosition().y);
	cout << distance << endl;
	glm::vec3 camera_pos = camera->transform->GetWorldPosition();
	camera_pos.y -= distance;
	camera->transform->SetWorldPosition(camera_pos);
	glm::vec3 camera_rotation = camera->transform->GetRotationEuler360();
	camera_rotation.x = -camera_rotation.x;
	camera->transform->SetWorldRotation(camera_rotation);
	camera->Update();

	// draw the cubemap
	fbo_reflection->Bind();
	RenderSkybox();
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		Shader *shader = shaders["CubeMap"];
		shader->Use();

		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(100));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		meshes["cube"]->Render();
		//RenderMesh(meshes["cube"], shader, glm::mat4(1));
	}*/

	//send uniforms to shaders
	Shader *shader;

	//TODO 
	//trimiteti la shadere numarul de puncte care aproximeaza o curba (no_of_generated_points)
	// glUniform1i(glGetUniformLocation(shader->program, "no_of_generated_points"), no_of_generated_points);
	//si caracteristici pentru crearea suprafetelor de translatie/rotatie (max_translate, max_rotate)
	// glUniform1f(glGetUniformLocation(shader->program, "max_translate"), max_translate);
	// glUniform1f(glGetUniformLocation(shader->program, "max_rotate"), max_rotate);

	//Mesh* mesh = meshes["surface"];
	//draw the object instanced
	//RenderMeshInstanced(mesh, shader, glm::mat4(1), river_texture);
	

	glEnable(GL_CLIP_DISTANCE0); // Clip plane for cutting scene for textures

	//fbo_reflection->Bind();
	RenderEnvironment(reflection_clip_plane);

	// Simple objects with no texture
	/*{
		shader = shaders["ClassicShader"];
		shader->Use();
		glUniform3fv(glGetUniformLocation(shader->program, "camera_position"), 1, glm::value_ptr(camera->transform->GetWorldPosition()));
		glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(shader->program, "Color"), 1, glm::value_ptr(glm::vec3(0, 0.7f, 0)));
		glUniform1i(glGetUniformLocation(shader->program, "reflective"), 0);
		glUniform4fv(glGetUniformLocation(shader->program, "clip_plane"), 1, glm::value_ptr(clip_plane));
		RenderMesh(meshes["test"], shader, glm::mat4(1));
		RenderMesh(meshes["sphere"], shader, glm::translate(glm::mat4(1), light_position));
	}*/

	// Objects with texture
	/*{
		shader = shaders["ClassicShader"];
		shader->Use();
		glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(0.4f));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);
		glUniform1i(glGetUniformLocation(shader->program, "shininess"), 1);

		//meshes["scene"]->Render();
		RenderMesh(meshes["scene"], shader, modelMatrix);
		glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 0);
	}*/

	camera_pos.y += distance;
	camera->transform->SetWorldPosition(camera_pos);
	camera_rotation.x = -camera_rotation.x;
	camera->transform->SetWorldRotation(camera_rotation);
	camera->Update();

	cout << "--- " << camera_pos << endl;


	glDisable(GL_CLIP_DISTANCE0);

	//Refraction pass
	fbo_refraction->Bind();
	RenderSkybox();
	glEnable(GL_CLIP_DISTANCE0);
	RenderEnvironment(refraction_clip_plane);
	glDisable(GL_CLIP_DISTANCE0);

	FrameBuffer::BindDefault();
	RenderSkybox();
	RenderEnvironment(glm::vec4(0));

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	/*{
		// FBO texture test
		shader = shaders["ClassicShader"];
		shader->Use();
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 5, 0));

		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
		glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
		glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);
		glUniform1i(glGetUniformLocation(shader->program, "shininess"), 1);

		//meshes["scene"]->Render();
		//glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->program, "textureImage"), 1);
		fbo_reflection->BindTexture(0, GL_TEXTURE0 + 1);

		RenderMesh(meshes["simple_plane"], shader, modelMatrix);
		glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 0);
	}*/

	// Render Water the last, because of the reflection/refraction textures
	{
		Shader *shader = shaders["SurfaceGeneration"];
		shader->Use();
		// Water texture
		// RenderGround(meshes["water"], shaders["TextureShader"], glm::mat4(1), ground_texture);
		//RenderMesh(meshes["water"], shaders["VertexColor"], glm::mat4(1));
		glUniform1f(glGetUniformLocation(shader->program, "time"), delta_time);
		glUniform1i(glGetUniformLocation(shader->program, "waves_count"), waves_count);
		for (int i = 0; i < waves_count; i++)
		{
			//glUniform2fv(glGetUniformLocation(shader->program, "directions"), 1, glm::value_ptr(directions[i]));
			//glUniform1fv(glGetUniformLocation(shader->program, "wavelength"), 1, &wavelength[i]);
		}
		//glUniform1f(glGetUniformLocation(shader->program, "wavelength"), wavelength[0]);
		glUniform2fv(glGetUniformLocation(shader->program, "directions"), waves_count, glm::value_ptr(directions[0]));
		glUniform1fv(glGetUniformLocation(shader->program, "wavelength"), waves_count, &wavelength[0]);
		glUniform3fv(glGetUniformLocation(shader->program, "camera_position"), 1, glm::value_ptr(camera->transform->GetWorldPosition()));
		glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(shader->program, "Color"), 1, glm::value_ptr(water_color));
		glUniform1i(glGetUniformLocation(shader->program, "reflective"), 1);
		glUniform1i(glGetUniformLocation(shader->program, "shininess"), 32);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);
		int loc_texture = shader->GetUniformLocation("texture_cubemap");
		glUniform1i(loc_texture, 0);

		glUniform1i(glGetUniformLocation(shader->program, "reflection_texture"), 1);
		fbo_reflection->BindTexture(0, GL_TEXTURE0 + 1);
		glUniform1i(glGetUniformLocation(shader->program, "refraction_texture"), 2);
		fbo_refraction->BindTexture(0, GL_TEXTURE0 + 2);

		//glActiveTexture(GL_TEXTURE0 + 3);
		dudv_texture->BindToTextureUnit(GL_TEXTURE0 + 3);
		glUniform1i(glGetUniformLocation(shader->program, "dudv_texture"), 3);
		glUniform1f(glGetUniformLocation(shader->program, "move_factor"), move_factor);

		cout << delta_time << directions[0] << directions[1] << wavelength[0] << endl;
		RenderMesh(meshes["water"], shader, glm::scale(glm::mat4(1), glm::vec3(0.4f)));
		cout << meshes["water"]->texCoords[0] << " " << meshes["water"]->texCoords[1] << endl;
		//RenderMesh(meshes["test"], shader, glm::mat4(1));
		//RenderMesh(meshes["sphere"], shader, glm::translate(glm::mat4(1), light_position));
	}

	

}

void Proiect::FrameEnd()
{
	//DrawCoordinatSystem();
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Proiect::OnInputUpdate(float deltaTime, int mods)
{
	// treat continuous update based on input
	float speed = 10;
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		if (window->KeyHold(GLFW_KEY_W)) {
			light_position -= glm::vec3(0, 0, 1) * deltaTime * speed;
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			light_position += glm::vec3(0, 0, 1) * deltaTime * speed;
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			light_position -= glm::vec3(1, 0, 0) * deltaTime * speed;
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			light_position += glm::vec3(1, 0, 0) * deltaTime * speed;
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			light_position -= glm::vec3(0, 1, 0) * deltaTime * speed;
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			light_position += glm::vec3(0, 1, 0) * deltaTime * speed;
		}
	}
};

void Proiect::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_F) {
		wireframe = !wireframe;
	}

	if (key == GLFW_KEY_R) {
		for (int i = 0; i < max_waves_count; i++) {
			directions[i] = glm::vec2((float)(rand() % 10) / 10, (float)(rand() % 10) / 10);
			if (directions[i][0] == 0 && directions[i][1] == 0) {
				int index = rand() % 2;
				while (directions[i][index] == 0) {
					directions[i][index] = (float)(rand() % 10) / 10;
				}
			}
			wavelength[i] = (float)(rand() % 10 + 1);
		}
	}

	if (key == GLFW_KEY_KP_8) {
		if (waves_count < max_waves_count)
		{
			waves_count++;
		}
	}

	if (key == GLFW_KEY_KP_2) {
		if (waves_count > 0)
		{
			waves_count--;
		}
	}
};

void Proiect::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Proiect::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
	
};

void Proiect::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Proiect::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Proiect::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Proiect::OnWindowResize(int width, int height)
{
	// treat window resize event
}

glm::vec2 Proiect::WorldToScreenCoords(glm::vec3 current_point) {
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	glm::ivec2 viewOffset = glm::ivec2(0);
	glm::ivec2 viewSize = window->GetResolution();

	glm::vec4 clipSpacePos = projectionMatrix * (viewMatrix * glm::vec4(current_point, 1.0));
	glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos.x, clipSpacePos.y, clipSpacePos.z) / clipSpacePos.w;
	glm::vec2 windowSpacePos = glm::vec2(
			((ndcSpacePos.x + 1.0) / 2.0) * viewSize.x + viewOffset.x,
			((1.0 - ndcSpacePos.y) / 2.0) * viewSize.y + viewOffset.y
		);
	return windowSpacePos;
}
