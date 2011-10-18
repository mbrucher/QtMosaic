/**
 * \file AntipoleTree.cpp
 */

#include "AntipoleTree.h"

AntipoleNode::~AntipoleNode()
{
}

AntipoleInternalNode::~AntipoleInternalNode()
{
  delete right;
  delete left;
}

bool AntipoleInternalNode::isLeaf()
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

