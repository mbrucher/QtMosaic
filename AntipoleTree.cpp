/**
 * \file AntipoleTree.cpp
 */

#include "AntipoleTree.h"

AntipoleNode::~AntipoleNode()
{
}

bool AntipoleNode::isLeaf()
{
  return false;
}

AntipoleLeaf::~AntipoleLeaf()
{
}

bool AntipoleLeaf::isLeaf()
{
  return true;
}

AntipoleTree::AntipoleTree(void)
{
}

