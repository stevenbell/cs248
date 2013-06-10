precision mediump float;

uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform vec4 ambientColor;
uniform vec4 diffuseColor;

in vec4 vPos;
in vec4 vNormal;
in vec2 vTexCoord;

uniform sampler2D sampler;

void main()
{
  vec4 texColor = texture2D(sampler, vTexCoord);
  
  vec3 lightIncidence = normalize(lightPosition - vPos.xyz);
  vec4 lightColor = ambientColor + diffuseColor * max(0.0, dot(lightIncidence, vNormal.xyz));
  
  gl_FragColor = lightColor * texColor;
}
