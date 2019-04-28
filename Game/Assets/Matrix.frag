#version 130
uniform sampler2D texture;
uniform vec2 resolution;
uniform float alpha;
uniform float step;

void main()
{
	vec4 textureFragment = texture2D(texture, gl_TexCoord[0].xy);
	
	if (floor(mod(gl_TexCoord[0].x * resolution.x, step)) == 0)
		textureFragment *= alpha;
	if (floor(mod(gl_TexCoord[0].y * resolution.y, step)) == 0)
		textureFragment *= alpha;
		
	gl_FragColor = textureFragment;
}