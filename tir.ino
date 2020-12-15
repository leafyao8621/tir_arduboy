#include <Arduboy2.h>

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define MARK 0x80
#define CHAR 0x7f

char grid[400];
unsigned score;
static const char *lookup = "!@#$%%^&*";

void core_init(void) {
    char temp = 0;
    char cont = 0;
    score = 0;
    for (char i = 0, *iter = grid; i < 20; ++i) {
        for (char j = 0; j < 20; ++j, ++iter) {
            cont = 1;
            for (; cont && (temp = lookup[rand() & 7]);) {
                cont = 0;
                if (i > 1) {
                    if (temp == iter[-20] && temp == iter[-40]) {
                        cont = 1;
                        continue;
                    }
                }
                if (j > 1) {
                    if (temp == iter[-1] && temp == iter[-2]) {
                        cont = 1;
                        continue;
                    }
                }
            }
            *iter = temp;
        }
    }
}


static void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

static char check(char r, char c) {
    char isr, ier, *sr, *er, isc, iec, *sc, *ec, tgt, *cur;
    cur = grid + r * 20 + c;
    sr = cur;
    er = cur;
    sc = cur;
    ec = cur;
    tgt = *cur & CHAR;
    isr = r;
    ier = r;
    isc = c;
    iec = c;
    for (;;) {
        if ((!isr || (isr && (sr[-20] & CHAR) != tgt)) &&
            (ier == 19 || (ier < 19 && (er[20] & CHAR) != tgt))) {
            break;
        }
        if (isr && (sr[-20] & CHAR) == tgt) {
            --isr;
            sr -= 20;
        }
        if (ier < 19 && (er[20] & CHAR) == tgt) {
            ++ier;
            er += 20;
        }
    }
    for (;;) {
        if ((!isc || (isc && (sc[-1] & CHAR) != tgt)) &&
            (iec == 19 || (iec < 19 && (ec[1] & CHAR) != tgt))) {
            break;
        }
        if (isc && (sc[-1] & CHAR) == tgt) {
            --isc;
            --sc;
        }
        if (iec < 19 && (ec[1] & CHAR) == tgt) {
            ++iec;
            ++ec;
        }
    }
    if ((ier - isr >= 2) || (iec - isc >= 2)) {
        if (ier - isr >= 2) {
            for (char *i = sr; i <= er; *i |= MARK, i += 20);
        }
        if (iec - isc >= 2) {
            for (char *i = sc; i <= ec; *(i++) |= MARK);
        }
        return 0;
    }
    return 1;
}

static void check_all(void) {
    for (char i = 0; i < 20; ++i) {
        for (char j = 0; j < 20; ++j) {
            check(i, j);
        }
    }
}

static char clear(void) {
    char cnt = 0;
    for (char i = 0, *iter = grid; i < 20; ++i) {
        for (char j = 0; j < 20; ++j, ++iter) {
            if (*iter & MARK) {
                *iter = 0x20;
                ++score;
                ++cnt;
            }
        }
    }
    return cnt;
}

static void drop(void) {
    for (char i = 0, *iter = grid; i < 20; ++i) {
        for (char j = 0; j < 20; ++j, ++iter) {
            if (*iter == 0x20) {
                for (char k = i, *cur = iter; k > 0; --k, cur -= 20) {
                    *cur = cur[-20];
                }
                grid[j] = lookup[rand() & 7];
            }
        }
    }
}

void core_move(char r, char c, char dir) {
    char *cur = grid + r * 20 + c;
    char a, b;
    a = 0;
    b = 0;
    switch (dir) {
    case UP:
        if (!r) {
            break;
        }
        swap(cur, cur - 20);
        a = check(r, c);
        b = check(r - 1, c);
        if (a && b) {
            swap(cur, cur - 20);
        } else {
            for (; clear(); drop(), check_all());
        }
        break;
    case DOWN:
        if (r > 18) {
            break;
        }
        swap(cur, cur + 20);
        a = check(r, c);
        b = check(r + 1, c);
        if (a && b) {
            swap(cur, cur + 20);
        } else {
            for (; clear(); drop(), check_all());
        }
        break;
    case LEFT:
        if (!c) {
            break;
        }
        swap(cur, cur - 1);
        a = check(r, c);
        b = check(r, c - 1);
        if (a && b) {
            swap(cur, cur - 1);
        } else {
            for (; clear(); drop(), check_all());
        }
        break;
    case RIGHT:
        if (c > 18) {
            break;
        }
        swap(cur, cur + 1);
        a = check(r, c);
        b = check(r, c + 1);
        if (a && b) {
            swap(cur, cur + 1);
        } else {
            for (; clear(); drop(), check_all());
        }
        break;
    }
}


int cur_state = 0;
int old_state = 0;
Arduboy2 arduboy;

void setup() {
  // put your setup code here, to run once:
  arduboy.begin();
  arduboy.setFrameRate(60);
  arduboy.audio.on();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!arduboy.nextFrame()) {
    return;
  }
  cur_state = arduboy.buttonsState();

  if (!(old_state & UP_BUTTON) && (cur_state & UP_BUTTON)) {
    Serial.println("UP");
  }
  if (!(old_state & DOWN_BUTTON) && (cur_state & DOWN_BUTTON)) {
    Serial.println("DOWN");
  }
  if (!(old_state & LEFT_BUTTON) && (cur_state & LEFT_BUTTON)) {
    Serial.println("LEFT");
  }
  if (!(old_state & RIGHT_BUTTON) && (cur_state & RIGHT_BUTTON)) {
    Serial.println("RIGHT");
  }
  if (!(old_state & A_BUTTON) && (cur_state & A_BUTTON)) {
    Serial.println("A");
  }
  if (!(old_state & B_BUTTON) && (cur_state & B_BUTTON)) {
    Serial.println("B");
  }

  old_state = cur_state;
  arduboy.display();
}
