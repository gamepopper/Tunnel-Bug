#version 130

uniform sampler2D 	texture;
uniform vec2 		offsetFactor;
uniform float		intensity = 1.0;

void main()
{
	vec2 textureCoordinates = gl_TexCoord[0].xy;
	vec4 color = texture2D(texture, textureCoordinates);

	if (intensity > 0.0)
	{
		vec2 offset = offsetFactor * intensity;

		color = vec4(0.0);
		color += texture2D(texture, textureCoordinates - 4.0 * offset) * 0.0162162162;
		color += texture2D(texture, textureCoordinates - 3.0 * offset) * 0.0540540541;
		color += texture2D(texture, textureCoordinates - 2.0 * offset) * 0.1216216216;
		color += texture2D(texture, textureCoordinates - offset)	   * 0.1945945946;
		color += texture2D(texture, textureCoordinates) * 0.2270270270;
		color += texture2D(texture, textureCoordinates + offset)	   * 0.1945945946;
		color += texture2D(texture, textureCoordinates + 2.0 * offset) * 0.1216216216;
		color += texture2D(texture, textureCoordinates + 3.0 * offset) * 0.0540540541;
		color += texture2D(texture, textureCoordinates + 4.0 * offset) * 0.0162162162;
	}

	gl_FragColor = color * gl_Color;
}