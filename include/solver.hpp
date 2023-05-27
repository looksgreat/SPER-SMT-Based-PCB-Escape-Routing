#pragma once
#include "z3++.h"
using namespace z3;
class SOLVER{
public:
  SOLVER()
    :_solver{_context},_model{nullptr}{}
  ~SOLVER(){
    if(_model){delete _model;}
  }
  void add(expr& cstr);
  void add(expr&& cstr);
  expr bool_val(bool);
  expr bool_const(const std::string&);
  expr int_const(const std::string&);
  expr int_val(int);
  expr bv_val(size_t, size_t);
  expr bv_const(const std::string&, size_t);
  expr_vector getVector();
  bool minimize(expr);
  bool check();
  model& get_model();
  int eval_num(expr);
  bool eval_bool(expr);
  expr eval(expr);
  expr get_expr();
  sort get_int_sort();
  sort get_bool_sort();
  void push();
  void pop();
  
private:
  context _context;
  optimize _solver;
  model* _model;
};
