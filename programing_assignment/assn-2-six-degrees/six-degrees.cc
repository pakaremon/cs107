#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include "imdb.h"
#include "path.h"
using namespace std;

/**
 * Using the specified prompt, requests that the user supply
 * the name of an actor or actress.  The code returns
 * once the user has supplied a name for which some record within
 * the referenced imdb existsif (or if the user just hits return,
 * which is a signal that the empty string should just be returned.)
 *
 * @param prompt the text that should be used for the meaningful
 *               part of the user prompt.
 * @param db a reference to the imdb which can be used to confirm
 *           that a user's response is a legitimate one.
 * @return the name of the user-supplied actor or actress, or the
 *         empty string.
 */

static string promptForActor(const string& prompt, const imdb& db)
{
  string response;
  while (true) {
    cout << prompt << " [or <enter> to quit]: ";
    getline(cin, response);
    if (response == "") return "";
    vector<film> credits;
    if (db.getCredits(response, credits)) return response;
    cout << "We couldn't find \"" << response << "\" in the movie database. "
   << "Please try again." << endl;
  }
}

/**
 * Generate the shortest possible path from source to target.
 *
 * @param source the source actor/actress.
 * @param target the target actor/actress.
 * @param db a reference to the imdb in which to search for the shortes path.
 */

static void generateShortestPath(const string& source, const string& target,
         const imdb& db)
{
  list<path> partialPaths; // functions as a queue
  set<string> previouslySeenActors;
  set<film> previouslySeenFilms;

  // create a partial path around the source actor
  // and add it to the queue of partial paths
  partialPaths.push_back(path(source));

  while (!partialPaths.empty() && partialPaths.front().getLength() <= 5) {
    // pull off front path
    path frontPath = partialPaths.front();
    partialPaths.pop_front();

    // look up last actor's movies
    vector<film> films;
    db.getCredits(frontPath.getLastPlayer(), films);

    for (film movie : films) {
      // if movie was already seen, skip it
      // otherwise, insert it to previouslySeenFilms
      if (!previouslySeenFilms.insert(movie).second)
        continue;

      // look up movie's cast
      vector<string> cast;
      db.getCast(movie, cast);
      for (string actor : cast) {
        // if actor was already seen, skip her/him
        // otherwise, insert her/him to previouslySeenActors
        if (!previouslySeenActors.insert(actor).second)
          continue;

        path partialPathClone = frontPath; // clone frontPath
        // add the movie/costar connection to the clone
        partialPathClone.addConnection(movie, actor);

        // if the costar is the target, then print the path and return
        if (actor == target) {
          cout << endl << partialPathClone << endl;
          return;
        } else { // otherwise, add the clone to partialPaths
          partialPaths.push_back(partialPathClone);
        }
      }
    }
  }

  // path not found
  cout << endl << "No path between those two people could be found." << endl << endl;
}

/**
 * Serves as the main entry point for the six-degrees executable.
 * There are no parameters to speak of.
 *
 * @param argc the number of tokens passed to the command line to
 *             invoke this executable.  It's completely ignored
 *             here, because we don't expect any arguments.
 * @param argv the C strings making up the full command line.
 *             We expect argv[0] to be logically equivalent to
 *             "six-degrees" (or whatever absolute path was used to
 *             invoke the program), but otherwise these are ignored
 *             as well.
 * @return 0 if the program ends normally, and undefined otherwise.
 */

int main(int argc, const char *argv[])
{
  imdb db(determinePathToData(argv[1])); // inlined in imdb-utils.h
  if (!db.good()) {
    cout << "Failed to properly initialize the imdb database." << endl;
    cout << "Please check to make sure the source files exist and that you have permission to read them." << endl;
    exit(1);
  }

  while (true) {
    string source = promptForActor("Actor or actress", db);
    if (source == "") break;
    string target = promptForActor("Another actor or actress", db);
    if (target == "") break;
    if (source == target) {
      cout << "Good one.  This is only interesting if you specify two different people." << endl;
    } else {
      generateShortestPath(source, target, db);
    }
  }

  cout << "Thanks for playing!" << endl;
  return 0;
}
