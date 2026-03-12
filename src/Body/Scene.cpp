//
// Created by theok on 20/01/2026.
//

#include "my_camera.h"
#include "transform_3D.h"
#include "Scene.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;


void Scene::InitCamera(float angle) {
    FoV = RADIANS(angle);
    zNear = 0.01f;
    zFar = 200.0f;

    res = window->GetResolution();
    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 12.f, 20.f),
              glm::vec3(0, 1, 0), Const2::UP);

    if (res.y != 0)
        aspr = (float)(res.x)/(float)res.y;
    else
        aspr = 1.0f;

    projection = glm::perspective(FoV, aspr, zNear, zFar);
}

Scene::Scene()
{
    InitCamera(60.f);
}


Scene::~Scene()
{
}

void Scene::LoadMeshes() {
    // Load meshes
    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, "src", "Body", "Models"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("sphere");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, "src", "Body", "Models"), "sphere.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("plane");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, "src", "Body", "Models"), "quad.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("cylinder");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, "src", "Body", "Models"), "cylinder.obj");
    meshes[mesh->GetMeshID()] = mesh;

    mesh = new Mesh("cone");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, "src", "Body", "Models"), "cone.obj");
    meshes[mesh->GetMeshID()] = mesh;
}

void Scene::LoadTextures() {
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, "src", "Body", "Textures");

    Texture2D* texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "ground.jpg").c_str(), GL_REPEAT);
    mapTextures["ground"] = texture;

    texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "Leaves.jpg").c_str(), GL_REPEAT);
    mapTextures["leaves"] = texture;

    texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "Wood.jpg").c_str(), GL_REPEAT);
    mapTextures["wood"] = texture;

    texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "tower.jpeg").c_str(), GL_REPEAT);
    mapTextures["tower"] = texture;

    texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "firefly.jpeg").c_str(), GL_REPEAT);
    mapTextures["firefly"] = texture;

    texture = new Texture2D();
    texture->Load2D(PATH_JOIN(sourceTextureDir, "imparatul-lumii.jpeg").c_str(), GL_REPEAT);
    mapTextures["cookie"] = texture;
}

