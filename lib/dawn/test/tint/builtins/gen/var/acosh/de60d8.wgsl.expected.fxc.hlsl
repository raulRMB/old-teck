SKIP: FAILED

vector<float16_t, 4> tint_acosh(vector<float16_t, 4> x) {
  return log((x + sqrt(((x * x) - float16_t(1.0h)))));
}

RWByteAddressBuffer prevent_dce : register(u0, space2);

void acosh_de60d8() {
  vector<float16_t, 4> arg_0 = (float16_t(1.54296875h)).xxxx;
  vector<float16_t, 4> res = tint_acosh(arg_0);
  prevent_dce.Store<vector<float16_t, 4> >(0u, res);
}

struct tint_symbol {
  float4 value : SV_Position;
};

float4 vertex_main_inner() {
  acosh_de60d8();
  return (0.0f).xxxx;
}

tint_symbol vertex_main() {
  const float4 inner_result = vertex_main_inner();
  tint_symbol wrapper_result = (tint_symbol)0;
  wrapper_result.value = inner_result;
  return wrapper_result;
}

void fragment_main() {
  acosh_de60d8();
  return;
}

[numthreads(1, 1, 1)]
void compute_main() {
  acosh_de60d8();
  return;
}
