@group(1) @binding(0) var arg_0 : texture_storage_2d_array<rg32sint, write>;

fn textureStore_7bb211() {
  textureStore(arg_0, vec2<i32>(1i), 1u, vec4<i32>(1i));
}

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  textureStore_7bb211();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  textureStore_7bb211();
}

@compute @workgroup_size(1)
fn compute_main() {
  textureStore_7bb211();
}
