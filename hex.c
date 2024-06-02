#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <raylib.h>

#define MATH_IMPL
#include "math.h"

#define arrlen(a) (size_t)(sizeof(a) / sizeof(*(a)))

typedef struct {
  enum {
    HEX_EMPTY,
    HEX_RED,
    HEX_BLUE,
  } board[11][11];
  float values[11][11];
  int w, h;
  int turn_num;
} Game;

int get_color(Game *g) {
  return g->turn_num % 2 + 1;
}

void draw_hex(Game *g, int x, int y, bool draw_lines, int unit) {
  Vector2 pos = (Vector2){
    SQRT3/2 * unit/2 + x * unit + y * unit/2,
    SQRT3/3 * unit + y * unit * SQRT3/2,
  };

  if (draw_lines) {
    DrawPolyLinesEx(pos, 6, unit * SQRT3/3, 90, 0.8, BLACK);
  } else {
    Color c;
    if (g->board[x][y] == HEX_EMPTY) c = WHITE;
    else if (g->board[x][y] == HEX_BLUE) c = BLUE;
    else if (g->board[x][y] == HEX_RED) c = RED;
    // c.a *= g->values[x][y];
    DrawPoly(pos, 6, unit * SQRT3/3, 90, c);
  }
}

bool check_connect(Game *g, Game *t, int x, int y) {
  t->board[x][y] = 1;

  int color = g->board[x][y];

  if (color == HEX_RED) {
    if (y == g->h - 1) return true;
  } else {
    if (x == g->w - 1) return true;
  }

  struct {
    int x, y;
  } nbors[6] = {
    { x - 1, y, },
    { x + 1, y, },
    { x, y - 1, },
    { x + 1, y - 1, },
    { x, y + 1, },
    { x - 1, y + 1, },
  };

  for (int i = 0; i < arrlen(nbors); i++) {
    int x = nbors[i].x,
        y = nbors[i].y;

    if (x < 0 || y < 0) continue;
    if (x >= g->w || y >= g->h) continue;

    if (!t->board[x][y] && g->board[x][y] == color) {
      if (check_connect(g, t, x, y)) return true;
    }
  }

  return false;
}

int check_win(Game *g) {
  for (int x = 0; x < g->w; x++) {
    if (g->board[x][0] != HEX_RED) continue;

    Game t = { 0 };
    if (check_connect(g, &t, x, 0)) return HEX_RED;
  }

  for (int y = 0; y < g->h; y++) {
    if (g->board[0][y] != HEX_BLUE) continue;

    Game t = { 0 };
    if (check_connect(g, &t, 0, y)) return HEX_BLUE;
  }

  return HEX_EMPTY;
}

void make_move(Game *g, int x, int y) {
  g->board[x][y] = get_color(g);
  g->turn_num += 1;
}

int main() {
  srand(time(0));

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "Hex");
  SetExitKey(KEY_Q);

  Game game = { .w = arrlen(game.board), .h = arrlen(game.board[0]), };

  float timer = 0;

  // int render_size = 0;
  int unit = 0;
  bool initial = true;

  while (!WindowShouldClose()) {
    if (IsWindowResized() || initial) {
      // render_size = minf(GetRenderWidth(), GetRenderHeight());
      unit = minf(
        GetRenderWidth() / (game.w + game.h/2),
        GetRenderHeight() / game.h
      );
    }

    /* ===== Drawing ===== */
    BeginDrawing();
    ClearBackground(BLACK);
    {
      for (int y = 0; y < game.h; y++) {
        for (int x = 0; x < game.w; x++) {
          draw_hex(&game, x, y, 0, unit);
        }
      }
      for (int y = 0; y < game.h; y++) {
        for (int x = 0; x < game.w; x++) {
          draw_hex(&game, x, y, 1, unit);
        }
      }
    }

    /* ===== Simulation ===== */
    {
      if (timer != 128) timer -= GetFrameTime();
      if (timer <= 0 && timer != 128) {
        timer = 0.0;

        int color = get_color(&game);

        int bx = 0, by = 0;
        float bwin_rate = -1;

        for (int y = 0; y < game.h; y++) {
          for (int x = 0; x < game.w; x++) {
            if (game.board[x][y]) continue;

            float win_rate = 0.0;
            const int num_games = 100;

            for (int i = 0; i < num_games; i++) {
              Game n = game;
              make_move(&n, x, y);

              while (true) {
                int win = check_win(&n);
                if (win == color) win_rate += 1;
                if (win) break;

                int x = 0, y = 0;
                do {
                  x = randi(0, n.w - 1);
                  y = randi(0, n.h - 1);
                } while (n.board[x][y]);

                n.board[x][y] = get_color(&n);
                n.turn_num += 1;
              }
            }

            win_rate /= num_games;
            game.values[x][y] = win_rate;
            if (win_rate > bwin_rate) {
              bwin_rate = win_rate;
              bx = x;
              by = y;
            }
          }
        }

        game.board[bx][by] = color;
        game.turn_num += 1;
        printf("%f %d %d\n", bwin_rate, bx, by);

        int win = check_win(&game);
        if (win) {
          timer = 128;
          printf("Yes! %d\n", win);
        }
      }
    }

    EndDrawing();

    initial = false;
  }

  CloseWindow();
  return 0;
}
