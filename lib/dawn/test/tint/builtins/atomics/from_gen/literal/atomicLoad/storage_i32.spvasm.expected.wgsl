struct SB_RW_atomic {
  /* @offset(0) */
  arg_0 : atomic<i32>,
}

struct SB_RW {
  /* @offset(0) */
  arg_0 : i32,
}

@group(0) @binding(0) var<storage, read_write> sb_rw : SB_RW_atomic;

fn atomicLoad_0806ad() {
  var res = 0i;
  let x_9 = atomicLoad(&(sb_rw.arg_0));
  res = x_9;
  return;
}

fn fragment_main_1() {
  atomicLoad_0806ad();
  return;
}

@fragment
fn fragment_main() {
  fragment_main_1();
}

fn compute_main_1() {
  atomicLoad_0806ad();
  return;
}

@compute @workgroup_size(1i, 1i, 1i)
fn compute_main() {
  compute_main_1();
}
