#version 450

layout(location = 0) in vec2 in_texcoord;
layout(location = 1) in vec3 in_pos;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_pos_camspace;
layout(location = 4) in vec3 in_normal_camspace;
layout(location = 5) in vec4 in_cam_params;
layout(location = 6) in vec4 in_screen_color;

layout(binding = 0) uniform sampler2D tex_sampler;
layout(binding = 1) uniform sampler2D tex_sampler_2;

out vec4 out_color;

void main() {
    vec3 second_color = 0.9*texture(tex_sampler_2, in_texcoord).xyz;
    out_color = 1.5*in_screen_color*texture(tex_sampler, in_texcoord)+vec4(second_color, 1.0);
}
