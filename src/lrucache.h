#ifndef LRUCACHE
#define LRUCACHE
typedef struct FrameNode{
  struct FrameNode* prev;
  struct FrameNode* next;
  int frameNum;
} FrameNode;

FrameNode* createFrameNode(int frameNum, FrameNode* prev, FrameNode* next);
void insertFrameNode(int frameNum);
void createFrameTracker(int numOfFrames);
void moveToFront(int frameNum);
void remove(int frameNum);
int getLRUFrame();
#endif