fn select_431dfb() {
  const arg_0 = vec2(1);
  const arg_1 = vec2(1);
  var arg_2 = vec2<bool>(true);
  var res = select(arg_0, arg_1, arg_2);
}

@vertex
fn vertex_main() -> @builtin(position) vec4<f32> {
  select_431dfb();
  return vec4<f32>();
}

@fragment
fn fragment_main() {
  select_431dfb();
}

@compute @workgroup_size(1)
fn compute_main() {
  select_431dfb();
}
