SKIP: FAILED

int tint_dot4I8Packed(uint param_0, uint param_1) {
  int accumulator = 0;
  return dot4add_i8packed(param_0, param_1, accumulator);
}

RWByteAddressBuffer prevent_dce : register(u0, space2);

void dot4I8Packed_881e62() {
  uint arg_0 = 1u;
  uint arg_1 = 1u;
  int res = tint_dot4I8Packed(arg_0, arg_1);
  prevent_dce.Store(0u, asuint(res));
}

struct tint_symbol {
  float4 value : SV_Position;
};

float4 vertex_main_inner() {
  dot4I8Packed_881e62();
  return (0.0f).xxxx;
}

tint_symbol vertex_main() {
  const float4 inner_result = vertex_main_inner();
  tint_symbol wrapper_result = (tint_symbol)0;
  wrapper_result.value = inner_result;
  return wrapper_result;
}

void fragment_main() {
  dot4I8Packed_881e62();
  return;
}

[numthreads(1, 1, 1)]
void compute_main() {
  dot4I8Packed_881e62();
  return;
}
DXC validation failure:
warning: DXIL.dll not found.  Resulting DXIL will not be signed for use in release environments.

error: validation errors
shader.hlsl:3:10: error: Opcode Dot4AddI8Packed not valid in shader model vs_6_0.
note: at '%2 = call i32 @dx.op.dot4AddPacked.i32(i32 163, i32 0, i32 1, i32 1)' in block '#0' of function 'vertex_main'.
Validation failed.



warning: DXIL.dll not found.  Resulting DXIL will not be signed for use in release environments.

error: validation errors
shader.hlsl:3:10: error: Opcode Dot4AddI8Packed not valid in shader model ps_6_0.
note: at '%2 = call i32 @dx.op.dot4AddPacked.i32(i32 163, i32 0, i32 1, i32 1)' in block '#0' of function 'fragment_main'.
Validation failed.



warning: DXIL.dll not found.  Resulting DXIL will not be signed for use in release environments.

error: validation errors
shader.hlsl:3:10: error: Opcode Dot4AddI8Packed not valid in shader model cs_6_0.
note: at '%2 = call i32 @dx.op.dot4AddPacked.i32(i32 163, i32 0, i32 1, i32 1)' in block '#0' of function 'compute_main'.
Validation failed.



