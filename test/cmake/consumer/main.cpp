#include <cli/cli.h>
#include <sstream>

int main()
{
    auto menu = std::make_unique<cli::Menu>("cli");
    menu->Insert("add", [](std::ostream& out, int a, int b){ out << a + b; });
    cli::Cli app(std::move(menu));
    std::ostringstream out;
    cli::CliSession session(app, out);
    session.Feed("add 2 3");
    return out.str() == "5" ? 0 : 1;
}
