#version 450

layout(location=0) in vec2 in_texcoord;
layout(location=1) in vec4 extra_in;
layout(location=2) in vec4 in_effect_params;

layout(binding = 0) uniform sampler2D tex_sampler;
layout(binding = 1) uniform sampler2D tex_sampler2;

out vec4 out_color;
const float PI = 3.141592;

vec3 bunny_intro(vec2 uv) {
    float i = floor(mod(in_effect_params.x, 64.));
    vec2 frame_xy = vec2(mod(i, 8.), floor(i/8.));
    vec2 uv_ = (uv-vec2(0.5,0.5))*vec2(1.333,1.0)+vec2(0.5,0.5);
    uv_ = (uv_+frame_xy)/vec2(8.,8.);
    return texture(tex_sampler, uv_).xyz;
}

vec3 red(vec2 uv) {
    return vec3(1.0, 0.0, 0.0);
}

float rand(float n){return fract(sin(n) * 43758.5453123);}

vec3 noise(vec2 uv) {
    float n = rand(uv.x*0.9878+uv.y*rand(uv.y+uv.x)+rand(in_effect_params.x));
    return vec3(n, n, n);
}

vec3 plain(vec2 uv) {
    return texture(tex_sampler, uv).xyz;
}

vec3 plain2(vec2 uv) {
    vec2 uv_ = (uv-vec2(0.5,0.5))*vec2(1.333,1.0)+vec2(0.5,0.5);
    return texture(tex_sampler, uv_).xyz;
}

vec3 homebrew(vec2 uv) {
    float x = in_effect_params.x;
    float uv_y = 1.-uv.y;
    float height = x*0.05+0.06*sin(x*2.);
    float s1 = (1.-step(uv_y+0.07*sin(uv.x*10.+x)+0.02*sin(uv.x*5.-x), height))*0.4+1.;
    float s2 = (1.-step(uv_y+0.02*sin(uv.x*23.-x)+0.02*sin(uv.x*5.-x), height))*0.3+1.;
    return vec3(0.44,0.5,1.0)*s1*s2;
}

vec3 rainbow(float h, float s, float v) {
  vec3 c = vec3(h, s, v);
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rainbow_twister(vec2 uv) {
    float iTime = in_effect_params.x;
    float twist_y = mod(iTime+uv.y+1.4*sin(uv.y*5.+cos(iTime))+0.55*sin(uv.y*2.3+cos(iTime*0.9)), PI/2.);
    float twist_x = 0.05*sin(iTime+uv.y*5.4);
    float twist_width = 0.12;
    float a = sin(twist_y+PI/4.);
    float b = sin(twist_y-PI/4.);
    float c = -a;
    
    vec3 color0 = rainbow(0.1*iTime-0.1*uv.y, 0.6, 1.0);
    
    vec3 color1 = color0*pow(abs(a-b),1.2);
    vec3 color2 = color0*pow(abs(b-c),1.2);
    
    float s1 = step(uv.x, 0.5+twist_x+twist_width*a)*(1.-step(uv.x, 0.5+twist_x+twist_width*b));
    float s2 = step(uv.x, 0.5+twist_x+twist_width*b)*(1.-step(uv.x, 0.5+twist_x+twist_width*c));
    float s3 = 1.0-s1-s2;
    return color1*s1 + color2*s2 + 0.8*color0*s3;
}


const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

float sdSphere(vec3 p, float radius) {
    return length(p)-radius;
}

float opSmoothUnion( float d1, float d2, float k )
{
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

mat3 rotateX(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(1, 0, 0),
        vec3(0, c, -s),
        vec3(0, s, c)
    );
}
mat3 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, 0, s),
        vec3(0, 1, 0),
        vec3(-s, 0, c)
    );
}
mat3 rotateZ(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat3(
        vec3(c, -s, 0),
        vec3(s, c, 0),
        vec3(0, 0, 1)
    );
}
mat2 rotate2(float theta) {
    float c = cos(theta);
    float s = sin(theta);
    return mat2(
        vec2(c, -s),
        vec2(s, c)
    );
}

