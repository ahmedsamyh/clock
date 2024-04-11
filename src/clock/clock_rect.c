#include <clock/clock_rect.h>
#include <commonlib.h>

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
      ASSERT(0 && "UNREACHABLE");
    }
    return true;
  }
  return false;
}

void Rect_get_3d_points(Rect rect, Vector3f* p0, Vector3f* p1, Vector3f* p2, Vector3f* p3) {
  Vector2f p0_ = v2f_add(rect.pos, (Vector2f){0.f, 0.f});
  Vector2f p1_ = v2f_add(rect.pos, (Vector2f){rect.size.x, 0.f});
  Vector2f p2_ = v2f_add(rect.pos, (Vector2f){rect.size.x, rect.size.y});
  Vector2f p3_ = v2f_add(rect.pos, (Vector2f){0.f, rect.size.y});

  *p0 = (Vector3f){p0_.x, p0_.y, 0.f};
  *p1 = (Vector3f){p1_.x, p1_.y, 0.f};
  *p2 = (Vector3f){p2_.x, p2_.y, 0.f};
  *p3 = (Vector3f){p3_.x, p3_.y, 0.f};
}

void Rect_get_points(Rect rect, Vector2f* p0, Vector2f* p1, Vector2f* p2, Vector2f* p3) {
  *p0 = v2f_add(rect.pos, (Vector2f){0.f, 0.f});
  *p1 = v2f_add(rect.pos, (Vector2f){rect.size.x, 0.f});
  *p2 = v2f_add(rect.pos, (Vector2f){rect.size.x, rect.size.y});
  *p3 = v2f_add(rect.pos, (Vector2f){0.f, rect.size.y});
}
