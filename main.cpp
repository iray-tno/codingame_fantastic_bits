#pragma GCC optimize "-O3"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>

using namespace std;

#ifdef _MSC_VER
#include <Windows.h>
    double get_ms() { return (double)GetTickCount64() / 1000; }
#else
#include <sys/time.h>
    double get_ms() { struct timeval t; gettimeofday(&t, NULL); return (double)t.tv_sec * 1000 + (double)t.tv_usec / 1000; }
#endif

class Timer {
 private:
  double start_time;

#ifdef USE_RDTSC
    double get_msec() { return get_absolute_sec() * 1000; }
#else
    double get_msec() { return get_ms(); }
#endif

public:
    Timer() {}

    void start() { start_time = get_msec(); }
    double get_elapsed() { return get_msec() - start_time; }
};

class Entity {
 public:
  int entityType;
  int owner;
  int x, y;
  int param1, param2;

  Entity() {}

  Entity(
    int _entityType,
    int _owner,
    int _x,
    int _y,
    int _param1,
    int _param2
  ) :
    entityType(_entityType),
    owner(_owner),
    x(_x),
    y(_y),
    param1(_param1),
    param2(_param2) {}

  string toString() {
    stringstream ss;
    ss << "\n";
    ss << "Entity >";
    ss << "type:" << entityType << "\n";
    ss << "owner: " << owner << "\n";
    ss << "pos: " << x << ", " << y << "\n";
    ss << "params: " << param1 << ", " << param2 << "\n";
    return ss.str();
  }

  static Entity buildFromInput() {
    int entityType;
    int owner;
    int x, y;
    int param1, param2;
    cin >> entityType >> owner >> x >> y >> param1 >> param2; cin.ignore();
    return Entity(
      entityType,
      owner,
      x, y,
      param1, param2
    );
  }
};

class Brain {

};

class StageInfo {
 public:
  int width, height, myId;
  StageInfo(
    int _width,
    int _height,
    int _myId) :
      width(_width),
      height(_height),
      myId(_myId) {}

  static StageInfo buildFromInput() {
    int width, height, myId;
    cin >> width >> height >> myId; cin.ignore();
    return StageInfo(width, height, myId);
  }
};

vector<vector<bool>> buildIsWallMap(StageInfo stageInfo, vector<string> map) {
  vector<vector<bool>> results(
    stageInfo.height,
    vector<bool>(stageInfo.width, false)
  );

  for (int i = 0; i < stageInfo.height; ++i) {
    for (int j = 0; j < stageInfo.width; ++j) {
      if (map[i][j] != '.') { results[i][j] = true; }
    }
  }
  return results;
}

vector<vector<bool>> buildIsBoxMap(StageInfo stageInfo, vector<string> map) {
  vector<vector<bool>> results(
    stageInfo.height,
    vector<bool>(stageInfo.width, false)
  );

  for (int i = 0; i < stageInfo.height; ++i) {
    for (int j = 0; j < stageInfo.width; ++j) {
      if (map[i][j] != '.' && map[i][j] != 'X') { results[i][j] = true; }
    }
  }
  return results;
}

vector<string> buildStrMapFromInput(StageInfo stageInfo) {
  vector<string> results(stageInfo.height);
  for (int i = 0; i < stageInfo.height; i++) {
    string row;
    cin >> row;
    results[i] = row;
  }
  return results;
}

vector<vector<bool>> buildIsBombMap(StageInfo stageInfo, vector<Entity> entities) {
  vector<vector<bool>> results(
    stageInfo.height,
    vector<bool>(stageInfo.width, false)
  );
  for (auto entity : entities) {
    if (entity.entityType == 1) {
      results[entity.y][entity.x] = true;
    }
  }
  return results;
}

vector<vector<int>> buildScoreMap(
  StageInfo stageInfo,
  vector<vector<bool>> IsWallMap,
  vector<vector<bool>> IsBombMap,
  vector<vector<bool>> IsBoxMap
) {
  vector<vector<int>> results(
    stageInfo.height,
    vector<int>(stageInfo.width, 0)
  );

  int h = 0, w = 0, wallDist = 0, bombDist = 0;
  auto wallStep = [&]() {
    if (wallDist != 0) {
      ++results[h][w];
      --wallDist;
    }
    if (IsWallMap[h][w]) {
      wallDist = 0;
      results[h][w] = 0;
    }
    if (IsBoxMap[h][w]) {
      wallDist = 3;
      results[h][w] = 0;
    }
  };
  auto bombStep = [&]() {
    if (bombDist != 0) {
      results[h][w] = 0;
      --bombDist;
    }
    if (IsWallMap[h][w]) {
      bombDist = 0;
      results[h][w] = 0;
    }
    if (IsBombMap[h][w]) {
      bombDist = 3;
      results[h][w] = 0;
    }
  };
  for(h = 0; h < stageInfo.height; ++h) {
    wallDist = 0;
    for(w = 0; w < stageInfo.width; ++w) {
      wallStep();
    }
    wallDist = 0;
    for(w = stageInfo.width - 1; 0 <= w; --w) {
      wallStep();
    }
    bombDist = 0;
    for(w = 0; w < stageInfo.width; ++w) {
      bombStep();
    }
    bombDist = 0;
    for(w = stageInfo.width - 1; 0 <= w; --w) {
      bombStep();
    }
  }

  for(w = 0; w < stageInfo.width; ++w) {
    wallDist = 0;
    for(h = 0; h < stageInfo.height; ++h) {
      wallStep();
    }
    wallDist = 0;
    for(h = stageInfo.height - 1; 0 <= h; --h) {
      wallStep();
    }
    bombDist = 0;
    for(h = 0; h < stageInfo.height; ++h) {
      bombStep();
    }
    bombDist = 0;
    for(h = stageInfo.height - 1; 0 <= h; --h) {
      bombStep();
    }
  }

  for(int h = 0; h < stageInfo.height; ++h) {
    for(int w = 0; w < stageInfo.width; ++w) {
      cerr << results[h][w] << " ";
    }
    cerr << endl;
  }
  return results;
}


/**
 * Grab Snaffles and try to throw them through the opponent's goal!
 * Move towards a Snaffle and use your team id to determine where you need to throw it.
 **/
int main()
{
    int myTeamId; // if 0 you need to score on the right of the map, if 1 you need to score on the left
    cin >> myTeamId; cin.ignore();

    // game loop
    while (1) {
        int entities; // number of entities still in game
        cin >> entities; cin.ignore();
        for (int i = 0; i < entities; i++) {
            int entityId; // entity identifier
            string entityType; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
            int x; // position
            int y; // position
            int vx; // velocity
            int vy; // velocity
            int state; // 1 if the wizard is holding a Snaffle, 0 otherwise
            cin >> entityId >> entityType >> x >> y >> vx >> vy >> state; cin.ignore();
        }
        for (int i = 0; i < 2; i++) {

            // Write an action using cout. DON'T FORGET THE "<< endl"
            // To debug: cerr << "Debug messages..." << endl;


            // Edit this line to indicate the action for each wizard (0 <= thrust <= 150, 0 <= power <= 500)
            // i.e.: "MOVE x y thrust" or "THROW x y power"
            cout << "MOVE 8000 3750 100" << endl;
        }
    }
}