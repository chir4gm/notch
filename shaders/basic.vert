#version 400
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_pos;
uniform mat4 model;
uniform uint tex_idx;
uniform mat4 view;
uniform mat4 projection;
out vec2 tex_coord;
void main() {
  gl_Position = projection * view * model * vec4(pos, 1.0);
  tex_coord = vec2(((tex_idx % 16) + tex_pos.x)/16.0f, ((tex_idx / 16) + tex_pos.y)/16.0f);
}