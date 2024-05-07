@group(1) @binding(0) var arg_0 : texture_depth_2d_array;

fn textureLoad_7b63e0() {
  var res : f32 = textureLoad(arg_0, vec2<u32>(1u), 1u, 1u);
  prevent_dce = res;
}

@group(2) @binding(0) var<storage, read_write> prevent_dce : f32;

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  textureLoad_7b63e0();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  textureLoad_7b63e0();
}

@compute @workgroup_size(1)
fn compute_main() {
  textureLoad_7b63e0();
}
