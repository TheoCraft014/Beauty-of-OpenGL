#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform float Time;
uniform float TreeHeight;

uniform float BendFrequency;
uniform float BendPhase;

uniform float Amic;
uniform float Amediu;
uniform float Amare;

// 0 trunk, 1, 2, 3, 4, 5, 6 frunzele
uniform int partType;

out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{
    texcoord = v_texture_coord;
    vec3 pos = v_position;

    // inaltime normalizata
    float t = (pos.y + (TreeHeight/2)) / TreeHeight;

    // trunchi subtiere
    if (partType == 0)
    {
        float scale = 1.0 - t;
        pos.x *= scale;
        pos.z *= scale;
    }

    // frunze
    // decid amplitudinea in functie de pozitie
    float amplitude = 0.0f;

    if (partType == 0) amplitude = 0.0f;
    else if (partType == 1 || partType == 2)  amplitude = Amic;
    else if (partType == 3 || partType == 4)  amplitude = Amediu;
    else if (partType == 5 || partType == 6)  amplitude = Amare;

    if (amplitude > 0.0)
    {
        float wave = sin(Time * BendFrequency + t * 0.5 + BendPhase);
        float offset = wave * amplitude * t;

        pos.x += offset;
    }

    // Calculam pozitia si normala in spatiul lumii pentru iluminare
    world_position = (Model * vec4(pos, 1.0)).xyz;
    world_normal = normalize(mat3(Model) * v_normal);

    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}
