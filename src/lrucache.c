
#include <stdio.h>
#include "lrucache.h"

//dummy head and tail node
FrameNode* frame_tracker_h = NULL; //head of lru list
FrameNode* frame_tracker_t = NULL; //tail of lru list
FrameNode** frame_tracker_map; //array of the frame nodes, indexed by frame number
int num_of_frames;

FrameNode* createFrameNode(int frameNum, FrameNode* prev, FrameNode* next);
void insertFrameNode(int frameNum);
void createFrameTracker(int numOfFrames);
void moveToFront(int frameNum);
void remove(int frameNum);

void insertFrameNode(int frameNum) {
  //adds inserts the frame node into the head
  //this should not create a new one, but use the map to put it at the head
  FrameNode* node = frame_tracker_map[frameNum];
  FrameNode* firstNode = frame_tracker_h->next;
  //set prev to head
  node->prev = frame_tracker_h;
  //set next to head.next
  node->next = firstNode;
  //set head.prev to node
  firstNode->prev = node;
  frame_tracker_h->next = node;
}

void createFrameTracker(int numOfFrames) {
  frame_tracker_h = createFrameNode(-1, NULL, frame_tracker_t);
  frame_tracker_t = createFrameNode(-2, frame_tracker_h, NULL);

  frame_tracker_map = malloc(numOfFrames * sizeof(FrameNode*));
  num_of_frames = num_of_frames;
  //also create the map
  //this creates all the nodes for the frame list
  for (int i = 0; i < num_of_frames; i++) {
    frame_tracker_map[i] = createFrameNode(i, NULL, NULL); 
    insertFrameNode(frame_tracker_map[i]);
  }
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
  //this removes the node from the list of nodes

  FrameNode* node = frame_tracker_map[frameNum];
  FrameNode* prevNode = node->prev;
  FrameNode* nextNode = node->next;

  //make neighbours point to eachother
  prevNode->next = nextNode;
  nextNode->prev = prevNode;

  //reset pointers
  node->next = NULL;
  node->prev = NULL;
}

int getLRUFrame() {
  //this returns the frame number of the tail - 1

}