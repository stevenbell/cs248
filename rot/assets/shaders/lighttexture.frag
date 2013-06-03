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
  //vec4 bumpNormal = texture2D(sampler, vTexCoord) * vec4(1.0, 0.4, 0.2, 1.0);
  vec4 bumpNormal = vNormal;
  
  vec3 lightIncidence = normalize(lightPosition - vPos.xyz);
  vec4 lightColor = ambientColor +
                 diffuseColor * max(0.0, dot(lightIncidence, normalize(bumpNormal.xyz)));
  //gl_FragColor = texture2D(sampler, vTexCoord) * lightColor;
  gl_FragColor = lightColor;
}
