#include <ossia/editor/expression/expression_not.hpp>
#include <ossia/editor/expression/expression.hpp>

namespace ossia
{
namespace expressions
{
expression_not::expression_not(expression_ptr p):
  mExpression{std::move(p)}
{

}

expression_not::~expression_not()
{
  // TODO REMOVE ALL CALLBACKS
}

bool expression_not::evaluate() const
{
  return !expressions::evaluate(*mExpression);
}

void expression_not::update() const
{
  expressions::update(*mExpression);
}

expression_base& expression_not::getExpression() const
{
  return *mExpression;
}

void expression_not::onFirstCallbackAdded()
{
  mResultCallbackIndex =
      expressions::add_callback(
        *mExpression,
        [&] (bool result) { resultCallback(result); });
}

void expression_not::onRemovingLastCallback()
{
  expressions::remove_callback(*mExpression, mResultCallbackIndex);
}

void expression_not::resultCallback(bool result)
{
  send(!result);
}
}
}