void Scene::LoadShaders() {
    Shader *shader = new Shader("TreeShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, "src", "Body", "shaders", "TreeVShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, "src", "Body", "shaders", "TreeFShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    shader = new Shader("TerrainShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, "src", "Body", "Shaders", "TerrainVShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, "src", "Body", "Shaders", "TerrainFShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    shader = new Shader("TextureShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, "src", "Body", "Shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, "src", "Body", "Shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;
}

void Scene::Init()
{
    colorB = glm::vec3(0.329, 0.51, 0.69);
    LoadMeshes();
    LoadShaders();
    LoadTextures();
    InitFireflies();
    InitSpotlights();

    GenerateTrees(60);
}


void Scene::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(colorB.x, colorB.y, colorB.z, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, res.x, res.y);
}

void Scene::RenderTerrain(glm::mat4 model, Texture2D* texture) {
    Shader* shader = shaders.at("TerrainShader");
    Mesh* mesh = meshes.at("plane");

    if (!shader || !mesh) {
        std::cout << "Error: shader or mesh\n";
        return ;
    }

    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(model));

    // Bind view matrix
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

    // DETALII LUMINA
    glm::vec3 eyePosition = camera->position;
    glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(eyePosition));

    glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 0.5f);
    glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.5f);
    glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 30);

    // VARIABILE CEATA
    glUniform3f(glGetUniformLocation(shader->program, "fogColor"), colorB.x, colorB.y, colorB.z);
    glUniform1f(glGetUniformLocation(shader->program, "dmax"), 50.0f);
    glUniform1i(glGetUniformLocation(shader->program, "fogEnabled"), fogEnabled);
	glUniform1f(glGetUniformLocation(shader->program, "fogDensity"), fogDensity);

    // VARIABILE SPOTLIGHT
    {
        std::vector<glm::vec3> spotlightPositions;
        std::vector<glm::vec3> spotlightColors;
        std::vector <float> spotlightConAngles;
        std::vector<glm::vec3> spotlightDirections;

        for (spotlight s : spotlights) {
            spotlightPositions.push_back(s.position);
            spotlightColors.push_back(s.color);
            spotlightConAngles.push_back(s.coneAngle);
            spotlightDirections.push_back(s.direction);
        }

        glUniform1i(glGetUniformLocation(shader->program, "num_spotlights"), (int)spotlights.size());

        if (!spotlights.empty()) {
            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_position"),
                (int)spotlightPositions.size(), glm::value_ptr(spotlightPositions[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_color"),
                (int)spotlightColors.size(), glm::value_ptr(spotlightColors[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_direction"),
                (int)spotlightColors.size(), glm::value_ptr(spotlightDirections[0]));

            glUniform1fv(glGetUniformLocation(shader->program, "spotlight_cone_angle"),
                (int)spotlightConAngles.size(), spotlightConAngles.data());
        }
    }

    // Shader Firefly
    {
        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;

        float t = (float)Engine::GetElapsedTime();
        glm::vec3 center = glm::vec3(10, 0, 10); // Centrul turnului

        for (const auto& f : fireflies) {
            float currentRadius = f.baseRadius + f.radialAmp * sin(t * f.radialSpeed);
            float currentAngle = f.angleOffset + t * f.speed;

            float x = center.x + currentRadius * cos(currentAngle);
            float z = center.z + currentRadius * sin(currentAngle);
            float y = f.height + f.vAmp * sin(t * f.vSpeed);

            lightPositions.push_back(glm::vec3(x, y, z));
            lightColors.push_back(f.color);
        }

		// Send number of fireflies
        glUniform1i(glGetUniformLocation(shader->program, "num_fireflies"), (int)fireflies.size());

		// Send firefly positions and colors to the shader
        if (!fireflies.empty()) {
            glUniform3fv(glGetUniformLocation(shader->program, "light_position"),
                (int)lightPositions.size(), glm::value_ptr(lightPositions[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "light_color"),
                (int)lightColors.size(), glm::value_ptr(lightColors[0]));
        }
    }

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        int text1_loc = glGetUniformLocation(shader->program, "texture_1");
        glUniform1i(text1_loc, 0);
    }

    Texture2D* cookieText = mapTextures["cookie"];
    if (cookieText)
    {
        glActiveTexture(GL_TEXTURE1);

        glBindTexture(GL_TEXTURE_2D, cookieText->GetTextureID());
        int text_loc = glGetUniformLocation(shader->program, "cookie_text");
        glUniform1i(text_loc, 1);
    }

    glUniform1f(glGetUniformLocation(shader->program, "alpha_threshold"), 0.1f);
    glUniform2f(glGetUniformLocation(shader->program, "uvScale"), 200.0f, 200.0f);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Scene::Update(float deltaTimeSeconds)
{
    model = glm::mat4(1.f);
    model *= transform3D::RotateOX(M_PI_2);
    model *= transform3D::Scale(200, 200, 1);
    RenderTerrain(model, mapTextures["ground"]);

    model = glm::mat4(1.f);
    model *= transform3D::Translate(10, 0, 10);
    RenderTower();
    RenderFireflies();

    for (const auto& pos : treePositions)
    {
        glm::mat4 model = glm::mat4(1.f);
        model *= transform3D::Translate(pos.x, pos.y, pos.z);
		float scale = pos.w;
		model *= transform3D::Scale(scale, scale, scale);

        RenderTree(model, mapTextures["wood"], mapTextures["leaves"]);
    }
}


void Scene::FrameEnd()
{

}

void Scene::RenderMeshColor(const Mesh *mesh, const glm::mat4 & modelMatrix, const glm::vec3 & color)
{
    Shader* shader = shaders["Color"];

    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniform3f(shader->GetUniformLocation("color"), color.r, color.g, color.b);

    mesh->Render();
}

void Scene::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID()) {
        std::cout << "Error: mesh or shader not init\n";
        return;
    }

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = projection;
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // DETALII LUMINA
    {
        glm::vec3 eyePosition = camera->position;
        glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(eyePosition));

        glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 0.5f);
        glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.5f);
        glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 30);
    }

    // VARIABILE CEATA
    glUniform3f(glGetUniformLocation(shader->program, "fogColor"), colorB.x, colorB.y, colorB.z);
    glUniform1f(glGetUniformLocation(shader->program, "dmax"), 50.0f);
    glUniform1i(glGetUniformLocation(shader->program, "fogEnabled"), fogEnabled);
	glUniform1f(glGetUniformLocation(shader->program, "fogDensity"), fogDensity);

    // VARIABILE SPOTLIGHT
    {
        std::vector<glm::vec3> spotlightPositions;
        std::vector<glm::vec3> spotlightColors;
        std::vector <float> spotlightConAngles;
        std::vector<glm::vec3> spotlightDirections;

        for (spotlight s : spotlights) {
            spotlightPositions.push_back(s.position);
            spotlightColors.push_back(s.color);
            spotlightConAngles.push_back(s.coneAngle);
            spotlightDirections.push_back(s.direction);
        }

        glUniform1i(glGetUniformLocation(shader->program, "num_spotlights"), (int)spotlights.size());

        if (!spotlights.empty()) {
            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_position"),
                (int)spotlightPositions.size(), glm::value_ptr(spotlightPositions[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_color"),
                (int)spotlightColors.size(), glm::value_ptr(spotlightColors[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_direction"),
                (int)spotlightColors.size(), glm::value_ptr(spotlightDirections[0]));

            glUniform1fv(glGetUniformLocation(shader->program, "spotlight_cone_angle"),
                (int)spotlightConAngles.size(), spotlightConAngles.data());
        }
    }

    // Shader Firefly
    {
        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;

        float t = (float)Engine::GetElapsedTime();
        glm::vec3 center = glm::vec3(10, 0, 10); // Centrul turnului

        for (const auto& f : fireflies) {
            float currentRadius = f.baseRadius + f.radialAmp * sin(t * f.radialSpeed);
            float currentAngle = f.angleOffset + t * f.speed;

            float x = center.x + currentRadius * cos(currentAngle);
            float z = center.z + currentRadius * sin(currentAngle);
            float y = f.height + f.vAmp * sin(t * f.vSpeed);

            lightPositions.push_back(glm::vec3(x, y, z));
            lightColors.push_back(f.color);
        }

        // Send number of fireflies
        glUniform1i(glGetUniformLocation(shader->program, "num_fireflies"), (int)fireflies.size());

        // Send firefly positions and colors to the shader
        if (!fireflies.empty()) {
            glUniform3fv(glGetUniformLocation(shader->program, "light_position"),
                (int)lightPositions.size(), glm::value_ptr(lightPositions[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "light_color"),
                (int)lightColors.size(), glm::value_ptr(lightColors[0]));
        }
    }

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        int text1_loc = glGetUniformLocation(shader->program, "texture_1");
        glUniform1i(text1_loc, 0);
    }

    Texture2D* cookieText = mapTextures["cookie"];
    if (cookieText)
    {
        glActiveTexture(GL_TEXTURE1);

        glBindTexture(GL_TEXTURE_2D, cookieText->GetTextureID());
        int text_loc = glGetUniformLocation(shader->program, "cookie_text");
        glUniform1i(text_loc, 1);
    }

    glUniform1f(glGetUniformLocation(shader->program, "alpha_threshold"), 0.1f);

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Scene::RenderTree(const glm::mat4 &modelMatrix, Texture2D* trunkT, Texture2D* leavesT) {
    if (!trunkT || !leavesT)
        return;

    Shader* shader = shaders["TreeShader"];
    shader->Use();

    Mesh* trunk = meshes.at("cylinder");
    Mesh* leaves = meshes.at("box");

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrixs
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection));

    // uniforme comune
    glUniform1f(glGetUniformLocation(shader->program, "Time"),
                (GLfloat)Engine::GetElapsedTime());

    glUniform1f(glGetUniformLocation(shader->program, "TreeHeight"), 1.4f);

    glUniform1f(glGetUniformLocation(shader->program, "BendFrequency"), 1.4f);
    glUniform1f(glGetUniformLocation(shader->program, "BendPhase"), 3.14f);

    glUniform1f(glGetUniformLocation(shader->program, "Amic"), 0.04f);
    glUniform1f(glGetUniformLocation(shader->program, "Amediu"), 0.08f);
    glUniform1f(glGetUniformLocation(shader->program, "Amare"), 0.16f);

    // DETALII LUMINA
    {
        glm::vec3 eyePosition = camera->position;
        glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(eyePosition));

        glUniform1f(glGetUniformLocation(shader->program, "material_kd"), 0.5f);
        glUniform1f(glGetUniformLocation(shader->program, "material_ks"), 0.5f);
        glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), 30);
    }

    // VARIABILE CEATA
    glUniform3f(glGetUniformLocation(shader->program, "fogColor"), colorB.x, colorB.y, colorB.z);
    glUniform1f(glGetUniformLocation(shader->program, "dmax"), 50.0f);
    glUniform1i(glGetUniformLocation(shader->program, "fogEnabled"), fogEnabled);
	glUniform1f(glGetUniformLocation(shader->program, "fogDensity"), fogDensity);

    // VARIABILE SPOTLIGHT
    {
        std::vector<glm::vec3> spotlightPositions;
        std::vector<glm::vec3> spotlightColors;
        std::vector <float> spotlightConAngles;
        std::vector<glm::vec3> spotlightDirections;

        for (spotlight s : spotlights) {
            spotlightPositions.push_back(s.position);
            spotlightColors.push_back(s.color);
            spotlightConAngles.push_back(s.coneAngle);
            spotlightDirections.push_back(s.direction);
        }

        glUniform1i(glGetUniformLocation(shader->program, "num_spotlights"), (int)spotlights.size());

        if (!spotlights.empty()) {
            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_position"),
                (int)spotlightPositions.size(), glm::value_ptr(spotlightPositions[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_color"),
                (int)spotlightColors.size(), glm::value_ptr(spotlightColors[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "spotlight_direction"),
                (int)spotlightColors.size(), glm::value_ptr(spotlightDirections[0]));

            glUniform1fv(glGetUniformLocation(shader->program, "spotlight_cone_angle"),
                (int)spotlightConAngles.size(), spotlightConAngles.data());
        }
    }

    // Shader Firefly
    {
        std::vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> lightColors;

        float t = (float)Engine::GetElapsedTime();
        glm::vec3 center = glm::vec3(10, 0, 10); // Centrul turnului

        for (const auto& f : fireflies) {
            float currentRadius = f.baseRadius + f.radialAmp * sin(t * f.radialSpeed);
            float currentAngle = f.angleOffset + t * f.speed;

            float x = center.x + currentRadius * cos(currentAngle);
            float z = center.z + currentRadius * sin(currentAngle);
            float y = f.height + f.vAmp * sin(t * f.vSpeed);

            lightPositions.push_back(glm::vec3(x, y, z));
            lightColors.push_back(f.color);
        }

        // Send number of fireflies
        glUniform1i(glGetUniformLocation(shader->program, "num_fireflies"), (int)fireflies.size());

        // Send firefly positions and colors to the shader
        if (!fireflies.empty()) {
            glUniform3fv(glGetUniformLocation(shader->program, "light_position"),
                (int)lightPositions.size(), glm::value_ptr(lightPositions[0]));

            glUniform3fv(glGetUniformLocation(shader->program, "light_color"),
                (int)lightColors.size(), glm::value_ptr(lightColors[0]));
        }
    }

    glm::mat4 mm = modelMatrix;
    glm::mat4 m = mm;
    m *= transform3D::Scale(1, Const2::TREE_HEIGHT / 1.4, 1);
    m *= transform3D::Translate(0,  0.66, 0);
    glUniform1i(glGetUniformLocation(shader->program, "partType"), 0);
    RenderSimpleMesh(trunk, shader, m, trunkT);

    float y = 0;
    float scale = 5.f;
    int lvl;
    mm = m * transform3D::Scale(1, 1.4f/Const2::TREE_HEIGHT, 1);
    mm *= transform3D::Translate(0, -0.66 * 2, 0);
    for (lvl = 1; lvl <= Const2::NUM_FRUNZE; lvl++) {
        glm::mat4 m = mm;
        m *= transform3D::Translate(0, y, 0);
        m *= transform3D::Scale(scale, 0.5f, scale);

        glUniform1i(glGetUniformLocation(shader->program, "partType"), lvl);
        RenderSimpleMesh(leaves, shader, m, leavesT);

        y += 0.95f;
        scale *= 0.75f;
    }
}

void Scene::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 32.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            camera->TranslateForward(deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera->TranslateRight(- deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera->TranslateForward(- deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera->TranslateRight(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera->TranslateUpward(- deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }
    }
}


void Scene::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_F) {
		fogEnabled = !fogEnabled;
    }
}


void Scene::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Scene::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        camera->RotateFirstPerson_OX(-sensivityOX * (float)deltaY);
        camera->RotateFirstPerson_OY(-sensivityOY * (float)deltaX);
    }
}


