in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
 
varying out vec4 vPos;
varying out vec4 vNormal;
varying out vec2 vTexCoord;
 
void main()
{
  vPos = modelViewMatrix * vec4(vertexPosition, 1.0);
  vNormal = modelViewMatrix * vec4(vertexNormal, 1.0);
  //vTexCoord = vec2((vertexPosition.x + 1.0) / 2.0, (vertexPosition.y + 1.0) / 2.0);
  vTexCoord = vertexTexCoord;
  gl_Position = projectionMatrix * vPos;
}
