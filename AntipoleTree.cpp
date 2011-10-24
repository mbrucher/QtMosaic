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

void AntipoleTree::build(const std::vector<std::vector<float> >& thumbnails)
{
  this->thumbnails = thumbnails;
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

float HelperFunctions::distance(const std::vector<float>& image1, const std::vector<float>& image2)
{
  float dist = 0;

  for(std::vector<float>::const_iterator it1 = image1.begin(), it2 = image2.begin(); it1 != image1.end() && it2 != image2.end(); ++it1, ++it2)
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
