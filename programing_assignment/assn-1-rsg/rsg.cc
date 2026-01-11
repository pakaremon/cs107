/**
 * File: rsg.cc
 * ------------
 * Provides the implementation of the full RSG application, which
 * relies on the services of the built-in string, ifstream, vector,
 * and map classes as well as the custom Production and Definition
 * classes provided with the assignment.
 */
 
#include <map>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include "definition.h"
#include "production.h"
using namespace std;

/**
 * Takes a reference to a legitimate infile (one that's been set up
 * to layer over a file) and populates the grammar map with the
 * collection of definitions that are spelled out in the referenced
 * file.  The function is written under the assumption that the
 * referenced data file is really a grammar file that's properly
 * formatted.  You may assume that all grammars are in fact properly
 * formatted.
 *
 * @param infile a valid reference to a flat text file storing the grammar.
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void readGrammar(ifstream& infile, map<string, Definition>& grammar)
{
  while (true) {
    string uselessText;
    getline(infile, uselessText, '{');
    if (infile.eof()) return;  // true? we encountered EOF before we saw a '{': no more productions!
    infile.putback('{');
    Definition def(infile);
    grammar[def.getNonterminal()] = def;
  }
}

/**
 * Returns true if str is a terminal, false otherwise.
 *
 * @param str a string to determine if it is a terminal.
 * @return true if the given string is a terminal, false otherwise.
 */
static bool isTerminal(const string& str)
{
  return str[0] != '<';
}

/**
 * Takes a non-terminal as a string and a reference to the grammar map and
 * generates a random sentence, as a vector of strings representing terminals.
 *
 * @param nonterminal the non-terminal to expand.
 * @param grammar a reference to the STL map, which maps nonterminal strings to
 *                their definitions.
 * @param sentence a vector of strings, representing a sentence.
 */
static void generateRandomSentence(const string& nonterminal,
                                   map<string, Definition>& grammar,
                                   vector<string>& sentence)
{
  // if nonterminal is not in grammar, exit
  if (grammar.find(nonterminal) == grammar.end()) {
    cerr << "Could not find \"" << nonterminal << "\" in the grammar file."
         << endl;
    exit(EXIT_FAILURE);
  }
  
  Production prod = grammar[nonterminal].getRandomProduction();
  for (Production::iterator curr = prod.begin(); curr != prod.end(); ++curr) {
    if (isTerminal(*curr))
      sentence.push_back(*curr);
    else
      generateRandomSentence(*curr, grammar, sentence);
  }
}

/**
 * Takes a terminal and returns true if it is necessary to print a space before.
 *
 * @param terminal a terminal.
 * @return true if it is necessary to print a space before the terminal, false
 *         otherwise.
 */
static bool isSpaceNeeded(const string& terminal)
{
  return terminal != "'\'s" && terminal[0] != ',' && terminal[0] != '.'
                            && terminal[0] != ':' && terminal[0] != '?';
}

/**
 * Prints a vector of strings, representing a sentence.
 *
 * @param sentence a vector of strings which represents the sentence to print.
 */
static void printSentence(const vector<string>& sentence)
{
  static const int MAX_LINE_LENGTH = 55;
  cout << "   ";
  int current_line_length = 3;
  for (string terminal : sentence) {
    if (isSpaceNeeded(terminal)) {
      if (current_line_length + terminal.size() + 1 <= MAX_LINE_LENGTH)
        cout << " ";
      current_line_length += 1;
    }
    if (current_line_length + terminal.size() > MAX_LINE_LENGTH) {
      cout << endl;
      current_line_length = 0;
    }
    cout << terminal;
    current_line_length += terminal.size();
  }
  cout << endl;
}

/**
 * Performs the rudimentary error checking needed to confirm that
 * the client provided a grammar file.  It then continues to
 * open the file, read the grammar into a map<string, Definition>,
 * and then print out the total number of Definitions that were read
 * in.  You're to update and decompose the main function to print
 * three randomly generated sentences, as illustrated by the sample
 * application.
 *
 * @param argc the number of tokens making up the command that invoked
 *             the RSG executable.  There must be at least two arguments,
 *             and only the first two are used.
 * @param argv the sequence of tokens making up the command, where each
 *             token is represented as a '\0'-terminated C string.
 */

int main(int argc, char *argv[])
{
  if (argc == 1) {
    cerr << "You need to specify the name of a grammar file." << endl;
    cerr << "Usage: rsg <path to grammar text file>" << endl;
    return 1; // non-zero return value means something bad happened 
  }
  
  ifstream grammarFile(argv[1]);
  if (grammarFile.fail()) {
    cerr << "Failed to open the file named \"" << argv[1] << "\".  Check to ensure the file exists. " << endl;
    return 2; // each bad thing has its own bad return value
  }
  
  // things are looking good...
  map<string, Definition> grammar;
  readGrammar(grammarFile, grammar);

  // generate random sentences and print them
  for (int i = 0; i < 3; i++) {
    vector<string> sentence;
    generateRandomSentence("<start>", grammar, sentence);
    cout << "Version #" << i+1 << ": ---------------------------" << endl;
    printSentence(sentence);
    cout << endl;
  }
  
  return 0;
}
