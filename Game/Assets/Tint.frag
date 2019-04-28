#version 130
uniform sampler2D texture;
uniform vec4 tint;

void main()
{
	vec4 textureFragment = texture2D(texture, gl_TexCoord[0].xy);
	textureFragment.r *= tint.r;
	textureFragment.g *= tint.g;
	textureFragment.b *= tint.b;
	gl_FragColor = textureFragment;
}