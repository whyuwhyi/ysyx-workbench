extern bool is_sim_end;

extern "C" void ebreak() { is_sim_end = true; }
