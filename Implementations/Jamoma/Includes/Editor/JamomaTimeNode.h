/*!
 * \file JamomaTimeNode.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

#include "JamomaTimeConstraint.h"
#include "JamomaTimeEvent.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeNode : public TimeNode, public enable_shared_from_this<JamomaTimeNode>
{

private:

# pragma mark -
# pragma mark Implementation specific
  
public:

# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeNode();
  
  JamomaTimeNode(const JamomaTimeNode * other);
  
  ~JamomaTimeNode();
  
  shared_ptr<TimeNode> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;
  
  shared_ptr<State> state() const override;

# pragma mark -
# pragma mark Accessors
  
  TimeValue getDate() const override;
  
  TimeValue getSimultaneityMargin() const override;
  
  void setSimultaneityMargin(TimeValue) override;
  
# pragma mark -
# pragma mark #TimeEvents
  
  iterator emplace(const_iterator,
                   shared_ptr<Expression> = nullptr) override;
};