void Scene::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Scene::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Scene::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Scene::OnWindowResize(int width, int height)
{
}

void Scene::RenderTower()
{
    glm::vec3 towerPos = Const2::TOWER_POS;

    Shader* shader = shaders["TextureShader"];
    Texture2D* texture = mapTextures["tower"];
    Mesh* meshBox = meshes["box"];
    Mesh* meshCone = meshes["cone"];

    float pillarH = 10.0f;
    float pillarW = 0.5f;
    float dist = 2.0f;

    glm::vec2 pillarOffsets[4] = {
        glm::vec2(dist, dist),  glm::vec2(-dist, dist),
        glm::vec2(dist, -dist), glm::vec2(-dist, -dist)
    };

    // Render the 4 pillars
    for (int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, towerPos);
        model = glm::translate(model, glm::vec3(pillarOffsets[i].x, pillarH / 2.0f, pillarOffsets[i].y));
        model = glm::scale(model, glm::vec3(pillarW, pillarH, pillarW));
        RenderSimpleMesh(meshBox, shader, model, texture);
    }

    // Platform will be a squished cube
    {
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, towerPos);
        model = glm::translate(model, glm::vec3(0, 2*pillarH / 3, 0));
		// Scale enough to cover the pillars( dist *2 + pillarW )
        model = glm::scale(model, glm::vec3(5.0f, 0.4f, 5.0f));
        RenderSimpleMesh(meshBox, shader, model, texture);
    }

    // Roof
    {
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, towerPos);
        model = glm::translate(model, glm::vec3(0, pillarH + 1.0f, 0));
        model = glm::scale(model, glm::vec3(4.5f, 1.5f, 4.5f));
        RenderSimpleMesh(meshCone, shader, model, texture);
    }

    // Spotlights
	for (int i = -1; i <= 1; i += 2) { // 2 spotlights on X axis
        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, towerPos);
        model = glm::translate(model, glm::vec3(i * 2.5f, 2 * pillarH / 3 - 0.6f, 0));
        model = glm::rotate(model, glm::radians(i * 45.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        RenderSimpleMesh(meshCone, shader, model, texture);
    }
}

