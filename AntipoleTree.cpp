/**
 * \file AntipoleTree.cpp
 */

#include <cmath>
#include <map>
#include <stdexcept>

#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

#include "AntipoleTree.h"

const int HelperFunctions::tournament_size = 3;
const long AntipoleTree::minimum_size = 100;

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

std::pair<long, float> AntipoleInternalNode::visitNode(const std::vector<float>& image, float max_dist, NodeMap& node_map) const
{
  node_map.insert(std::make_pair(HelperFunctions::distance2(image, left->getCenter()) - left->getRadius(), left));
  node_map.insert(std::make_pair(HelperFunctions::distance2(image, right->getCenter()) - right->getRadius(), right));
  return std::make_pair(-1, std::numeric_limits<float>::max());
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
  long closest = -1;
  float mindist = std::numeric_limits<float>::max();

  for(MatchingThumbnails::const_iterator it = matching_thumbnails.begin(); it != matching_thumbnails.end(); ++it)
  {
    float dist = HelperFunctions::distance2(image, thumbnails[*it]);
    if(dist < mindist)
    {
      mindist = dist;
      closest = *it;
    }
  }
  return std::make_pair(closest, mindist);
}

std::pair<long, float> AntipoleLeaf::visitNode(const std::vector<float>& image, float max_dist, NodeMap& node_map) const
{
  return getClosestThumbnail(image, max_dist);
}

void AntipoleLeaf::setMatching(const MatchingThumbnails& matching_thumbnails)
{
  this->matching_thumbnails = matching_thumbnails;
}

AntipoleTree::AntipoleTree(void)
  :root(NULL), conversion_method(0)
{
}

AntipoleTree::~AntipoleTree()
{
  delete root;
}

void AntipoleTree::setConversionMethod(int conversion_method)
{
  this->conversion_method = conversion_method;
}

void AntipoleTree::build(const QVector<QImage>& thumbnails)
{
  this->thumbnails.clear();
  
  MatchingThumbnails default_matching;
  for(int i = 0; i < thumbnails.size(); ++i)
  {
    default_matching.insert(i);
    this->thumbnails.push_back(convert(thumbnails[i]));
  }
  delete root;

  root = buildNewNode(minimum_size, default_matching);

}

const std::vector<std::vector<float> >& AntipoleTree::getThumbnails() const
{
  return thumbnails;
}

long AntipoleTree::getClosestThumbnail(const std::vector<float>& image) const
{
  if(root)
  {
    NodeMap visiting_map;
    visiting_map.insert(std::make_pair(0, root));
    std::pair<long, float> best_pair = std::make_pair(-1, std::numeric_limits<float>::max());

    while(!visiting_map.empty() && best_pair.second > visiting_map.begin()->first)
    {
      AntipoleNode* node = visiting_map.begin()->second;
      visiting_map.erase(visiting_map.begin());
      std::pair<long, float> node_best_pair = node->visitNode(image, best_pair.second, visiting_map);
      if(node_best_pair.first >=0 && node_best_pair.second < best_pair.second)
      {
        best_pair = node_best_pair;
      }
    }

    return best_pair.first; //root->getClosestThumbnail(image, std::numeric_limits<float>::max()).first;
  }
  else
  {
    return -1;
  }
}

std::vector<float> AntipoleTree::convert(const QImage& image) const
{
  switch(conversion_method)
  {
    case 0:
      return HelperFunctions::convert_rgb(image);
      break;
    case 1:
      return HelperFunctions::convert_lab(image);
      break;
    case 2:
      return HelperFunctions::convert_lch(image);
      break;
  }
  throw std::runtime_error("Bad conversion method");
}

long AntipoleTree::getClosestThumbnail(const QImage& image) const
{
  return getClosestThumbnail(convert(image));
}

AntipoleNode* AntipoleTree::buildNewNode(float minimum_size, const MatchingThumbnails& old_matching)
{
  MatchingThumbnails left_matching;
  MatchingThumbnails right_matching;
  std::vector<float> left_center;
  std::vector<float> right_center;

  if (old_matching.size() > minimum_size)
  {
    int status = divideMatching(minimum_size, old_matching, left_center, right_center, left_matching, right_matching);
    if(status == 1)
    {
      AntipoleInternalNode* node = new AntipoleInternalNode(this);
      QFuture<AntipoleNode*> left_future = QtConcurrent::run(this, &AntipoleTree::buildNewNode, minimum_size, left_matching);
      QFuture<AntipoleNode*> right_future = QtConcurrent::run(this, &AntipoleTree::buildNewNode, minimum_size, right_matching);
      AntipoleNode* left = left_future.result();
      AntipoleNode* right = right_future.result();
      node->setLeft(left);
      computeCenter(left_center, left_matching);
      left->setCenter(left_center);
      left->setRadius(computeMaxRadius(left_center, left_matching));
      node->setRight(right);
      computeCenter(right_center, right_matching);
      right->setCenter(right_center);
      right->setRadius(computeMaxRadius(right_center, right_matching));
    }
  }
  AntipoleLeaf* leaf = new AntipoleLeaf(this);
  leaf->setMatching(old_matching);
  return leaf;
}

