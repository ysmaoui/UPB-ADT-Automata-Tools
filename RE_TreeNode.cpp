/**
 * @file RETreeNode.cpp
 * Implementation of the regular expression tree node class.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 */
#include <stddef.h>
#include <string>
#include "RE_TreeNode.hpp"
#include "RE_RegularExpression.hpp"
#include "FSA_FiniteStateAutomata.hpp"
#include "FSA_State.hpp"

using namespace std;

/**
 * Creates a new node in the regular expression tree.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 * @param c The content of the node. This is either an operator or a literal.
 */
RETreeNode::RETreeNode(string c) {
	content = c;
	if(content.compare("<epsilon>")==0) {
		//empty literal
		content = "";
	}
	p_left = NULL;
	p_right = NULL;
}

RETreeNode::~RETreeNode() {
	if(p_left != NULL) {
		delete p_left;
	}
	if(p_right != NULL) {
		delete p_right;
	}
}

/**
 * Gets the content of the node. This is either an operator or a literal.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 * @return Node content.
 */
string RETreeNode::getContent() {
	return content;
}

/**
 * Sets the content of the node. This can either be an operator or a literal.
 * Note that you cannot change a node type by giving an operator node a literal value
 * or vice versa.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 */
void RETreeNode::setContent(string c) {
	if(!isOperator() && RegularExpression::isOperator(c))
		throw "Cannot change a terminal to an operator.";
	if(isOperator() && !RegularExpression::isOperator(c))
		throw "Cannot change an operator to a terminal.";
	content = c;
}

/**
 * Checks whether the given node represents an operator.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 * @return Returns true only if a child node is present and the content of the node is a valid operator.
 */
bool RETreeNode::isOperator() {
	return (p_left != NULL || RegularExpression::isOperator(content));
}

/**
 * Gets the left operand of this operator. Literals have no left operand.
 * @return Left operand.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 */
RETreeNode *RETreeNode::getLeft() {
	return p_left;
}

/**
 * Gets the right operand of this operator. Literals have no right operand.
 * @return Right operand.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 */
RETreeNode *RETreeNode::getRight() {
	return p_right;
}

/**
 * Sets the left operand of this operator. Literals can have no left operand.
 * @param p_l Left operand.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 */
void RETreeNode::setLeft(RETreeNode *p_l) {
	if(isOperator())
		p_left = p_l;
	else
		throw "Node ("+content+") does not represent an operator. Cannot add left child: "+p_l->getContent();
}

/**
 * Sets the right operand of this operator. Literals can have no right operand.
 * @param p_r Right operand.
 * @author Daniel Dreibrodt, Konstantin Steinmiller
 */
void RETreeNode::setRight(RETreeNode *p_r) {
	if(isOperator())
		p_right = p_r;
	else
		throw "Node ("+content+") does not represent an operator. Cannot add right child: "+p_r->getContent();
}

/**
 * Determines whether this tree node represents an empty literal,
 * which is usually represented by the epsilon symbol.
 * @return Whether the content of this tree node is empty.
 * @author Daniel Dreibrodt
 */
bool RETreeNode::isEmpty() {
	return content.length()==0;
}

/**
 * Generates a non-deterministic finite state automaton representing
 * the regular expression tree with this node as its root.
 * @param labelNum Pointer to the counter variable that is used for naming the states of the FSA.
 * @return A NDFSA representing the regular expression tree with this node as its root.
 */
FiniteStateAutomata *RETreeNode::toFSA(int *labelNum) {
	FiniteStateAutomata *fsa = new FiniteStateAutomata();
	if(isOperator()) {
		if(content.compare(RegularExpression::re_andOp)==0) {
			// For a concatenation all final states of the FSA for the left subtree
			// have to be connected with an empty transition to the start state
			// of the FSA for the right subtree.
			// All final states of the left FSA have to be turned into normal states.
			// The start state of the right FSA has to be turned into a normal state.

			FiniteStateAutomata *leftFSA = p_left->toFSA(labelNum);
			FiniteStateAutomata *rightFSA = p_right->toFSA(labelNum);

			//TODO
		} else if(content.compare(RegularExpression::re_orOp)==0) {
			// For an boolean or operator the start states of the left and right
			// subtree's FSAs have to be merged.
			// As this is not possible with our API a new start state is created
			// which has empty transitions to both starting states of the left
			// and right subtree's FSAs.
			// Those two start states are then turned into normal states.

			char *stateAname = new char[20];
			sprintf(stateAname, "State%d", *labelNum);
			State *startState = new State(stateAname, true, false);
			(*labelNum)++;

			FiniteStateAutomata *leftFSA = p_left->toFSA(labelNum);
			FiniteStateAutomata *rightFSA = p_right->toFSA(labelNum);

			//TODO
		} else if(content.compare(RegularExpression::re_starOp)==0) {
			// For a repetition all final states of the left subtree's
			// FSA are connected to its starting state.
			// Then the starting state is also turned into a final state.

			FiniteStateAutomata *leftFSA = p_left->toFSA(labelNum);

			//TODO
		}
	} else {
		//For literals create one start state A and one final state B
		//The transition between A and B is marked by the literal itself

		char *stateAname = new char[20];
		sprintf(stateAname, "State%d", *labelNum);
		(*labelNum)++;
		State *stateA = new State(stateAname, true, false);
		fsa->addState(stateA);

		char *stateBname = new char[20];
		sprintf(stateBname, "State%db", *labelNum);
		(*labelNum)++;
		State *stateB = new State(stateBname, false, true);
		fsa->addState(stateB);

		fsa->addTransition(stateAname, content, stateBname);
	}
	return fsa;
}