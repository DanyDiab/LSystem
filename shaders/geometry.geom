#version 330 core

layout(lines) in;
layout (line_strip, max_vertices = 2) out;

in float vWidth[];

void main(){
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    EndPrimitive();
}


// 2 vertices
// get direction vector
// calculate orthognal vector

// take vertex 1, step -negative width emit vertex,
// step positive width and emit vertex.

// take vertex 2 repeat step.
