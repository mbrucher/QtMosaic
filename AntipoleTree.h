/**
 * \file AntipoleTree.h
 */

#ifndef ANTIPOLETREE
#define ANTIPOLETREE

#include <vector>

#include <qimage.h>

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
  std::vector<long> inner_thumbnails;
  std::vector<long> local_index_to_global_index;
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

  void build(const std::vector<std::vector<float> >& thumbnails);

  long getClosestThumbnail(const std::vector<float>& image);
};

struct HelperFunctions
{
  static float distance2(const std::vector<float>& image1, const std::vector<float>& image2);
  static std::vector<float> convert(const QImage& image);
  static long median1(const std::vector<std::vector<float> >& objects);
};

#endif
