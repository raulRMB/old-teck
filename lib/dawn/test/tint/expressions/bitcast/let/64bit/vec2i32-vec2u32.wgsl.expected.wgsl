@compute @workgroup_size(1)
fn f() {
  let a : vec2<i32> = vec2<i32>(1073757184i, -(1006616064i));
  let b : vec2<u32> = bitcast<vec2<u32>>(a);
}
