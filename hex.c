#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <raylib.h>

#define arrlen(a) (size_t)(sizeof(a) / sizeof(*(a)))
#define SQRT3 1.73205080757

int minf(int x, int y) {
  return x < y ? x : y;
}

int main() {
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "Hex");
  SetTargetFPS(60);
  SetExitKey(KEY_Q);

  struct {
    enum {
      HEX_EMPTY,
      HEX_RED,
      HEX_BLUE,
    } board[11][11];
    int w, h;
  } game = { .w = arrlen(game.board[0]), .h = arrlen(game.board), };

  int render_size = 0;
  int unit = 0;

  bool initial = true;
  while (!WindowShouldClose()) {
    if (IsWindowResized() || initial) {
      render_size = minf(GetRenderWidth(), GetRenderHeight());
      unit = render_size / minf(game.w, game.h);
    }

    BeginDrawing();
    ClearBackground(RED);
    {
      for (int i = 0; i < game.h; i++) {
        for (int j = 0; j < game.w; j++) {
          DrawPolyLines(
            (Vector2){ unit/2 + j * unit + i * unit/2, unit/2 + i * unit * SQRT3/2},
            6,
            unit * SQRT3/3,
            90,
            WHITE
          );
        }
      }
    }
    EndDrawing();

    initial = false;
  }

  CloseWindow();
  return 0;
}
