#version 310 es

shared uint arg_0;
void tint_zero_workgroup_memory(uint local_idx) {
  if ((local_idx < 1u)) {
    atomicExchange(arg_0, 0u);
  }
  barrier();
}

layout(binding = 0, std430) buffer prevent_dce_block_ssbo {
  uint inner;
} prevent_dce;

void atomicSub_0d26c2() {
  uint res = atomicAdd(arg_0, -(1u));
  prevent_dce.inner = res;
}

void compute_main(uint local_invocation_index) {
  tint_zero_workgroup_memory(local_invocation_index);
  atomicSub_0d26c2();
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  compute_main(gl_LocalInvocationIndex);
  return;
}
