#include "maze.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <queue>

using namespace std;
using namespace cs225;

/**
 * No-parameter constructor.
 * Creates an empty maze.
 */
SquareMaze::SquareMaze() {
    w = 0;
    h = 0;
}

/**
 * Makes a new SquareMaze of the given height and width.
 *
 * If this object already represents a maze it will clear all the
 * existing data before doing so. You will start with a square grid (like
 * graph paper) with the specified height and width. You will select
 * random walls to delete without creating a cycle, until there are no
 * more walls that could be deleted without creating a cycle. Do not
 * delete walls on the perimeter of the grid.
 *
 * Hints: You only need to store 2 bits per square: the "down" and
 * "right" walls. The finished maze is always a tree of corridors.)
 *
 * @param width The width of the SquareMaze (number of cells)
 * @param height The height of the SquareMaze (number of cells)
 */
void SquareMaze::makeMaze(int width, int height) {
    w = width;
    h = height;
    s = width * height;

    sets.addelements(s);
    rWall.clear();
    dWall.clear();
    rWall.resize(s, true);
    dWall.resize(s, true);

    int visitedCells = 0;
    while (visitedCells < s - 1) {
        int x = rand() % w;
        int y = rand() % h;
        int dir = rand() % 4;

        int i = x, j = y;
        switch (dir) {
            case 0:
                i += 1;
                break;
            case 1:
                j += 1;
                break;
            case 2:
                if (x > 0)
                    i -= 1;
                break;
            case 3:
                if (y > 0)
                    j -= 1;
                break;
        }
        if (i < 0 || j < 0 || i >= w || j >= h)
            continue;

        int currentCell = y * w + x;
        int nextCell = j * w + i;

        if (sets.find(currentCell) != sets.find(nextCell)) {
            if (dir == 0 && x != w - 1) {
                setWall(x, y, 0, false);
                sets.setunion(currentCell, nextCell);
                visitedCells++;
            } else if (dir == 1 && y != h - 1) {
                setWall(x, y, 1, false);
                sets.setunion(currentCell, nextCell);
                visitedCells++;
            } else if (dir == 2 && x > 0) {
                setWall(i, j, 0, false);
                sets.setunion(currentCell, nextCell);
                visitedCells++;
            } else if (dir == 3 && y > 0) {
                setWall(i, j, 1, false);
                sets.setunion(currentCell, nextCell);
                visitedCells++;
            }
        }
    }
}

/**
 * This uses your representation of the maze to determine whether it is
 * possible to travel in the given direction from the square at
 * coordinates (x,y).
 *
 * For example, after makeMaze(2,2), the possible input coordinates will
 * be (0,0), (0,1), (1,0), and (1,1).
 *
 * - dir = 0 represents a rightward step (+1 to the x coordinate)
 * - dir = 1 represents a downward step (+1 to the y coordinate)
 * - dir = 2 represents a leftward step (-1 to the x coordinate)
 * - dir = 3 represents an upward step (-1 to the y coordinate)
 *
 * You can not step off of the maze or through a wall.
 *
 * This function will be very helpful in solving the maze. It will also
 * be used by the grading program to verify that your maze is a tree that
 * occupies the whole grid, and to verify your maze solution. So make
 * sure that this function works!
 *
 * @param x The x coordinate of the current cell
 * @param y The y coordinate of the current cell
 * @param dir The desired direction to move from the current cell
 * @return whether you can travel in the specified direction
 */
bool SquareMaze::canTravel(int x, int y, int dir) const {
    bool canMove = false;
    int index;

    switch (dir) {
        case 0: // Right
            index = y * w + x;
            canMove = (x < w - 1 && !rWall.at(index));
            break;
        case 1: // Down
            index = y * w + x;
            canMove = (y < h - 1 && !dWall.at(index));
            break;
        case 2: // Left
            index = y * w + x - 1;
            canMove = (x > 0 && !rWall.at(index));
            break;
        case 3: // Up
            index = (y - 1) * w + x;
            canMove = (y > 0 && !dWall.at(index));
            break;
        default:
            canMove = false;
    }

    return canMove;
}

/**
 * Sets whether or not the specified wall exists.
 *
 * This function should be fast (constant time). You can assume that in
 * grading we will not make your maze a non-tree and then call one of the
 * other member functions. setWall should not prevent cycles from
 * occurring, but should simply set a wall to be present or not present.
 * Our tests will call setWall to copy a specific maze into your
 * implementation.
 *
 * @param x The x coordinate of the current cell
 * @param y The y coordinate of the current cell
 * @param dir Either 0 (right) or 1 (down), which specifies which wall to
 * set (same as the encoding explained in canTravel). You only need to
 * support setting the bottom and right walls of every square in the grid.
 * @param exists true if setting the wall to exist, false otherwise
 */
void SquareMaze::setWall(int x, int y, int dir, bool exists) {
    int index = y * w + x;
    switch (dir) {
        case 0:
            rWall.at(index) = exists;
            break;
        case 1:
            dWall.at(index) = exists;
            break;
    }
}

/**
 * Solves this SquareMaze.
 *
 * For each square on the bottom row (maximum y coordinate), there is a
 * distance from the origin (i.e. the top-left cell), which is defined as
 * the length (measured as a number of steps) of the only path through
 * the maze from the origin to that square.
 *
 * Select the square in the bottom row with the largest distance from the
 * origin as the destination of the maze. solveMaze() returns the
 * winning path from the origin to the destination as a vector of
 * integers, where each integer represents the direction of a step, using
 * the same encoding as in canTravel().
 *
 * If multiple paths of maximum length exist, use the one with the
 * destination cell that has the smallest x value.
 *
 * Hint: this function should run in time linear in the number of cells
 * in the maze.
 *
 * @return a vector of directions taken to solve the maze
 */
