@group(0) @binding(0)
var<uniform> u : vec4<u32>;

@compute @workgroup_size(1)
fn main() {
  let x = u;
}
