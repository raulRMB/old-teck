#version 310 es
#extension GL_AMD_gpu_shader_half_float : require

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void unused_entry_point() {
  return;
}
f16vec3 u = f16vec3(1.0hf);
void f() {
  bvec3 v = bvec3(u);
}

