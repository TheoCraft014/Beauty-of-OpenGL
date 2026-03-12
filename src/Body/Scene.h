#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"
#include "MeshMaker.h"
#include "my_camera.h"


    struct firefly {
        float baseRadius;
        float angleOffset;
        float speed;
        float radialAmp;
        float radialSpeed;

        float height;
        float vAmp;
        float vSpeed;

        glm::vec3 color;
    };

    struct spotlight {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec3 direction;
        float coneAngle; // alpha
    };


    class Scene : public gfxc::SimpleScene
    {
    public:
        Scene();
        ~Scene();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D *texture1 = nullptr);
        Texture2D *CreateRandomTexture(unsigned int width, unsigned int height);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void InitCamera(float angle);
        void LoadMeshes();
        void LoadTextures();
        void LoadShaders();

        // render mesh aceeasi culoare pe toti vertecsii - shaderul Color
        void RenderMeshColor(const Mesh * mesh, const glm::mat4 & modelMatrix, const glm::vec3 & color);

        std::unordered_map<std::string, Texture2D *> mapTextures;

        void RenderTerrain(glm::mat4 model, Texture2D* texture);
        void RenderTree(const glm::mat4 &modelMatrix, Texture2D* trunk, Texture2D* leaves);
        void GenerateTrees(int numTrees);

        void RenderTower();

        // spotlights
        void InitSpotlights();

        // fireflies
        void InitFireflies();
        void RenderFireflies();

        glm::ivec2 res;
        glm::mat4 model;
        glm::vec3 colorB;

    protected:
        std::vector<firefly> fireflies;
        std::vector<spotlight> spotlights;
        implemented::Camera *camera;

        // matricea proiectie a scenei
        glm::mat4 projection;

        // verifica daca este perspectiva
        bool isP;

        // aspect ratio
        float aspr;

        // parametrii
        float FoV, zFar, zNear;
        float left, right, top, bottom;
		int fogEnabled = 1;
		float fogDensity = 0.02f;
		std::vector<glm::vec4> treePositions;
    };
