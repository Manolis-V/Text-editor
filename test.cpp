#include <string>
using namespace std;
extern void runLexer(const string& input);

int main() {
    string input = R"(int main() {
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