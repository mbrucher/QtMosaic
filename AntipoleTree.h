/**
 * \file AntipoleTree.h
 */

#ifndef ANTIPOLETREE
#define ANTIPOLETREE

#include <vector>
#include <set>

#include <qimage.h>

class AntipoleTree;

class AntipoleNode
{
protected:
  const AntipoleTree* tree;
public:
  AntipoleNode(const AntipoleTree* tree);
  virtual ~AntipoleNode();
  virtual bool isLeaf() const = 0;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image) const = 0;
};

class AntipoleInternalNode: public AntipoleNode
{
  AntipoleNode* left;
  AntipoleNode* right;
  float radius;
public:
  AntipoleInternalNode(const AntipoleTree* tree);
  virtual ~AntipoleInternalNode();

  virtual bool isLeaf() const;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image) const;
};

class AntipoleLeaf: public AntipoleNode
{
  std::set<long> inner_thumbnails;
public:
  AntipoleLeaf(const AntipoleTree* tree);
  virtual ~AntipoleLeaf();

  virtual bool isLeaf() const;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image) const;
};

class AntipoleTree
{
  std::vector<std::vector<float> > thumbnails;
  AntipoleNode* root;
public:
  AntipoleTree(void);
  ~AntipoleTree();

  const std::vector<std::vector<float> >& getThumbnails() const;
  void build(const std::vector<std::vector<float> >& thumbnails);

  long getClosestThumbnail(const std::vector<float>& image) const;
  long getClosestThumbnail(const QImage& image) const;
};

struct HelperFunctions
{
  static float distance2(const std::vector<float>& image1, const std::vector<float>& image2);
  static std::vector<float> convert(const QImage& image);
  static long median1(const std::vector<std::vector<float> >& objects);
};

#endif