int AntipoleTree::divideMatching(float minimum_size, const MatchingThumbnails& old_matching, std::vector<float>& left_center, std::vector<float>& right_center, MatchingThumbnails& left_matching, MatchingThumbnails& right_matching)
{
  std::pair<std::vector<float>, std::vector<float> > pair = HelperFunctions::approxAntipole(thumbnails, old_matching);
  left_center = pair.first;
  right_center = pair.second;
  assignMatching(old_matching, left_center, right_center, left_matching, right_matching);
  return 1;
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
    long start = it - copied_objects.begin();
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
    long result = (median1(new_tournament) + start) % copied_objects.size();
    long distance = it - copied_objects.begin();
    copied_objects.erase(copied_objects.begin() + result);
    if(result < distance)
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

std::vector<float> HelperFunctions::convert_rgb(const QImage& image)
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

static float pivotRGB(float n)
{
  return (n > 0.04045 ? std::pow((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100;
}

static float pivotXYZ(float n)
{
  return (n > 0.008856 ? std::pow(n, 1. / 3.) : (903.3 * n + 16) / 116);
}

void convertRGB2XYZ(float r, float g, float b, float& x, float& y, float& z)
{
  float x_ref= 95.047;
  float y_ref = 100;
  float z_ref = 108.883;
  x = pivotXYZ(r * 0.4124 + g * 0.3576 + b * 0.1805) / x_ref;
  y = pivotXYZ(r * 0.2126 + g * 0.7152 + b * 0.0722) / y_ref;
  z = pivotXYZ(r * 0.0193 + g * 0.1192 + b * 0.9505) / z_ref;
}

void convertXYZ2LAB(float x, float y, float z, float& l, float& a, float& b)
{
  l = std::max(0., 116. * y - 16);
  a = 500 * (x - y);
  b = 200 * (y - z);
}

void convertRGB2LAB(float red, float green, float blue, float& l, float& a, float& b)
{
  float x, y, z;
  convertRGB2XYZ(red, green, blue, x, y, z);
  convertXYZ2LAB(x, y, z, l, a, b);
}

void convertAB2CH(float a, float b, float& c, float& h)
{
  h = std::atan2(b, a) / M_PI * 180;
  c = std::sqrt(a*a + b*b);
}

void convertRGB2LCH(float red, float green, float blue, float& l, float& c, float& h)
{
  float x, y, z, a, b;
  convertRGB2XYZ(red, green, blue, x, y, z);
  convertXYZ2LAB(x, y, z, l, a, b);
  convertAB2CH(a, b, c, h);
}

std::vector<float> HelperFunctions::convert_lab(const QImage& image)
{
  std::vector<float> thumbnail;
  
  for(int j = 0; j < image.height(); ++j)
  {
    for(int i = 0; i < image.width(); ++i)
    {
      QRgb pixel = image.pixel(i, j);
      
      float red = pivotRGB(qRed(pixel));
      float green = pivotRGB(qGreen(pixel));
      float blue = pivotRGB(qBlue(pixel));
      
      float l, a, b;
      
      convertRGB2LAB(red, green, blue, l, a, b);
      
      thumbnail.push_back(l);
      thumbnail.push_back(a);
      thumbnail.push_back(b);
    }
  }
  
  return thumbnail;
}

std::vector<float> HelperFunctions::convert_lch(const QImage& image)
{
  std::vector<float> thumbnail;
  
  for(int j = 0; j < image.height(); ++j)
  {
    for(int i = 0; i < image.width(); ++i)
    {
      QRgb pixel = image.pixel(i, j);
      
      float red = pivotRGB(qRed(pixel));
      float green = pivotRGB(qGreen(pixel));
      float blue = pivotRGB(qBlue(pixel));
      
      float l, c, h;
      
      convertRGB2LCH(red, green, blue, l, c, h);
      
      thumbnail.push_back(l);
      thumbnail.push_back(c);
      thumbnail.push_back(h);
    }
  }
  
  return thumbnail;
}
