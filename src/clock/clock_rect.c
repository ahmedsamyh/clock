#include <clock/clock_rect.h>
#include <assert.h>

bool Rect_contains_point(Rect r1, Vector2f p){
  return (p.x >= r1.pos.x && p.x < r1.pos.x + r1.size.x &&
	  p.y >= r1.pos.y && p.y < r1.pos.y + r1.size.y);
}

bool Rect_contains_rect(Rect r1, Rect r2){
  return (Rect_contains_point(r1, r2.pos) &&
	  Rect_contains_point(r1, (Vector2f){r2.pos.x + r2.size.x,
					     r2.pos.y + r2.size.y}));
}

bool Rect_intersects_rect(Rect r1, Rect r2){
  const float rect1_l = r1.pos.x;
  const float rect1_r = r1.pos.x+r1.size.x;
  const float rect1_t = r1.pos.y;
  const float rect1_b = r1.pos.y+r1.size.y;

  const float rect2_l = r2.pos.x;
  const float rect2_r = r2.pos.x+r2.size.x;
  const float rect2_t = r2.pos.y;
  const float rect2_b = r2.pos.y+r2.size.y;

  return (rect1_r >= rect2_l &&
	  rect1_l <= rect2_r &&
	  rect1_t <= rect2_b &&
	  rect1_b >= rect2_t);
}

bool Rect_resolve_rect_collision(Rect* rect1, const Rect rect2) {
  const float rect1_l = rect1->pos.x;
  const float rect1_r = rect1->pos.x+rect1->size.x;
  const float rect1_t = rect1->pos.y;
  const float rect1_b = rect1->pos.y+rect1->size.y;

  const float rect2_l = rect2.pos.x;
  const float rect2_r = rect2.pos.x+rect2.size.x;
  const float rect2_t = rect2.pos.y;
  const float rect2_b = rect2.pos.y+rect2.size.y;

  // resolve collision only if it ever happens
  if (Rect_intersects_rect(*rect1, rect2)){
    Vector2f cb2_bot = {0.f, rect2_b};
    Vector2f cb1_top = {0.f, rect1_t};
    float d2_top = v2f_mag2(v2f_sub(cb1_top, cb2_bot));
    Vector2f cb2_left = {rect2_l, 0.f};
    Vector2f cb1_right = {rect1_r, 0.f};
    float d2_right = v2f_mag2(v2f_sub(cb1_right, cb2_left));
    Vector2f cb2_right = {rect2_r, 0.f};
    Vector2f cb1_left = {rect1_l, 0.f};
    float d2_left = v2f_mag2(v2f_sub(cb1_left, cb2_right));
    Vector2f cb2_top = {0.f, rect2_t};
    Vector2f cb1_bot = {0.f, rect1_b};
    float d2_bot = v2f_mag2(v2f_sub(cb1_bot, cb2_top));

    float min_d2 = fminf(d2_top, fminf(d2_left, fminf(d2_right, d2_bot)));

    if (min_d2 == d2_top){
      rect1->pos.y = rect2_b;
    } else if (min_d2 == d2_left){
      rect1->pos.x = rect2_r;
    } else if (min_d2 == d2_right){
      rect1->pos.x = rect2_l - rect1->size.x;
    } else if (min_d2 == d2_bot){
      rect1->pos.y = rect2_t - rect1->size.y;
    } else {
      assert(0 && "UNREACHABLE");
    }
    return true;
  }
  return false;
}