void Scene::InitSpotlights() {
    spotlights.clear();

    float pillarH = 10.0f;
    for (int i = -1; i <= 1; i = i + 2) {
        spotlight s{};
        glm::vec3 spotlightOffset = glm::vec3((float)i * 2.5f, 2 * pillarH / 3 - 0.6f, 0);
        s.position = Const2::TOWER_POS + spotlightOffset;
        s.color = Colors2::WHITE_;
        s.coneAngle = glm::radians(30.f);
        s.direction = glm::vec3(i * sin(s.coneAngle), -cos(s.coneAngle), 0);

        spotlights.push_back(s);
    }
}

void Scene::InitFireflies() {
    fireflies.clear();

    srand((unsigned int)time(NULL));

    for (int i = 0; i < Const2::NUM_FIREFLIES; i++) {
        firefly f{};

        // 3 - 20
        f.baseRadius = 3.0f + (rand() % 1700) / 100.0f;
		// 0.5 - 5.0
        f.radialAmp = 0.5f + (rand() % 450) / 100.0f;
        f.radialSpeed = 0.5f + (rand() % 100) / 100.0f;


        f.angleOffset = (rand() % 360) * (M_PI / 180.0f);
		//0.3 - 1.5
        f.speed = 0.3f + (rand() % 120) / 100.0f;
        if (rand() % 2 == 0) f.speed *= -1;

		// 1.0 - 6.0
        f.height = 1.0f + (rand() % 500) / 100.0f;
        f.vAmp = 0.5f + (rand() % 100) / 100.0f;
        f.vSpeed = 0.8f + (rand() % 150) / 100.0f;

		// culori random
        float t = (rand() % 100) / 100.0f;
        float r = 1.0f - 0.3f * t;
        float g = 1.0f;
        float b = 0.7f;

        f.color = glm::vec3(r, g, b);;

        fireflies.push_back(f);
    }
}

