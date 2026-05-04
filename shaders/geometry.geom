#version 330 core

layout(lines) in;
layout (triangle_strip, max_vertices = 4) out;

in float vWidth[];

void main(){

    float width = vWidth[0];
    float halfWidth = width / 2.0;

    vec3 pos1 = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
    vec3 pos2 = gl_in[1].gl_Position.xyz / gl_in[1].gl_Position.w;
    vec2 direction = normalize(pos1.xy - pos2.xy);
    vec2 orthognal = vec2(-direction.y, direction.x);

    vec2 offset = orthognal * halfWidth;

    gl_Position = vec4((pos1.xy - offset) * gl_in[0].gl_Position.w, gl_Position.z * gl_in[0].gl_Position.w, gl_in[0].gl_Position.w);
    EmitVertex();

    gl_Position = vec4((pos1.xy + offset) * gl_in[0].gl_Position.w, pos1.z * gl_in[0].gl_Position.w, gl_in[0].gl_Position.w);
    EmitVertex();

    gl_Position = vec4((pos2.xy - offset) * gl_in[1].gl_Position.w, pos2.z * gl_in[1].gl_Position.w, gl_in[1].gl_Position.w);
    EmitVertex();

    gl_Position = vec4((pos2.xy + offset) * gl_in[1].gl_Position.w, pos2.z * gl_in[1].gl_Position.w, gl_in[1].gl_Position.w);
    EmitVertex();

    EndPrimitive();
}

