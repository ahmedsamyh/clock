#include <clock/clock_rect.h>

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
  Vector2f tl = (Vector2f){r2.pos.x, r2.pos.y};
  Vector2f tr = (Vector2f){r2.pos.x + r2.size.x, r2.pos.y};
  Vector2f br = (Vector2f){r2.pos.x + r2.size.x, r2.pos.y + r2.size.y};
  Vector2f bl = (Vector2f){r2.pos.x, r2.pos.y + r2.size.y};

  return (Rect_contains_point(r1, tl) ||
	  Rect_contains_point(r1, tr) ||
	  Rect_contains_point(r1, br) ||
	  Rect_contains_point(r1, bl));
}
