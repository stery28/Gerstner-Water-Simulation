#pragma once

#include <Component/SimpleScene.h>
#include <vector>

class Proiect : public SimpleScene
{
public:
	Proiect();
	~Proiect();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	unsigned int UploadCubeMapTexture(const std::string &posx, const std::string &posy, const std::string &posz, const std::string& negx, const std::string& negy, const std::string& negz);
	void RenderMeshInstanced(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture);
	void RenderWater(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	glm::vec2 WorldToScreenCoords(glm::vec3 current_point);


protected:
	//informatii suprafata generate
	int cubeMapTextureID;
	float water_size = 10.0f;
	glm::vec3 water_color = glm::vec3(0, 0.2f, 0.7f);
	glm::vec3 light_position = glm::vec3(0, 12, 0);
	int waves_count = 5;
	std::vector<glm::vec2> directions = std::vector<glm::vec2>();// = new std::vector<glm::vec2>();
	std::vector<float> steepness = std::vector<float>();// = new std::vector<float>();
	std::vector<float> wavelength = std::vector<float>();// = new std::vector<float>();
	//TODO: Add multiple configs
	// std::unordered_map<std::string, Texture2D*> mapTextures;
	Texture2D *ground_texture;
	Texture2D *river_texture;
	float delta_time = 0.0f;
	bool wireframe = false;
};
