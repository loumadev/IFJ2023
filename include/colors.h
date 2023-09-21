#define BLACK "\033[30m"
#define DARK_BLUE "\033[34m"
#define DARK_GREEN "\033[32m"
#define DARK_AQUA "\033[36m"
#define DARK_RED "\033[31m"
#define DARK_PURPLE "\033[35m"
#define GOLD "\033[33m"
#define GREY "\033[37m"
#define DARK_GREY "\033[90m"
#define BLUE "\033[94m"
#define GREEN "\033[92m"
#define AQUA "\033[96m"
#define RED "\033[91m"
#define PURPLE "\033[95m"
#define YELLOW "\033[93m"
#define WHITE "\033[97m"

#define C0 BLACK
#define C1 DARK_BLUE
#define C2 DARK_GREEN
#define C3 DARK_AQUA
#define C4 DARK_RED
#define C5 DARK_PURPLE
#define C6 GOLD
#define C7 GREY
#define C8 DARK_GREY
#define C9 BLUE
#define Ca GREEN
#define Cb AQUA
#define Cc RED
#define Cd PURPLE
#define Ce YELLOW
#define Cf WHITE

#define BOLD "\033[1m"
#define ITALIC "\033[3m"
#define UNDERLINE "\033[4m"
#define STRIKETHROUGH "\033[9m"
#define HIDDEN "\033[8m"
#define INVERT "\033[7m"
#define BLINK "\033[5m"

#define RESET "\033[0m"
#define RST RESET

/*

   DEMO:
   ```c
   printf(
        C0 "C0  " RESET
        C1 "C1  " RESET
        C2 "C2  " RESET
        C3 "C3  " RESET
        C4 "C4  " RESET
        C5 "C5  " RESET
        C6 "C6  " RESET
        C7 "C7  " RESET
        C8 "C8  " RESET
        C9 "C9  " RESET
        Ca "Ca  " RESET
        Cb "Cb  " RESET
        Cc "Cc  " RESET
        Cd "Cd  " RESET
        Ce "Ce  " RESET
        Cf "Cf  " RESET
        "\n"
        BOLD "BOLD" RESET "\n"
        ITALIC "ITALIC" RESET "\n"
        UNDERLINE "UNDERLINE" RESET "\n"
        STRIKETHROUGH "STRIKETHROUGH" RESET "\n"
        HIDDEN "HIDDEN" RESET "\n"
        INVERT "INVERT" RESET "\n"
        BLINK "BLINK" RESET "\n"
   );
   ```

 */