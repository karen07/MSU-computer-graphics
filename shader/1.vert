#version 430
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 LightDir;
out vec2 TexCoord;
out vec3 ViewDir;
out vec3 Position;

uniform vec4 LightPosition;
uniform mat4 Projection;
uniform mat4 Veiw;
uniform mat4 Model;
void main()
{
    TexCoord = VertexTexCoord;
    vec3 Normal = vec3(normalize( Veiw * Model * vec4(VertexNormal, 0.0)));
	vec3 tang =vec3(normalize( Veiw * Model * vec4(1.0, 0.0, 0.0, 0.0)));
	vec3 binormal = normalize(cross( Normal, tang ));
    mat3 toObjectLocal = mat3(
        tang.x, binormal.x, Normal.x,
        tang.y, binormal.y, Normal.y,
        tang.z, binormal.z, Normal.z ) ;
    vec3 pos = vec3( Veiw * Model * vec4(VertexPosition,1.0) );
	Position = pos;
    LightDir = normalize( toObjectLocal * (LightPosition.xyz - pos) );

    ViewDir = toObjectLocal * normalize(-pos);

    TexCoord = VertexTexCoord;
    gl_Position = Projection * Veiw * Model * vec4(VertexPosition,1.0);
}