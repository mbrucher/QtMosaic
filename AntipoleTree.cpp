/**
 * \file AntipoleTree.cpp
 */

#include <cmath>
#include <map>
#include <stdexcept>

#include "AntipoleTree.h"

AntipoleNode::AntipoleNode(const AntipoleTree* tree)
  :tree(tree)
{
}

AntipoleNode::~AntipoleNode()
{
}

AntipoleInternalNode::AntipoleInternalNode(const AntipoleTree* tree)
  :AntipoleNode(tree)
{
}

AntipoleInternalNode::~AntipoleInternalNode()
{
  delete right;
  delete left;
}

bool AntipoleInternalNode::isLeaf() const
{
  return false;
}

std::pair<long, float> AntipoleInternalNode::getClosestThumbnail(const std::vector<float>& image) const
{
  throw;
}

AntipoleLeaf::AntipoleLeaf(const AntipoleTree* tree)
  :AntipoleNode(tree)
{
}

AntipoleLeaf::~AntipoleLeaf()
{
}

bool AntipoleLeaf::isLeaf() const
{
  return true;
}

std::pair<long, float> AntipoleLeaf::getClosestThumbnail(const std::vector<float>& image) const
{
  const std::vector<std::vector<float> >& thumbnails = tree->getThumbnails();
  if(thumbnails.empty())
  {
    throw std::runtime_error("Empty set for clostest thumbnail");
  }
  long closest = 0;
  float mindist = std::numeric_limits<float>::max();

  for(std::vector<std::vector<float> >::const_iterator it = thumbnails.begin(); it != thumbnails.end(); ++it)
  {
    float dist = HelperFunctions::distance2(image, *it);
    if(dist < mindist)
    {
      mindist =dist;
      closest = it - thumbnails.begin();
    }
  }
  return std::make_pair(closest, mindist); // add the local index to global index conversion
}

AntipoleTree::AntipoleTree(void)
  :root(NULL)
{
}

AntipoleTree::~AntipoleTree()
{
  delete root;
}

void AntipoleTree::build(const std::vector<std::vector<float> >& thumbnails)
{
  this->thumbnails = thumbnails;
  root = new AntipoleLeaf(this);
}

const std::vector<std::vector<float> >& AntipoleTree::getThumbnails() const
{
  return thumbnails;
}

long AntipoleTree::getClosestThumbnail(const std::vector<float>& image) const
{
  if(root)
  {
    return root->getClosestThumbnail(image).first;
  }
  else
  {
    return -1;
  }
}

float HelperFunctions::distance2(const std::vector<float>& object1, const std::vector<float>& object2)
{
  float dist = 0;

  for(std::vector<float>::const_iterator it1 = object1.begin(), it2 = object2.begin(); it1 != object1.end() && it2 != object2.end(); ++it1, ++it2)
  {
    dist += (*it1 - *it2) * (*it1 - *it2);
  }
  return dist;
}

std::vector<float> HelperFunctions::convert(const QImage& image)
{
  std::vector<float> thumbnail;

  for(int j = 0; j < image.height(); ++j)
  {
    for(int i = 0; i < image.width(); ++i)
    {
      QRgb pixel = image.pixel(i, j);
      thumbnail.push_back(qRed(pixel));
      thumbnail.push_back(qBlue(pixel));
      thumbnail.push_back(qGreen(pixel));
    }
  }

  return thumbnail;
}

long HelperFunctions::median1(const std::vector<std::vector<float> >& objects)
{
  if(objects.empty())
  {
    throw std::runtime_error("Empty set for 1-median algorithm");
  }

  std::map<float, long> distances;

  for(std::vector<std::vector<float> >::const_iterator it1 = objects.begin(); it1 != objects.end(); ++it1)
  {
    float dist = 0;
    for(std::vector<std::vector<float> >::const_iterator it2 = objects.begin(); it2 != objects.end(); ++it2)
    {
      dist += std::sqrt(distance2(*it1, *it2));
    }
    distances[dist] = it1 - objects.begin();
  }


  return distances.begin()->second;
}