void Scene::RenderFireflies()
{
    glm::vec3 center = glm::vec3(10, 0, 10);
    float t = (float)Engine::GetElapsedTime();
    Mesh* mesh = meshes["sphere"];
    Shader* shader = shaders["TextureShader"];
    Texture2D* texture = mapTextures["firefly"];

    for (const auto& f : fireflies) {

        float currentRadius = f.baseRadius + f.radialAmp * sin(t * f.radialSpeed);
        float currentAngle = f.angleOffset + t * f.speed;

        float x = center.x + currentRadius * cos(currentAngle);
        float z = center.z + currentRadius * sin(currentAngle);
        float y = f.height + f.vAmp * sin(t * f.vSpeed);

        glm::mat4 model = glm::mat4(1);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.15f)); // Licurici mici
        RenderSimpleMesh(mesh, shader, model, texture);
    }
}

void Scene::GenerateTrees(int numTrees)
{
    // Centrul turnului
    float centerX = Const2::TOWER_POS.x;
    float centerZ = Const2::TOWER_POS.z;
    float spread = 40.0f;
    treePositions.clear();

    for (int i = 0; i < numTrees; i++) {
        float offsetX = (rand()% (int)(spread * 2)) - spread;
        float offsetZ = (rand()% (int)(spread * 2)) - spread;
        float x = 10  + offsetX;
        float z = 10 + offsetZ;

        if (abs(x - centerX) < 7.0f && abs(z - centerZ) < 7.0f) {
            i--;
            continue;
        }
        float scale = 0.9f + (rand() % 80) / 100.0f;

        treePositions.push_back(glm::vec4(x, 0, z, scale));
    }
}
