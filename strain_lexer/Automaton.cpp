/*************************************************
 * File: Automaton.hh
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of functions which work on
 * RegexpAutomata.
 */

#include "Automaton.hh"
#include <set>
#include <queue>
#include <algorithm>
#include <deque>
#include <iostream>
#include <iterator>
using namespace std;

/* Returns the e-closure of a particular state, which is the
 * set of states reachable from that state only by following
 * e-transitions.
 */
set<MachineState*> EpsilonClosure(MachineState* state) {
	/* Do a BFS in the graph. */
	set<MachineState*> visited;
	queue<MachineState*> toVisit;
	toVisit.push(state);

	while (!toVisit.empty()) {
		MachineState* curr = toVisit.front();
		toVisit.pop();

		if (visited.count(curr)) continue;
		visited.insert(curr);

		for (set<MachineState*>::iterator itr =
			curr->eTransitions.begin();
			itr != curr->eTransitions.end(); ++itr)
			toVisit.push(*itr);
	}

	return visited;
}

/* Comparison which compares states by finding the one with the lower-numbered
 * (higher) priority.
 */
bool CompareByPriority(MachineState* one, MachineState* two) {
	return one->priority < two->priority;
}

/* Given a set of states, returns the new state which corresponds to
 * those states.
 */
MachineState*
SetOfStatesToState(const set<MachineState*>& states,
	map<set<MachineState*>, MachineState*>& lkp) {
	MachineState* referent = lkp[states];
	if (referent == 0) { // Not entered yet
	  /* Figure out what the terminal is.  This is the minimum of the terminal
	   * number of each state in the set of states.
	   */
		int terminalNumber =
			(*min_element(states.begin(), states.end(),
				CompareByPriority))->terminalNumber;
		referent = lkp[states] = new MachineState(terminalNumber);
	}
	return referent;
}

/* Performs the subset construction on an e-NFA to convert it into
 * a DFA.
 */
RegexpAutomaton DoSubsetConstruction(RegexpAutomaton automaton) {
	/* Mapping from sets to new states. */
	map<set<MachineState*>, MachineState*> stateLookup;

	/* Compute the new start state as the eclose of the original
	 * start state.
	 */
	set<MachineState*> startStates = EpsilonClosure(automaton.start);
	MachineState* start = SetOfStatesToState(startStates, stateLookup);

	/* Create a BFS queue. */
	queue< set<MachineState*> > toVisit;
	toVisit.push(startStates);

	/* ... and a visited set. */
	set<MachineState*> exploredNewStates;

	/* Explore! */
	while (!toVisit.empty()) {
		set<MachineState*> currStates = toVisit.front();
		toVisit.pop();

		/* Look up the corresponding state. */
		MachineState* referent = SetOfStatesToState(currStates,
			stateLookup);

		/* If this state has been visited, we're done. */
		if (exploredNewStates.count(referent)) continue;
		exploredNewStates.insert(referent);

		/* Compute the set of all hard transitions here. */
		multimap<char, MachineState*> groupTransitions;
		for (set<MachineState*>::iterator itr = currStates.begin();
			itr != currStates.end(); ++itr) {
			MachineState* curr = *itr;
			groupTransitions.insert(curr->transitions.begin(),
				curr->transitions.end());
		}

		/* Convert this into a map from chars to sets of states. */
		map< char, set<MachineState*> > setTransitions;
		for (multimap<char, MachineState*>::iterator it =
			groupTransitions.begin();
			it != groupTransitions.end(); ++it)
			setTransitions[it->first].insert(it->second);

		/* Replace each element with its epsilon closure. */
		for (map<char, set<MachineState*> >::iterator itr =
			setTransitions.begin();
			itr != setTransitions.end(); ++itr) {
			set<MachineState*> eClosure;
			for (set<MachineState*>::iterator stateItr = itr->second.begin();
				stateItr != itr->second.end(); ++stateItr) {
				set<MachineState*> currClosure = EpsilonClosure(*stateItr);
				eClosure.insert(currClosure.begin(), currClosure.end());
			}
			itr->second = eClosure;
		}

		/* Populate the actual transition table. */
		for (map<char, set<MachineState*> >::iterator itr =
			setTransitions.begin();
			itr != setTransitions.end(); ++itr) {
			MachineState* target = SetOfStatesToState(itr->second,
				stateLookup);
			referent->transitions.insert(make_pair(itr->first, target));
			toVisit.push(itr->second);
		}
	}

	RegexpAutomaton result;
	result.start = start;
	result.terminal = 0;
	return result;
}

/* For convenience. */
typedef pair<MachineState*, MachineState*> MachineStatePair;

/* Given a pair of states, returns a pair of those states in canonical
 * ordering.  This ordering is chosen using the integer representation
 * of the addresses of the states.
 */
MachineStatePair CanonicalOrder(MachineState* one, MachineState* two) {
	return MachineStatePair(min(one, two), max(one, two));
}

/* Returns a set of all states in the specified automaton. */
set<MachineState*> GetAllStatesIn(RegexpAutomaton automaton) {
	/* Standard BFS traversal. */
	set<MachineState*> result;
	queue<MachineState*> toVisit;

	toVisit.push(automaton.start);

	while (!toVisit.empty()) {
		MachineState* curr = toVisit.front();
		toVisit.pop();

		/* Skip null for now. */
		if (curr == NULL) continue;

		if (result.find(curr) != result.end()) continue;
		result.insert(curr);

		/* Explore outward. */
		for (multimap<char, MachineState*>::iterator itr =
			curr->transitions.begin(); itr != curr->transitions.end(); ++itr)
			toVisit.push(itr->second);
		for (set<MachineState*>::iterator itr =
			curr->eTransitions.begin(); itr != curr->eTransitions.end(); ++itr)
			toVisit.push(*itr);
	}

	return result;
}

