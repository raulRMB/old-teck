[numthreads(1, 1, 1)]
void unused_entry_point() {
  return;
}

static int3 u = (1).xxx;

void f() {
  const float3 v = float3(u);
}
