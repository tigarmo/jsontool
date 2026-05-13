#include <CLI/CLI.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <tabulate/table.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using json = nlohmann::json;

// Resolve a dot-notation path (e.g. "foo.bar.0") into the JSON value.
static const json* resolve_key(const json& root, const std::string& path)
{
    const json* node = &root;
    std::istringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '.')) {
        if (node->is_object() && node->contains(token)) {
            node = &(*node)[token];
        } else if (node->is_array()) {
            try {
                node = &(*node).at(std::stoul(token));
            } catch (...) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }
    return node;
}

// Render a JSON object as a two-column tabulate table.
static void print_table(const json& obj)
{
    tabulate::Table table;
    table.add_row({"Key", "Value"});
    table[0].format()
        .font_style({tabulate::FontStyle::bold})
        .font_color(tabulate::Color::cyan);

    for (auto& [k, v] : obj.items()) {
        std::string val = v.is_string() ? v.get<std::string>() : v.dump();
        table.add_row({k, val});
    }
    std::cout << table << "\n";
}

int main(int argc, char** argv)
{
    CLI::App app{"jsontool — pretty-print and query JSON files"};

    std::string filepath;
    std::string key;

    app.add_option("file", filepath, "Path to JSON file")->required();
    app.add_option("-k,--key", key, "Dot-notation key path to look up (e.g. foo.bar.0)");

    CLI11_PARSE(app, argc, argv);

    // Read file
    std::ifstream ifs(filepath);
    if (!ifs) {
        fmt::print(stderr, fmt::emphasis::bold | fg(fmt::color::red),
                   "Error: cannot open '{}'\n", filepath);
        return 1;
    }

    json root;
    try {
        ifs >> root;
    } catch (const json::parse_error& e) {
        fmt::print(stderr, fmt::emphasis::bold | fg(fmt::color::red),
                   "Parse error: {}\n", e.what());
        return 1;
    }

    if (key.empty()) {
        // Pretty-print mode
        if (root.is_object()) {
            print_table(root);
        } else {
            fmt::print("{}\n", root.dump(2));
        }
    } else {
        // Key-lookup mode
        const json* result = resolve_key(root, key);
        if (!result) {
            fmt::print(stderr, fmt::emphasis::bold | fg(fmt::color::yellow),
                       "Key '{}' not found\n", key);
            return 1;
        }
        if (result->is_string()) {
            fmt::print("{}\n", result->get<std::string>());
        } else {
            fmt::print("{}\n", result->dump(2));
        }
    }

    return 0;
}
