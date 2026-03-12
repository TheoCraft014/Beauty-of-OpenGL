#version 330

// Input
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniforms Texture & Camera
uniform sampler2D texture_1;
uniform vec3 eye_position;
uniform float alpha_threshold = 0.1f;

// Uniforms Lights (Fireflies)
uniform vec3 light_position[50];
uniform vec3 light_color[50];
uniform int num_fireflies;

// Uniforms Material
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// fog uniforms
uniform vec3 fogColor;
uniform float dmax;
uniform int fogEnabled;
uniform float fogDensity;

// Spotlight uniforms
uniform int num_spotlights;
uniform vec3 spotlight_position[2];
uniform vec3 spotlight_color[2];
uniform vec3 spotlight_direction[2];
uniform float spotlight_cone_angle[2];

uniform sampler2D cookie_text;

// Output
layout(location = 0) out vec4 out_color;

vec3 compute_spotlight(int i, vec3 world_position, vec3 world_normal)
{
    vec3 Lpos = spotlight_position[i];
    vec3 color = spotlight_color[i];
    float alpha = spotlight_cone_angle[i];
    vec3 dir = normalize(spotlight_direction[i]);

    vec3 v = world_position - Lpos;

    float spot = dot(-normalize(Lpos - world_position), dir);
    float spot_limit = cos(alpha);

    if (spot < spot_limit)
        return vec3(0, 0, 0);

    float spotfactor = pow((spot - spot_limit) / (1.0 - spot_limit), 2);

    vec3 z = dir;
    // produsul scalar
    float rez = dot(z, vec3(0, 1, 0));

    // vectorul arbitrar
    vec3 a = vec3(0);

    if (abs(rez) < 0.99) {
        a = vec3(0, 1, 0);
    } else {
        a = vec3(1, 0, 0);
    }

    vec3 x = normalize(cross(a, z));
    vec3 y = cross(z, x);

    float xs = dot(v, x);
    float ys = dot(v, y);
    float zs = dot(v, z);

    if (zs <= 0.0)
        return vec3(0, 0, 0);

    vec2 p = vec2(xs/zs, ys/zs);

    float R = tan(alpha);
    vec2 n = p/R;

    vec2 uv = n * 0.5f + 0.5f;

    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
        return vec3(0, 0, 0);

    vec3 cookie = texture(cookie_text, uv).rgb;
    return color * cookie * spotfactor;
}

void main()
{
    vec4 color_text = texture(texture_1, texcoord);
    if(color_text.a < alpha_threshold)
    discard;

    vec3 N = normalize(world_normal);
    vec3 V = normalize(eye_position - world_position);

    // Lumina ambientala de baza
    vec3 ambient = vec3(0.45);
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    // Calculam contributia fiecarui licurici
    for(int i = 0; i < num_fireflies; i++)
    {
        vec3 L = normalize(light_position[i] - world_position);
        float dist = distance(light_position[i], world_position);

        // Atenuare (scade lumina cu distanta)
        float att = 1.0 / (1.0 + 0.14 * dist + 0.07 * dist * dist);

        // Difuza
        float diff_factor = max(dot(N, L), 0.0);
        diffuse += light_color[i] * diff_factor * att;

        // Speculara
        vec3 R = reflect(-L, N);
        float spec_factor = pow(max(dot(V, R), 0.0), material_shininess);
        specular += light_color[i] * spec_factor * att;
    }

    vec3 light = ambient + material_kd * diffuse + material_ks * specular;
    int i;
    for (i = 0; i < num_spotlights; i++) {
        light += compute_spotlight(i, world_position, world_normal);
    }
    vec4 res = vec4(color_text.rgb * light, 1.0);

    // Fog logic
    float dist = distance(eye_position, world_position);
    float fogFactor = (1.0 - exp(-fogDensity * dist)) * float(fogEnabled);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    out_color = mix(res, vec4(fogColor, 1.0), fogFactor);
}
