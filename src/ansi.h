#ifndef BOOKSTORE_SRC_ANSI_H_
#define BOOKSTORE_SRC_ANSI_H_

class ANSI {
 public:
  enum Code {
    RESET = 0,
    // font style
    BOLD = 1,
    DIM = 2,
    ITALICS = 3,
    UNDERLINED = 4,
    BLINK = 5,
    REVERSE = 7,
    HIDDEN = 8,
    STRIKETHROUGH = 9,
    // foreground color
    FG_DEFAULT = 39,
    FG_BLACK = 30,
    FG_RED = 31,
    FG_GREEN = 32,
    FG_YELLOW = 33,
    FG_BLUE = 34,
    FG_MAGENTA = 35,
    FG_CYAN = 36,
    FG_LIGHT_GRAY = 37,
    FG_DARK_GRAY = 90,
    FG_LIGHT_RED = 91,
    FG_LIGHT_GREEN = 92,
    FG_LIGHT_YELLOW = 93,
    FG_LIGHT_BLUE = 94,
    FG_LIGHT_MAGENTA = 95,
    FG_LIGHT_CYAN = 96,
    FG_WHITE = 97,
    // background color
    BG_RED = 41,
    BG_GREEN = 42,
    BG_BLUE = 44,
    BG_MAGENTA = 45,
    BG_CYAN = 46,
    BG_LIGHT_GRAY = 47,
    BG_DEFAULT = 49,
    BG_DARK_Gray = 100,
    BG_LIGHT_RED = 101,
    BG_LIGHT_GREEN = 102,
    BG_LIGHT_YELLOW = 103,
    BG_LIGHT_BLUE = 104,
    BG_LIGHT_MAGENTA = 105,
    BG_LIGHT_CYAN = 106,
    BG_WHITE = 107
  };

 public:
  template <typename... Args>
  ANSI(const std::string& text, Args&&... args) : text(text), data{std::forward<Args>(args)...} {}

  friend std::ostream& operator<<(std::ostream& os, const ANSI& x) {
    for (Code code : x.data) {
      os << "\033[" << code << "m";
    }
    return os << x.text << "\033[0m";
  }

 private:
  const std::string& text;
  std::vector<Code> data;
};

std::ostream& operator<<(std::ostream& os, ANSI::Code x) { return os << "\033[" << int(x) << "m"; }

#endif //BOOKSTORE_SRC_ANSI_H_