/* Returns the successor state of a particular state given a particular
 * character.  This may be NULL if no transition is defined.
 */
MachineState* Successor(MachineState* state, char ch) {
	multimap<char, MachineState*>::iterator result =
		state->transitions.find(ch);
	return result == state->transitions.end() ? NULL : result->second;
}

/* Advances an iterator one step. */
template <typename IteratorType> IteratorType IterNext(IteratorType itr) {
	return ++itr;
}

/* Uses the construction from "Automata Theory, Languages, and Computation,
 * 3rd Edition" by Hopcroft, Motwani, and Ullman.
 *
 * We will use a modified construction in this case.  We'll treat the
 * dead state as a terminal state and everything else as nonterminal.
 */
void MinimizeDFA(RegexpAutomaton& automaton) {

	/* Get an explicit set of all states in the automaton. */
	set<MachineState*> allStates = GetAllStatesIn(automaton);

	/* For each state, the set of states it is equivalent to. */
	map<MachineState*, set<MachineState*> > equivalences;

	/* Initially, everything is equivalent to everything else. */
	for (set<MachineState*>::iterator one = allStates.begin();
		one != allStates.end(); ++one) {
		for (set<MachineState*>::iterator two = allStates.begin();
			two != allStates.end(); ++two) {
			equivalences[*one].insert(*two);
		}
		equivalences[*one].insert(NULL);
		equivalences[NULL].insert(*one);
	}

	/* Map from canonical pairs of states to states that need to be
	 * marked distinguishable if the initial pair is marked
	 * distinguishable.
	 */
	map<MachineStatePair, set<MachineStatePair> > dependencies;

	/* For each pair of states and for each symbol, populate each list if
	 * the successors of that pair on that symbol aren't identically the
	 * same state.
	 */
	for (set<MachineState*>::iterator one = allStates.begin();
		one != allStates.end(); ++one) {
		for (set<MachineState*>::iterator two = IterNext(one);
			two != allStates.end(); ++two) {
			for (int ch = 0; ch < 128; ++ch) {
				MachineState* oneSucc = Successor(*one, char(ch));
				MachineState* twoSucc = Successor(*two, char(ch));
				if (oneSucc != twoSucc) {
					dependencies[CanonicalOrder(oneSucc, twoSucc)].
						insert(CanonicalOrder(*one, *two));
				}
			}
		}

		/* Same, but for NULL.  Note that NULL is its own successor
		 * always.
		 */
		for (int ch = 0; ch < 128; ++ch) {
			MachineState* oneSucc = Successor(*one, char(ch));
			if (oneSucc != NULL) {
				dependencies[CanonicalOrder(oneSucc, NULL)].
					insert(CanonicalOrder(*one, NULL));
			}
		}
	}

	/* deque of all pairs known to be bad and haven't been processed. */
	deque<MachineStatePair> workQueue;

	/* Initially, all terminal states are distinct from null. */
	for (set<MachineState*>::iterator one = allStates.begin();
		one != allStates.end(); ++one)
		if ((*one)->terminalNumber != numeric_limits<int>::max())
			workQueue.push_back(CanonicalOrder(*one, NULL));

	/* Also, all states with different terminal status are null. */
	for (set<MachineState*>::iterator one = allStates.begin();
		one != allStates.end(); ++one) {
		for (set<MachineState*>::iterator two = IterNext(one);
			two != allStates.end(); ++two) {
			if ((*one)->terminalNumber != (*two)->terminalNumber)
				workQueue.push_back(CanonicalOrder(*one, *two));
		}
	}

	/* Keep scanning through the worklist, disconnecting pairs
	 * that appear on the list.
	 */
	while (!workQueue.empty()) {
		/* Get the current element. */
		MachineStatePair curr = workQueue.front();
		workQueue.pop_front();

		/* If the two aren't equivalent, we're done. */
		if (!equivalences[curr.first].count(curr.second)) {
			continue;
		}

		/* Disconnect the two. */
		equivalences[curr.first].erase(curr.second);
		equivalences[curr.second].erase(curr.first);

		/* Update the worklist with the dependences. */
		workQueue.insert(workQueue.end(),
			dependencies[curr].begin(),
			dependencies[curr].end());
	}

	/* Rebuild the automaton by replacing all instances of the first
	 * set of states with the representative states for each.  The
	 * representatives are formed by taking the first element of each
	 * set of states.  Since these states are ordered by address and
	 * each entry of each equivalence class will have the same elements,
	 * this can be done by looking at the first element of each
	 * equivalence class.
	 *
	 * This is done using a BFS.  As usual.
	 */
	queue<MachineState*> toVisit;
	set<MachineState*> visited;

	/* Replace the start state with its representative. */
	automaton.start = *equivalences[automaton.start].begin();
	toVisit.push(automaton.start);

	/* We've already seen NULL... it never changes. */
	visited.insert(NULL);

	while (!toVisit.empty()) {
		MachineState* curr = toVisit.front();
		toVisit.pop();

		if (visited.find(curr) != visited.end()) continue;
		visited.insert(curr);

		/* Replace all transitions with transitions to the representative
		 * location, then add those locations to the work queue.
		 */
		for (multimap<char, MachineState*>::iterator itr =
			curr->transitions.begin(); itr != curr->transitions.end(); ++itr) {
			itr->second = *equivalences[itr->second].begin();
			toVisit.push(itr->second);
		}

		/* Note that there are no e-transitions... this is a DFA! */
	}

	clog << "Number of states: " << GetAllStatesIn(automaton).size()
		<< endl;
}