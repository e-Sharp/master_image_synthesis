#version 330

#ifdef VERTEX_SHADER
// in int gl_VertexID           // (entree) indice du sommet
// out vec4 gl_Position;        // (resultat obligatoire) coordonnees (transformees) du ieme sommet de l'objet

uniform mat4 mvpMatrix;

layout(location= 0) in vec3 position;

void main( )
{
    gl_Position = mvpMatrix * vec4(position, 1);    
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef GEOMETRY_SHADER

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 center;
out vec3 pos;

void main( ) {
    center = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position).xyz / 3.f;

    pos = gl_in[0].gl_Position.xyz;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    pos = gl_in[1].gl_Position.xyz;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    pos = gl_in[2].gl_Position.xyz;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}

#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef FRAGMENT_SHADER
// fragment shader
// in int gl_PrimitiveID;	// (entree) indice du triangle
// in vec4 gl_FragCoord;	// (entree) coordonnees du fragment dans l'espace image
// in bool gl_FrontFacing;	// (entree) orientation du triangle

uniform vec2 viewport;

in vec3 center;
in vec3 pos;

out vec4 fragment_color;	// (resultat obligatoire) couleur du fragment

void main() {
    vec2 cs = (2.f * gl_FragCoord.xy - viewport)  / viewport.y;

    fragment_color= vec4(
        vec3(
            1. - smoothstep(0., 0.04, abs(length(pos - center) - 0.05))
        ), 
        1);
}
#endif
