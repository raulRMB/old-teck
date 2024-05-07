@group(1) @binding(0) var arg_0 : texture_storage_1d<rg32float, read>;

fn textureLoad_d81c57() {
  var res : vec4<f32> = textureLoad(arg_0, 1i);
  prevent_dce = res;
}

@group(2) @binding(0) var<storage, read_write> prevent_dce : vec4<f32>;

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  textureLoad_d81c57();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  textureLoad_d81c57();
}

@compute @workgroup_size(1)
fn compute_main() {
  textureLoad_d81c57();
}