std::vector<int> SquareMaze::solveMaze() {
    std::vector<int> distance(w * h, -1);
    std::vector<int> predecessor(w * h, -1);
    std::queue<int> queue;

    queue.push(0);
    distance[0] = 0;

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        int x = current % w;
        int y = current / w;

        for (int direction = 0; direction < 4; direction++) {
            int i = x, j = y;
            switch (direction) {
                case 0:
                    i += 1;
                    break;
                case 1:
                    j += 1;
                    break;
                case 2:
                    i -= 1;
                    break;
                case 3:
                    j -= 1;
                    break;
            }
            int next = j * w + i;

            if (i >= 0 && i < w && j >= 0 && j < h && canTravel(x, y, direction) && distance[next] == -1) {
                queue.push(next);
                distance[next] = distance[current] + 1;
                predecessor[next] = current;
            }
        }
    }

    int maxDistance = 0, destination = 0;
    for (int i = 0; i < w; i++) {
        int cell = (h - 1) * w + i;
        if (distance[cell] > maxDistance) {
            maxDistance = distance[cell];
            destination = cell;
        }
    }

    std::vector<int> path;
    for (int i = destination; i != 0; i = predecessor[i]) {
        int dx = i % w - predecessor[i] % w;
        int dy = i / w - predecessor[i] / w;
        if (dx == 1)
            path.push_back(0);
        else if (dy == 1)
            path.push_back(1);
        else if (dx == -1)
            path.push_back(2);
        else if (dy == -1)
            path.push_back(3);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * Draws the maze without the solution.
 *
 * First, create a new PNG. Set the dimensions of the PNG to
 * (width*10+1,height*10+1). where height and width were the arguments to
 * makeMaze. Blacken the entire topmost row and leftmost column of
 * pixels, except the entrance (1,0) through (9,0).  For each square in
 * the maze, call its maze coordinates (x,y). If the right wall exists,
 * then blacken the pixels with coordinates ((x+1)*10,y*10+k) for k from
 * 0 to 10. If the bottom wall exists, then blacken the pixels with
 * coordinates (x*10+k, (y+1)*10) for k from 0 to 10.
 *
 * The resulting PNG will look like the sample image, except there will
 * be no exit from the maze and the red line will be missing.
 *
 * @return a PNG of the unsolved SquareMaze
 */
cs225::PNG* SquareMaze::drawMaze() const {
    cs225::PNG* output = new cs225::PNG(w * 10 + 1, h * 10 + 1);

    for (int i = 0; i < w * 10 + 1; i++) {
        if (i < 1 || i > 9) {
            HSLAPixel &oPixel = output->getPixel(i, 0);
            oPixel.l = 0;
        }
    }
    for (int i = 0; i < h * 10 + 1; i++) {
        HSLAPixel &oPixel = output->getPixel(0, i);
        oPixel.l = 0;
    }
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (!canTravel(x, y, 0)) {
                for (int k = 0; k <= 10; ++k) {
                    output->getPixel((x + 1) * 10, y * 10 + k).l = 0;
                }
            }

            if (!canTravel(x, y, 1)) {
                for (int k = 0; k <= 10; ++k) {
                    output->getPixel(x * 10 + k, (y + 1) * 10).l = 0;
                }
            }
        }
    }

    return output;
}

/**
 * This function calls drawMaze, then solveMaze; it modifies the PNG
 * from drawMaze to show the solution vector and the exit.
 *
 * Start at pixel (5,5). Each direction in the solution vector
 * corresponds to a trail of 11 red pixels in the given direction. If the
 * first step is downward, color pixels (5,5) through (5,15) red. (Red is
 * 0,1,0.5,1 in HSLA). Then if the second step is right, color pixels (5,15)
 * through (15,15) red. Then if the third step is up, color pixels
 * (15,15) through (15,5) red. Continue in this manner until you get to
 * the end of the solution vector, so that your output looks analogous
 * the above picture.
 *
 * Make the exit by undoing the bottom wall of the destination square:
 * call the destination maze coordinates (x,y), and whiten the pixels
 * with coordinates (x*10+k, (y+1)*10) for k from 1 to 9.
 *
 * @return a PNG of the solved SquareMaze
 */
cs225::PNG* SquareMaze::drawMazeWithSolution() {
    cs225::PNG* maze = drawMaze();
    std::vector<int> solution = solveMaze();
    int x = 5;
    int y = 5;

    for (size_t i = 0; i < solution.size(); i++) {
        for (int j = 0; j < 10; j++) {
            HSLAPixel& pixel = maze->getPixel(x, y);
            pixel.h = 0;
            pixel.s = 1.0;
            pixel.l = 0.5;

            switch (solution[i]) {
                case 0:
                    x++;
                    break;
                case 1:
                    y++;
                    break;
                case 2:
                    x--;
                    break;
                case 3:
                    y--;
                    break;
            }
        }
    }
    HSLAPixel& oPixel = maze->getPixel(x, y);
    oPixel.h = 0;
    oPixel.s = 1.0;
    oPixel.l = 0.5;

    x
    return maze;
}
