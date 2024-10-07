#version 450

layout(location = 0) in vec2 in_texcoord;
layout(binding = 0) uniform sampler2D tex_sampler_orig;
layout(location = 1) in vec4 compose_params;

out vec4 out_color;

void main()
{   
    out_color = texture(tex_sampler_orig, in_texcoord)*compose_params.y;
}
