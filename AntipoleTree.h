/**
 * \file AntipoleTree.h
 */

#ifndef ANTIPOLETREE
#define ANTIPOLETREE

#include <vector>
#include <set>
#include <qimage.h>

class AntipoleTree;

typedef std::set<long> MatchingThumbnails;

class AntipoleNode
{
protected:
  const AntipoleTree* tree;
  float radius;
  std::vector<float> center;
public:
  AntipoleNode(const AntipoleTree* tree);
  virtual ~AntipoleNode();

  void setCenter(const std::vector<float>& center);
  void setRadius(float radius);
  float minimumDistance(const std::vector<float>& image) const;

  virtual bool isLeaf() const = 0;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image, float max_dist) const = 0;
};

class AntipoleInternalNode: public AntipoleNode
{
  AntipoleNode* left;
  AntipoleNode* right;

  std::pair<long, float> getBestSubnode(const AntipoleNode* node, const std::vector<float>& image, const std::pair<long, float>& current_best) const;
public:
  AntipoleInternalNode(const AntipoleTree* tree);
  virtual ~AntipoleInternalNode();

  void setLeft(AntipoleNode* left);
  void setRight(AntipoleNode* right);

  virtual bool isLeaf() const;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image, float max_dist) const;
};

class AntipoleLeaf: public AntipoleNode
{
  MatchingThumbnails matching_thumbnails;
public:
  AntipoleLeaf(const AntipoleTree* tree);
  virtual ~AntipoleLeaf();

  virtual bool isLeaf() const;
  virtual std::pair<long, float> getClosestThumbnail(const std::vector<float>& image, float max_dist) const;
  void setMatching(const MatchingThumbnails& inner_thumbnails);
};

class AntipoleTree
{
  std::vector<std::vector<float> > thumbnails;
  AntipoleNode* root;

  AntipoleNode* buildNewNode(float minimum_size, const MatchingThumbnails& old_matching);
  int divideMatching(float minimum_size, const MatchingThumbnails& old_matching, std::vector<float>& left_center, std::vector<float>& righ_center, MatchingThumbnails& left_matching, MatchingThumbnails& right_matching);
  void assignMatching(const MatchingThumbnails& old_matching, std::vector<float>& left_center, std::vector<float>& right_center, MatchingThumbnails& left_matching, MatchingThumbnails& right_matching);
  float computeMaxRadius(const std::vector<float>& center, const MatchingThumbnails& matching) const;
  void computeCenter(std::vector<float>& center, const MatchingThumbnails& matching) const;
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
  static const int tournament_size;
  static float distance2(const std::vector<float>& image1, const std::vector<float>& image2);
  static std::vector<float> convert(const QImage& image);
  static long median1(const std::vector<std::vector<float> >& objects);
  static std::pair<std::vector<float>, std::vector<float> > approxAntipole(const std::vector<std::vector<float> >& objects, const MatchingThumbnails& matching);
};

#endif
