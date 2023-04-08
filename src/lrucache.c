
#include <stdio.h>
#include "lrucache.h"

FrameNode frame_tracker_h; //head of lru list
FrameNode frame_tracker_t; //tail of lru list
int* frame_tracker_map; //array of the frame nodes, indexed by frame number
int num_of_frames;

FrameNode* createFrameNode(int frameNum, FrameNode* prev, FrameNode* next);
void insertFrameNode(int frameNum);
void createFrameTracker(int numOfFrames);
void moveToFront(int frameNum);
void remove(int frameNum);

void insertFrameNode(int frameNum) {
  //adds inserts the frame node into the head
  //this should not create a new one, but use the map to put it at the head


}

void createFrameTracker(int numOfFrames) {
  //this creates all the nodes for the frame list
  //also create the map
}

FrameNode* createFrameNode(int frameNum, FrameNode* prev, FrameNode* next) {
  FrameNode* node = malloc(sizeof(FrameNode));
  node->next = next;
  node->prev = prev;
  node->frameNum = frameNum;
}

void moveToFront(int frameNum) {
  //this removes the frame from its current spot in the list
  //then inserts it at teh front of the list
}

void remove(int frameNum) {
  //this removes the node from the list of nods
  //if the node is the tail, then the tail is updated
  //if this is the head, then the head is updated

}

int getLRUFrame() {
  //this returns the frame number of the head

}