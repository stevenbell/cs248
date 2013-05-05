precision mediump float;

uniform vec4 lightPosition;
uniform vec4 viewPosition;

uniform vec4 ambientColor;
uniform vec4 diffuseColor;

in vec4 vPos;
in vec4 vNormal;

void main()
{
  vec3 lightIncidence = normalize(lightPosition.xyz - vPos.xyz);
  gl_FragColor = ambientColor +
                 diffuseColor * max(0.0, dot(lightIncidence, normalize(vNormal.xyz)));
}
