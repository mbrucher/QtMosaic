/**
 * \file AntipoleTree.cpp
 */

#include <cmath>
#include <map>
#include <stdexcept>

#include "AntipoleTree.h"

const int HelperFunctions::tournament_size = 3;

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
  MatchingThumbnails left_matching;
  MatchingThumbnails right_matching;
  std::vector<float> left_center;
  std::vector<float> right_center;

  int status = divideMatching(minimum_size, old_matching, left_center, right_center, left_matching, right_matching);
  if(status == 1)
  {
    AntipoleInternalNode* node = new AntipoleInternalNode(this);
    AntipoleNode* left = buildNewNode(minimum_size, left_matching);
    node->setLeft(left);
    computeCenter(left_center, left_matching);
    left->setCenter(left_center);
    left->setRadius(computeMaxRadius(left_center, left_matching));
    AntipoleNode* right = buildNewNode(minimum_size, right_matching);
    node->setRight(right);
    computeCenter(right_center, right_matching);
    right->setCenter(right_center);
    right->setRadius(computeMaxRadius(right_center, right_matching));
  }
//  if (old_matching.size() <= tournament_size)
  {
    AntipoleLeaf* leaf = new AntipoleLeaf(this);
    leaf->setMatching(old_matching);
    return leaf;
  }
  return NULL;
}

int AntipoleTree::divideMatching(float minimum_size, const MatchingThumbnails& old_matching, std::vector<float>& left_center, std::vector<float>& right_center, MatchingThumbnails& left_matching, MatchingThumbnails& right_matching)
{
  std::pair<std::vector<float>, std::vector<float> > pair = HelperFunctions::approxAntipole(thumbnails, old_matching);
  if(HelperFunctions::distance2(pair.first, pair.second) < minimum_size)
  {
    return 0;
  }
  left_center = pair.first;
  right_center = pair.second;
  assignMatching(old_matching, left_center, right_center, left_matching, right_matching);
}

void AntipoleTree::assignMatching(const MatchingThumbnails& old_matching, std::vector<float>& left_center, std::vector<float>& right_center, MatchingThumbnails& left_matching, MatchingThumbnails& right_matching)
{
  left_matching.clear();
  right_matching.clear();
  for(MatchingThumbnails::const_iterator it = old_matching.begin(); it != old_matching.end(); ++it)
  {
    if(HelperFunctions::distance2(thumbnails[*it], left_center) > HelperFunctions::distance2(thumbnails[*it], right_center))
    {
      right_matching.insert(*it);
    }
    else
    {
      left_matching.insert(*it);
    }
  }
}

float AntipoleTree::computeMaxRadius(const std::vector<float>& center, const MatchingThumbnails& matching) const
{
  float radius = 0;
  for(MatchingThumbnails::const_iterator it = matching.begin(); it != matching.end(); ++it)
  {
    float new_radius = HelperFunctions::distance2(center, thumbnails[*it]);
    radius = std::max(radius, new_radius);
  }
  return radius;
}

void AntipoleTree::computeCenter(std::vector<float>& center, const MatchingThumbnails& matching) const
{
  center = std::vector<float>(center.size(), 0);
  for(MatchingThumbnails::const_iterator it = matching.begin(); it != matching.end(); ++it)
  {
    std::vector<float>::const_iterator itbis = thumbnails[*it].begin();
    for(std::vector<float>::iterator it2 = center.begin(); it2 != center.end(); ++it2)
    {
      (*it2) += (*itbis);
      ++itbis;
    }
  }
  for(std::vector<float>::iterator it = center.begin(); it != center.end(); ++it)
  {
    *it /= matching.size();
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

std::pair<std::vector<float>, std::vector<float> > HelperFunctions::approxAntipole(const std::vector<std::vector<float> >& objects, const MatchingThumbnails& matching)
{
  std::vector<std::vector<float> > copied_objects;
  for(MatchingThumbnails::const_iterator it = matching.begin(); it != matching.end(); ++it)
  {
    copied_objects.push_back(objects[*it]);
  }
  std::vector<std::vector<float> >::iterator it = copied_objects.begin();

  while(copied_objects.size() > 2)
  {
    std::vector<std::vector<float> > new_tournament;
    for(int i = 0; i < tournament_size && i < copied_objects.size(); ++i)
    {
      new_tournament.push_back(*it);
      ++it;
      if(it == copied_objects.end())
      {
        it = copied_objects.begin();
      }
    }
    long result = median1(new_tournament);
    long distance = it - copied_objects.begin();
    copied_objects.erase(copied_objects.begin() + result);
    if(result > distance)
    {
      it = copied_objects.begin() + distance - 1;
    }
    else if(result == distance)
    {
      if(distance == copied_objects.size())
      {
        it = copied_objects.begin();
      }
      else
      {
        it = copied_objects.begin() + distance;
      }
    }
  }
  return std::make_pair(*copied_objects.begin(), *(copied_objects.begin()+1));
}
