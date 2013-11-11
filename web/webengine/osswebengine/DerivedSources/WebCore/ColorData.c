/* ANSI-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -CDEot -L ANSI-C -k '*' -N findColor -D -s 2 /cygdrive/x/tot/WebCore/platform/ColorData.gperf  */
struct NamedColor { const char *name; int RGBValue; };
/* maximum key range = 1130, duplicates = 1 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133,   10,  105,  355,
         0,    0,   20,    5,   10,    0,    0,   15,    0,  110,
         0,    0,  250,    5,    0,   10,   15,  200,  290,  140,
      1133,  140,    5, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133, 1133,
      1133, 1133, 1133, 1133, 1133, 1133
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 20:
        hval += asso_values[(unsigned char)str[19]];
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const struct NamedColor *
findColor (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 149,
      MIN_WORD_LENGTH = 3,
      MAX_WORD_LENGTH = 20,
      MIN_HASH_VALUE = 3,
      MAX_HASH_VALUE = 1132
    };

  static const struct NamedColor wordlist[] =
    {
      {"red", 0xff0000},
      {"linen", 0xfaf0e6},
      {"gold", 0xffd700},
      {"green", 0x008000},
      {"indigo", 0x4b0082},
      {"goldenrod", 0xdaa520},
      {"indianred", 0xcd5c5c},
      {"orange", 0xffa500},
      {"orangered", 0xff4500},
      {"sienna", 0xa0522d},
      {"tan", 0xd2b48c},
      {"teal", 0x008080},
      {"darkred", 0x8b0000},
      {"seagreen", 0x2e8b57},
      {"darkgreen", 0x006400},
      {"darkgoldenrod", 0xb8860b},
      {"lightgreen", 0x90ee90},
      {"seashell", 0xfff5ee},
      {"darkorange", 0xff8c00},
      {"khaki", 0xf0e68c},
      {"thistle", 0xd8bfd8},
      {"forestgreen", 0x228b22},
      {"darkseagreen", 0x8fbc8f},
      {"lightseagreen", 0x20b2aa},
      {"darkkhaki", 0xbdb76b},
      {"lime", 0x00ff00},
      {"beige", 0xf5f5dc},
      {"limegreen", 0x32cd32},
      {"maroon", 0x800000},
      {"salmon", 0xfa8072},
      {"gainsboro", 0xdcdcdc},
      {"grey", 0x808080},
      {"snow", 0xfffafa},
      {"tomato", 0xff6347},
      {"magenta", 0xff00ff},
      {"gray", 0x808080},
      {"lawngreen", 0x7cfc00},
      {"darksalmon", 0xe9967a},
      {"white", 0xffffff},
      {"lightsalmon", 0xffa07a},
      {"darkgrey", 0xa9a9a9},
      {"wheat", 0xf5deb3},
      {"lightgrey", 0xd3d3d3},
      {"darkmagenta", 0x8b008b},
      {"darkgray", 0xa9a9a9},
      {"slategrey", 0x708090},
      {"lightgray", 0xd3d3d3},
      {"slategray", 0x708090},
      {"floralwhite", 0xfffaf0},
      {"ghostwhite", 0xf8f8ff},
      {"darkslategrey", 0x2f4f4f},
      {"azure", 0xf0ffff},
      {"lightslategrey", 0x778899},
      {"darkslategray", 0x2f4f4f},
      {"aqua", 0x00ffff},
      {"lightslategray", 0x778899},
      {"brown", 0xa52a2a},
      {"dimgrey", 0x696969},
      {"pink", 0xffc0cb},
      {"dimgray", 0x696969},
      {"palegreen", 0x98fb98},
      {"saddlebrown", 0x8b4513},
      {"palegoldenrod", 0xeee8aa},
      {"springgreen", 0x00ff7f},
      {"yellow", 0xffff00},
      {"mistyrose", 0xffe4e1},
      {"olive", 0x808000},
      {"greenyellow", 0xadff2f},
      {"yellowgreen", 0x9acd32},
      {"hotpink", 0xff69b4},
      {"honeydew", 0xf0fff0},
      {"lightpink", 0xffb6c1},
      {"silver", 0xc0c0c0},
      {"lavender", 0xe6e6fa},
      {"blue", 0x0000ff},
      {"whitesmoke", 0xf5f5f5},
      {"violet", 0xee82ee},
      {"violetred", 0xd02090},
      {"dodgerblue", 0x1e90ff},
      {"lightyellow", 0xffffe0},
      {"bisque", 0xffe4c4},
      {"darkolivegreen", 0x556b2f},
      {"lightgoldenrodyellow", 0xfafad2},
      {"darkblue", 0x00008b},
      {"steelblue", 0x4682b4},
      {"darkviolet", 0x9400d3},
      {"lightblue", 0xadd8e6},
      {"slateblue", 0x6a5acd},
      {"aquamarine", 0x7fffd4},
      {"coral", 0xff7f50},
      {"orchid", 0xda70d6},
      {"oldlace", 0xfdf5e6},
      {"lightsteelblue", 0xb0c4de},
      {"darkslateblue", 0x483d8b},
      {"lightslateblue", 0x8470ff},
      {"cornsilk", 0xfff8dc},
      {"darkorchid", 0x9932cc},
      {"rosybrown", 0xbc8f8f},
      {"lightcoral", 0xf08080},
      {"antiquewhite", 0xfaebd7},
      {"olivedrab", 0x6b8e23},
      {"sandybrown", 0xf4a460},
      {"ivory", 0xfffff0},
      {"turquoise", 0x40e0d0},
      {"navy", 0x000080},
      {"peru", 0xcd853f},
      {"midnightblue", 0x191970},
      {"mediumseagreen", 0x3cb371},
      {"royalblue", 0x4169e1},
      {"darkturquoise", 0x00ced1},
      {"skyblue", 0x87ceeb},
      {"crimson", 0xdc143c},
      {"navajowhite", 0xffdead},
      {"black", 0x000000},
      {"firebrick", 0xb22222},
      {"cyan", 0x00ffff},
      {"lightskyblue", 0x87cefa},
      {"deeppink", 0xff1493},
      {"lemonchiffon", 0xfffacd},
      {"darkcyan", 0x008b8b},
      {"lightcyan", 0xe0ffff},
      {"plum", 0xdda0dd},
      {"palevioletred", 0xd87093},
      {"burlywood", 0xdeb887},
      {"mintcream", 0xf5fffa},
      {"chartreuse", 0x7fff00},
      {"fuchsia", 0xff00ff},
      {"blanchedalmond", 0xffebcd},
      {"blueviolet", 0x8a2be2},
      {"lavenderblush", 0xfff0f5},
      {"aliceblue", 0xf0f8ff},
      {"cadetblue", 0x5f9ea0},
      {"paleturquoise", 0xafeeee},
      {"powderblue", 0xb0e0e6},
      {"purple", 0x800080},
      {"mediumspringgreen", 0x00fa9a},
      {"deepskyblue", 0x00bfff},
      {"mediumblue", 0x0000cd},
      {"mediumvioletred", 0xc71585},
      {"chocolate", 0xd2691e},
      {"mediumslateblue", 0x7b68ee},
      {"mediumaquamarine", 0x66cdaa},
      {"mediumorchid", 0xba55d3},
      {"cornflowerblue", 0x6495ed},
      {"moccasin", 0xffe4b5},
      {"mediumturquoise", 0x48d1cc},
      {"papayawhip", 0xffefd5},
      {"peachpuff", 0xffdab9},
      {"mediumpurple", 0x9370d8}
    };

  static const short lookup[] =
    {
        -1,   -1,   -1,    0,   -1,    1,   -1,   -1,
        -1,    2,    3,    4,   -1,   -1,    5,   -1,
        -1,   -1,   -1,    6,   -1,    7,   -1,   -1,
         8,   -1,    9,   -1,   10,   11,   -1,   -1,
        12,   13,   -1,   -1,   -1,   -1,   -1,   14,
        -1,   -1,   -1,   15,   -1,   16,   -1,   -1,
        17,   -1,   18,   -1,   -1,   -1,   -1,   19,
        -1,   20,   -1,   -1,   -1,   21,   22,   -1,
        -1,   -1,   -1,   -1,   23,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   24,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   25,   26,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   27,   -1,   28,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        29,   -1,   -1,   30,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   31,   -1,   -1,
        -1,   -1,   32,   -1,   33,   34,   -1,   35,
        -1,   -1,   -1,   -1,   36,   37,   -1,   -1,
        -1,   -1,   38,   39,   -1,   -1,   -1,   -1,
        -1,   -1,   40,   -1,   41,   -1,   -1,   -1,
        42,   -1,   43,   -1,   44,   45,   -1,   -1,
        -1,   -1,   46,   -1,   -1,   -1,   -1,   47,
        -1,   -1,   -1,   -1,   -1,   -1,   48,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   49,
        -1,   -1,   50,   -1,   51,   -1,   -1,   -1,
        52,   -1,   -1,   -1,   53,   54,   -1,   -1,
        -1,   -1,   55,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   56,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   57,   -1,
        -1,   -1,   -1,   -1,   -1,   58,   -1,   -1,
        59,   -1,   60,   -1,   61,   -1,   62,   -1,
        -1,   63,   -1,   -1,   -1,   -1,   64,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   65,   66,
      -449,   69,   70,  -82,   -2,   -1,   -1,   -1,
        71,   -1,   72,   -1,   73,   74,   75,   76,
        -1,   -1,   77,   -1,   -1,   -1,   -1,   -1,
        78,   79,   -1,   -1,   -1,   -1,   80,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   81,   82,
        -1,   -1,   83,   84,   85,   -1,   -1,   -1,
        86,   -1,   -1,   -1,   -1,   87,   -1,   -1,
        -1,   -1,   -1,   88,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   89,   90,   91,   -1,   92,   -1,
        -1,   -1,   93,   -1,   -1,   -1,   -1,   -1,
        94,   -1,   -1,   -1,   95,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        96,   -1,   -1,   -1,   97,   98,   -1,   99,
        -1,   -1,   -1,   -1,   -1,   -1,  100,  101,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  102,   -1,   -1,   -1,  103,
        -1,   -1,   -1,   -1,  104,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,  105,   -1,
        -1,  106,   -1,  107,   -1,   -1,   -1,   -1,
       108,   -1,   -1,   -1,  109,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  110,   -1,   -1,
        -1,   -1,  111,   -1,   -1,   -1,  112,   -1,
        -1,   -1,  113,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       114,   -1,   -1,   -1,   -1,  115,   -1,   -1,
       116,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  117,   -1,   -1,   -1,  118,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  119,   -1,   -1,   -1,   -1,   -1,
       120,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,  121,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  122,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  123,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,  124,  125,   -1,  126,   -1,  127,   -1,
        -1,   -1,   -1,   -1,  128,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,  129,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  130,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,  131,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  132,
        -1,  133,  134,  135,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,  136,   -1,   -1,   -1,  137,
        -1,   -1,   -1,   -1,  138,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  139,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,  140,
        -1,   -1,   -1,   -1,   -1,  141,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,  142,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  143,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       144,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,  145,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       146,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,  147,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,  148
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct NamedColor *wordptr = &wordlist[TOTAL_KEYWORDS + lookup[offset]];
              register const struct NamedColor *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->name;

                  if (*str == *s && !strcmp (str + 1, s + 1))
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}
