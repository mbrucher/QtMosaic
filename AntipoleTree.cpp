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

std::pair<long, float> AntipoleInternalNode::getClosestThumbnail(const std::vector<float>& image, const std::vector<std::vector<float> >& thumbnails)
{
  throw;
}

AntipoleLeaf::~AntipoleLeaf()
{
}

bool AntipoleLeaf::isLeaf()
{
  return true;
}

std::pair<long, float> AntipoleLeaf::getClosestThumbnail(const std::vector<float>& image, const std::vector<std::vector<float> >& thumbnails)
{
  throw;
}

AntipoleTree::AntipoleTree(void)
  :root(NULL)
{
}

long AntipoleTree::getClosestThumbnail(const std::vector<float>& image)
{
  if(root)
  {
    return root->getClosestThumbnail(image, thumbnails).first;
  }
  else
  {
    return -1;
  }
}
