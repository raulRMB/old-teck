#version 310 es

void tint_symbol() {
  uint tint_symbol_1 = 0u;
  bool tint_continue = false;
  while (true) {
    tint_continue = false;
    switch(2) {
      case 1: {
        tint_continue = true;
        break;
      }
      default: {
        break;
      }
    }
    if (tint_continue) {
      {
        if (true) { break; }
      }
      continue;
    }
    tint_symbol_1 = (tint_symbol_1 + 1u);
    {
      if (true) { break; }
    }
  }
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
  tint_symbol();
  return;
}
