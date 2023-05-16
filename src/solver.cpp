#include "../include/solver.hpp"
using namespace z3;
void SOLVER::add(expr&& cstr){
  _solver.add(cstr);
}

void SOLVER::add(expr& cstr){
  _solver.add(cstr);
}

auto SOLVER::bool_val(bool val) -> expr
{
  return _context.bool_val(val);
}

auto SOLVER::bool_const(const std::string&name) -> expr
{
  return _context.bool_const(name.c_str());
}

auto SOLVER::int_const(const std::string&name) -> expr
{
  return _context.int_const(name.c_str());
}

auto SOLVER::int_val(int val) -> expr
{
  return _context.int_val(val);
}

auto SOLVER::bv_val(size_t val, size_t size) -> expr
{
  return _context.bv_val(val, size);
}

auto SOLVER::bv_const(const std::string&name, size_t size) -> expr
{
  return _context.bv_const(name.c_str(), size);
}

auto SOLVER::getVector() -> expr_vector 
{
  return expr_vector(_context);
}

auto SOLVER::get_model() -> model&
{
  assert(_model!=nullptr && "query for unsat model");
  return  *_model;
}

auto SOLVER::eval(expr var) -> expr
{
  return get_model().eval(var);
}

auto SOLVER::eval_num(expr var) -> int
{
  return eval(var).get_numeral_int64();
}

auto SOLVER::eval_bool(expr var) -> bool
{
  return eq(eval(var), bool_val(true));
}

bool SOLVER::minimize(expr objective){
  optimize::handle h1 = _solver.minimize(objective);
  return check();
}

bool SOLVER::check(){
  if(_solver.check()==z3::sat){
    if(_model){
      delete _model;
    }
    _model = new model(_solver.get_model());
    return true;
  }
  else
    return false;
}

expr SOLVER::get_expr(){
  expr ex(_context);
  return ex;
}

auto SOLVER::get_int_sort() {
  return _context.int_sort();
}

auto SOLVER::get_bool_sort() {
  return _context.bool_sort();
}

