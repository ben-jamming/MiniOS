#ifndef LRUCACHE
#define LRUCACHE
typedef struct FrameNode{
  struct FrameNode* prev;
  struct FrameNode* next;
  int frameNum;
} FrameNode;

FrameNode* createFrameNode(int frameNum, FrameNode* prev, FrameNode* next);
void insertFrame(int frameNum);
void insertFrameNode(FrameNode* newNode);
void createFrameTracker(int numOfFrames);
void moveToFront(int frameNum);
void removeFrame(int frameNum);
int getLRUFrame();
void freeCache();
#endif