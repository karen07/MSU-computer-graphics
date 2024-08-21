#version 430
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 Projection;
uniform mat4 Veiw;
uniform mat4 Model;
void main()
{
    Normal = vec3(normalize( Veiw * Model * vec4(VertexNormal, 0.0)));
    Position = vec3(Veiw * Model * vec4(VertexPosition,1.0));
    gl_Position = Projection * vec4(Position,1.0);
}