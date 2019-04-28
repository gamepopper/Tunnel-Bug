#version 130

#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D from, to;
uniform float progress;
uniform vec2 resolution;

vec4 getFromColor(vec2 UV)
{
	return texture2D(from, vec2(UV.x, 1 - UV.y));
}

vec4 getToColor(vec2 UV)
{
	return texture2D(to, UV);
}

// Author: Eke Péter <peterekepeter@gmail.com>
// License: MIT
vec4 transition(vec2 p) {
  float x = progress;
  x=smoothstep(.0,1.0,(x*2.0+p.x-1.0));
  return mix(getFromColor((p-.5)*(1.-x)+.5), getToColor((p-.5)*x+.5), x);
}

void main()
{
	gl_FragColor = transition(gl_TexCoord[0].xy / resolution.xy) * gl_Color;
}