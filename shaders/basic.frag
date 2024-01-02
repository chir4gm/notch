#version 400
out vec4 frag_colour;
in vec2 tex_coord;
uniform sampler2D blockTexture;
void main() {
  vec4 tex_color = texture(blockTexture, tex_coord);
   if (tex_color.xyz == vec3(0.0f,0.0f,0.0f))
	discard;
  frag_colour = tex_color;
}