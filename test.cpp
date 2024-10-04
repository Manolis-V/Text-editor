#include <string>
extern void runLexer(const std::string& input);

int main() {
    std::string input = R"(int main() {
    float num = 3.14;
    // This is a comment
    if (num > 2) {
        return 1;
    } else {
        return 0;
    }
})";
    runLexer(input);
    return 0;
}
