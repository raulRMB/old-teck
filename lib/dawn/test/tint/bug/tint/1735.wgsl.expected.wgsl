struct S {
  f : f32,
}

@group(0) @binding(0) var<storage, read> in : S;

@group(0) @binding(1) var<storage, read_write> out : S;

@compute @workgroup_size(1)
fn main() {
  out = in;
}
