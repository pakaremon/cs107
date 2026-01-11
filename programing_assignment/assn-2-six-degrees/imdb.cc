using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
            (movieInfo.fd == -1) ); 
}

struct playerData {
  const void *actorFile;
  const string *name;
};

int compareActors(const void *ap, const void *bp)
{
  playerData *pd = (playerData *) ap;
  int offset = *((int *) bp);

  string actorName = (const char *)  pd->actorFile + offset;
  return pd->name->compare(actorName);
}

bool imdb::getCredits(const string& player, vector<film>& films) const
{
  int numActors = *((int *) actorFile);
  playerData data;
  data.actorFile = actorFile;
  data.name = &player;

  // search for given player
  int *offset = (int *) bsearch(&data, (int *) actorFile + 1, numActors,
                                sizeof(int), compareActors);
  if (offset == NULL)
    return false; // not found

  // get number of movies in which the actor has appeared
  int numMoviesOffset = player.size() + 1;
  if (numMoviesOffset % 2 != 0)
    numMoviesOffset++;
  short numMovies = *((short *) ((char *) actorFile + *offset + numMoviesOffset));

  // get actor's films
  int moviesOffset = (numMoviesOffset + 2) % 4 == 0 ? 2 : 4;
  if ((numMoviesOffset + moviesOffset) % 4 != 0)
    moviesOffset = 4;
  for (int i = 0; i < numMovies; i++) {
    int movieOffset = *((int *) ((char *) actorFile + *offset + numMoviesOffset + moviesOffset) + i);
    string movieTitle = (const char *) movieFile + movieOffset;
    int movieYear = 1900 + *((char *) movieFile + movieOffset + movieTitle.size() + 1);
    film f;
    f.title = movieTitle;
    f.year = movieYear;
    films.push_back(f);
  }
  
  return true;
}

struct movieData {
  const void *movieFile;
  const film *movie;
};

int compareMovies(const void *ap, const void *bp)
{
  movieData *md = (movieData *) ap;
  int offset = *((int *) bp);

  film movie;
  movie.title = (const char *) md->movieFile + offset;
  movie.year = 1900 + *((char *) md->movieFile + offset + movie.title.size() + 1);

  if (*(md->movie) < movie)
    return -1;
  else if (*(md->movie) == movie)
    return 0;
  else
    return 1;
}

bool imdb::getCast(const film& movie, vector<string>& players) const
{
  // get number of movies
  int numMovies = *((int *) movieFile);

  movieData data;
  data.movieFile = movieFile;
  data.movie = &movie;

  // search for given movie
  int *offset = (int *) bsearch(&data, (int *) movieFile + 1, numMovies,
                                sizeof(int), compareMovies);
  if (offset == NULL)
    return false; // not found
  
  // get number of actors appearing in the film
  int numActorsOffset = movie.title.size() + 2;
  if (numActorsOffset % 2 != 0)
    numActorsOffset++;
  int numActors = *((short *) ((char *) movieFile + *offset + numActorsOffset));
  
  // get actors appearing in the film
  int actorsOffset = (numActorsOffset + 2) % 4 == 0 ? 2 : 4;
  for (int i = 0; i < numActors; i++) {
    int actorOffset = *((int *) ((char *) movieFile + *offset + numActorsOffset + actorsOffset) + i);
    string actorName = (const char *) actorFile + actorOffset;
    players.push_back(actorName);
  }
  
  return true;
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
