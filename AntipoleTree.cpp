/**
 * \file AntipoleTree.cpp
 */

#include <cmath>
#include <map>
#include <stdexcept>

#include "AntipoleTree.h"

const int AntipoleTree::tournament_size = 3;

AntipoleNode::AntipoleNode(const AntipoleTree* tree)
  :tree(tree)
{
}

AntipoleNode::~AntipoleNode()
{
}

void AntipoleNode::setCenter(const std::vector<float>& center)
{
  this->center = center;
}

void AntipoleNode::setRadius(float radius)
{
  this->radius = radius;
}

float AntipoleNode::minimumDistance(const std::vector<float>& image) const
{
  return HelperFunctions::distance2(image, center) - radius;
}

AntipoleInternalNode::AntipoleInternalNode(const AntipoleTree* tree)
  :AntipoleNode(tree), right(NULL), left(NULL)
{
}

AntipoleInternalNode::~AntipoleInternalNode()
{
  delete right;
  delete left;
}

void AntipoleInternalNode::setLeft(AntipoleNode* left)
{
  this->left = left;
}

void AntipoleInternalNode::setRight(AntipoleNode* right)
{
  this->right = right;
}

bool AntipoleInternalNode::isLeaf() const
{
  return false;
}

std::pair<long, float> AntipoleInternalNode::getBestSubnode(const AntipoleNode* node, const std::vector<float>& image, const std::pair<long, float>& current_best) const
{
  if(node->minimumDistance(image) < current_best.second)
  {
    std::pair<long, float> best_node = left->getClosestThumbnail(image, current_best.second);
    if (best_node.second < current_best.second)
    {
      return best_node;
    }
  }
  return current_best;
}

std::pair<long, float> AntipoleInternalNode::getClosestThumbnail(const std::vector<float>& image, float max_dist) const
{
  std::pair<long, float> best = std::make_pair(-1, max_dist);

  if(left)
  {
    best = getBestSubnode(left, image, best);
  }
  if(right)
  {
    best = getBestSubnode(right, image, best);
  }
  return best;
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

std::pair<long, float> AntipoleLeaf::getClosestThumbnail(const std::vector<float>& image, float max_dist) const
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

  MatchingThumbnails::const_iterator it = matching_thumbnails.find(closest);
  if(it == matching_thumbnails.end())
  {
    throw std::runtime_error("Matching map does not have the needed thumbnail");
  }
  return std::make_pair(*it, mindist);
}

void AntipoleLeaf::setMatching(const MatchingThumbnails& matching_thumbnails)
{
  this->matching_thumbnails = matching_thumbnails;
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
  delete root;

  MatchingThumbnails default_matching;
  for(int i = 0; i < thumbnails.size(); ++i)
  {
    default_matching.insert(i);
  }

  root = buildNewNode(100, default_matching);

  //root = new AntipoleLeaf(this);
}

const std::vector<std::vector<float> >& AntipoleTree::getThumbnails() const
{
  return thumbnails;
}

long AntipoleTree::getClosestThumbnail(const std::vector<float>& image) const
{
  if(root)
  {
    return root->getClosestThumbnail(image, std::numeric_limits<float>::max()).first;
  }
  else
  {
    return -1;
  }
}

long AntipoleTree::getClosestThumbnail(const QImage& image) const
{
  return getClosestThumbnail(HelperFunctions::convert(image));
}

AntipoleNode* AntipoleTree::buildNewNode(float minimum_size, const MatchingThumbnails& old_matching)
{
//  if (old_matching.size() <= tournament_size)
  {
    AntipoleLeaf* leaf = new AntipoleLeaf(this);
    leaf->setMatching(old_matching);
    return leaf;
  }
  return NULL;
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
