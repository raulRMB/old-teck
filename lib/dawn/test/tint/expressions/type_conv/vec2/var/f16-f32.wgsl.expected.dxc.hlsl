[numthreads(1, 1, 1)]
void unused_entry_point() {
  return;
}

static vector<float16_t, 2> u = (float16_t(1.0h)).xx;

void f() {
  const float2 v = float2(u);
}
