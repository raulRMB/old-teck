SKIP: FAILED


enable chromium_experimental_subgroups;

fn subgroupBroadcast_34fa3d() {
  var res : vec3<u32> = subgroupBroadcast(vec3<u32>(1u), 1u);
  prevent_dce = res;
}

@group(2) @binding(0) var<storage, read_write> prevent_dce : vec3<u32>;

@compute @workgroup_size(1)
fn compute_main() {
  subgroupBroadcast_34fa3d();
}

Failed to generate: <dawn>/test/tint/builtins/gen/literal/subgroupBroadcast/34fa3d.wgsl:38:8 error: GLSL backend does not support extension 'chromium_experimental_subgroups'
enable chromium_experimental_subgroups;
       ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

