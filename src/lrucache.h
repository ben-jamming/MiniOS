#ifndef LRUCACHE
#define LRUCACHE
typedef struct FrameNode{

} FrameNode;

void insertFrameNode(int frameNum);
void createFrameTracker(int numOfFrames);
void moveToFront(int frameNum);
void remove(int frameNum);
int getLRUFrame();
#endif