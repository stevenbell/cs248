precision mediump float;

uniform vec4 lightPosition;
uniform vec4 viewPosition;

uniform vec4 ambientColor;
uniform vec4 diffuseColor;

in vec4 vPos;
in vec4 vNormal;
in vec2 vTexCoord;

uniform sampler2D sampler;

void main()
{
  gl_FragColor = texture2D(sampler, vTexCoord);// + vec4(0.0, 0.3, 0.0, 1.0);
}