float sdBox( vec3 p, vec3 b ) {
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdPlane( vec3 p, vec3 n, float h )
{
  // n must be normalized
  return dot(p,n) + h;
}

float terrain(vec2 point) {
    point = point*1.4;
    float t = 
        0.2*sin(point.x*1.6854)
        +0.3232*sin(point.x*1.9899)
        +0.1423*sin(point.x*1.1595+point.y*2.4)
        +0.431*sin(point.y*2.2443);
    return 0.5*t;
}

float sceneSDF(vec3 point) {
    float iTime = in_effect_params.x;
    int effect = int(in_effect_params.y);
    float s = 0;
    switch (effect) {
        case 7:
            s = opSmoothUnion(
                sdSphere(point+vec3(0.55*cos(iTime*7.7),0.356*sin(iTime*5.2),0.43*sin(iTime*4.45+1.)), 0.7),
                sdSphere(point+vec3(-0.45*cos(iTime*4.3),0.64*sin(iTime*-3.656),-0.25*sin(iTime*4.45-1.)), 0.7),
                0.3);
            s = opSmoothUnion(s, sdSphere(point+vec3(0.0,0.5*sin(iTime*10.),0.0),0.7), 0.3);
            return s;
        case 9:
            point = point*rotateY(iTime);
            s = opSmoothUnion(
                sdBox((point+vec3(2.*sin(iTime),0.0,0.0))*rotateZ(cos(iTime))*rotateY(PI/4.-iTime), vec3(0.5,0.5,0.5)),
                sdBox((point+vec3(-2.*sin(iTime),0.0,0.0))*rotateZ(cos(iTime))*rotateY(iTime), vec3(0.5,0.5,0.5)),
                0.5);
            return s;
        case 10:
            point = point+vec3(0.0,terrain(point.xy),0.0);
            s = sdPlane(point, vec3(0.0,1.,0.6), -1.0);
            return s;
    }
}

float shortestDistanceToSurface(vec3 eye, vec3 marchingDirection, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth * marchingDirection);
        if (dist < EPSILON) {
			return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }
    return end;
}

vec3 rayDirection(float fieldOfView, vec2 size, vec2 uv) {
    vec2 xy = uv - size / 2.0;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}
       

vec3 metaball(vec2 uv) {
    uv = uv*vec2(1.0,-1.0)+vec2(0.0,1.0);
    vec3 dir = rayDirection(45.0, vec2(1.0,1.0), uv);
    vec3 eye = vec3(0.0, 0.0, 10.);
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    
    if (dist > MAX_DIST - EPSILON) {
        return rainbow(in_effect_params.x*0.05+0.4, 0.9, 0.9);
    }
    vec3 point = eye + dist * dir;
    vec3 normal = estimateNormal(point);
    float light = dot((dir+vec3(0.0,-0.6,0.2))*-1.,normal)*0.3+0.7;
    switch (int(in_effect_params.y)) {
        case 7: case 9: return (0.9*light+0.7*pow(light, 20.))*rainbow(in_effect_params.x*0.05+0.4, 0.9, 1.0);
        case 10: return (0.22*point.z+1.)*texture(tex_sampler, (0.24*(point.xy*rotate2(-0.5)+vec2(in_effect_params.x, 1.5))*vec2(0.5,-1.0))).xyz;
    }
}

vec3 color_texture(vec2 uv) {
    return texture(tex_sampler, uv).xyz*rainbow(in_effect_params.x, 0.15, 1.0);
}

void main() {
    vec2 uv = vec2(1.0-in_texcoord.y, in_texcoord.x);
    int effect = int(in_effect_params.y);
    vec3 color;
    switch (effect) {
        case 0:
            color = bunny_intro(uv);
            break;
        case 1:
            color = vec3(0.0, 0.0, 0.0);
            break;
        case 2:
            color = noise(uv);
            break;
        case 3:
            color = plain(uv);
            break;
        case 4:
            color = plain2(uv);
            break;
        case 5:
            color = homebrew(uv);
            break;
        case 6:
            color = rainbow_twister(uv);
            break;
        case 7: case 9: case 10:
            color = metaball(uv);
            break;
        case 8:
            color = color_texture(uv);
            break;
        default: break;
    }
    out_color = vec4(color*in_effect_params.z, 1.0);
}