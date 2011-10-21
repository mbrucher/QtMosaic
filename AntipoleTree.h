/**
 * \file AntipoleTree.h
 */

#ifndef ANTIPOLETREE
#define ANTIPOLETREE

#include <vector>

class AntipoleNode
{
  std::vector<long> imagesReferences;
public:
  virtual ~AntipoleNode();
  virtual bool isLeaf() = 0;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image, const std::vector<std::vector<float> >& thumbnails) = 0;
};

class AntipoleInternalNode: public AntipoleNode
{
  AntipoleNode* left;
  AntipoleNode* right;
  float radius;
public:
  virtual ~AntipoleInternalNode();

  virtual bool isLeaf();
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image, const std::vector<std::vector<float> >& thumbnails);
};

class AntipoleLeaf: public AntipoleNode
{
public:
  virtual ~AntipoleLeaf();

  virtual bool isLeaf();
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image, const std::vector<std::vector<float> >& thumbnails);
};

class AntipoleTree
{
  std::vector<std::vector<float> > thumbnails;
  AntipoleNode* root;
public:
  AntipoleTree(void);

  long getClosestThumbnail(const std::vector<float>& image);
};

#endif
