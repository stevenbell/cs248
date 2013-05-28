in vec4 vertexPosition;
in vec2 vertexTexCoord;

varying out vec2 vTexCoord;

void main()
{
  gl_Position = vertexPosition;
  vTexCoord = vertexTexCoord;
}