fn bitcast_7ffa9c() {
  var res : vec4<u32> = bitcast<vec4<u32>>(vec4<u32>(1u));
  prevent_dce = res;
}

@group(2) @binding(0) var<storage, read_write> prevent_dce : vec4<u32>;

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  bitcast_7ffa9c();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  bitcast_7ffa9c();
}

@compute @workgroup_size(1)
fn compute_main() {
  bitcast_7ffa9c();
}
