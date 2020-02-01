#include "chain.h"
#include "chain_given.cpp"

// PA1 functions

/**
 * Destroys the current Chain. This function should ensure that
 * memory does not leak on destruction of a chain.
 */
Chain::~Chain(){
  clear();
  delete head_;
}

/**
 * Inserts a new node at the end of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
void Chain::insertBack(const Block & ndata){
  length_++;
  Node * b = new Node(ndata);
  b->next = head_;
  b->prev = head_->prev;
  head_->prev = b;
  b->prev->next = b;
}

/**
 * Modifies the Chain by moving the subchain of len Nodes,
 * starting at position startPos, dist positions toward the
 * end of the chain.  If startPos + len - 1 + dist > length
 * then dist = length - startPos - len + 1 (to prevent gaps
 * in the new chain).
 * The subchain occupies positions (startPos + dist) through
 * (startPos + dist + len - 1) of the resulting chain.
 * The order of subchain nodes is not changed in the move.
 * You may assume that: 1 <= startPos <= length - len + 1,
 * 0 <= dist <= length, and 0 <= len <= length. 
 */
void Chain::moveBack(int startPos, int len, int dist){
	Node * start_n = walk(head_, startPos);
	Node * n1 = start_n->prev;
	int d = dist;
	if (startPos + len - 1 + dist > length_) {
		d = length_ - startPos - len + 1;
	}
	Node * end_n = walk(start_n,len-1);
	Node * n2 = end_n->next;
	n1->next = n2;
	n2->prev = n1;
	Node * n3 = walk(n1,d);
	Node * n4 = n3->next;
	start_n->prev = n3;
	n3->next = start_n;
	end_n->next = n4;
	n4->prev = end_n;
}

/**
 * Rolls the current Chain by k nodes: reorders the current list
 * so that the first k nodes are the (n-k+1)th, (n-k+2)th, ... , nth
 * nodes of the original list followed by the 1st, 2nd, ..., (n-k)th
 * nodes of the original list where n is the length.
 */
void Chain::roll(int k){
	Node * first = walk(head_,length_-k+1);
	head_->next->prev = head_->prev;
	head_->prev->next = head_->next;
	first->prev->next = head_;
	head_->prev = first->prev;
	head_->next = first;
	first->prev = head_;
}

/**
 * Modifies the current chain by reversing the sequence
 * of nodes from pos1 up to and including pos2. You may
 * assume that pos1 and pos2 exist in the given chain,
 * and pos1 <= pos2.
 * The positions are 1-based.
 */
void Chain::reverseSub(int pos1, int pos2){
	Node * start = walk(head_,pos1);
	Node * n1 = start->prev;
	Node * end = walk(start,pos2-pos1);
	Node * n2 = end->next;
	Node * temp = n1->prev;
	for (int k = 0; k < pos2-pos1+1; k++) {
		n1->next = end;
		n1->prev = temp;
		n1 = n1->next;
		end = end->prev;
		temp = temp->next;
	}
	start->next = n2;
	start->prev = temp;
	n2->prev = start;
}

/*
* Modifies both the current chain and the "other" chain by removing
* nodes from the other chain and adding them between the nodes
* of the current chain -- one "other" node between two current nodes --
* until one of the two chains is exhausted.  Then the rest of the
* nodes in the non-empty chain follow.
* The length of the resulting chain should be the sum of the lengths
* of current and other. The other chain should have only
* the head_ sentinel at the end of the operation.
* The weave fails and the original
* chains should be unchanged if block sizes are different.
* In that case, the result of the function should be:
* cout << "Block sizes differ." << endl;
*/
void Chain::weave(Chain & other) { // leaves other empty.
	if (head_->next->data.width() != other.head_->next->data.width() || head_->next->data.height() != other.head_->next->data.height()) {
		cout << "Block sizes differ." << endl;
	}
	Node * org = head_->next;
	Node * oth = other.head_->next;
	for (int i = 1; i < min(length_,other.length_)*2; i++) {
		oth->prev = org;
		Node * temp = org->next;
		org->next = oth;
		oth = temp;
		org = org->next;
	}
	if (other.length_ >= length_) {
		while (org->next != other.head_) {
			org = org->next;
		}
		org->next = head_;
		head_->prev = org;
	}
	if (other.length_ < length_) {
		org->next = oth;
		oth->prev = org;
		while (org->next != head_) {
			org = org->next;
		}
	}
	length_ = length_+other.length_;
	other.length_ = 0;
	other.head_->next = other.head_;
	other.head_->prev = other.head_;
}


/**
 * Destroys all dynamically allocated memory associated with the
 * current Chain class except for the sentinel head_. Sets length_
 * to zero.  After clear() the chain represents an empty chain.
 */
void Chain::clear() {
  while (head_->next != head_) {
	  Node * p = head_->next;
	  head_->next = p->next;
	  delete p;
  }
  head_->prev = head_;
  length_ = 0;
}

/**
 * Makes the current object into a copy of the parameter:
 * All member variables should have the same value as
 * those of other, but the memory should be completely
 * independent. This function is used in both the copy
 * constructor and the assignment operator for Chains.
 */
void Chain::copy(Chain const& other) {
	length_ = other.length_;
	height_ = other.height_;
	width_ = other.width_;
	Node * p = other.head_;
	while (p->next != other.head_) {
		p = p->next;
		insertBack(p->data);
	}
}
